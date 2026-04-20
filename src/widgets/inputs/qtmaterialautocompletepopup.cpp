#include "qtmaterial/widgets/inputs/qtmaterialautocompletepopup.h"

#include <QAbstractItemModel>
#include <QEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QListView>
#include <QPainter>
#include <QPointer>
#include <QSortFilterProxyModel>
#include <QStringListModel>
#include <QStyleOption>
#include <QVBoxLayout>

#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialshadowrenderer.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"

QtMaterialAutocompletePopup::QtMaterialAutocompletePopup(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_Hover, true);
    setFocusPolicy(Qt::StrongFocus);
    setWindowFlag(Qt::Popup, true);
    setAutoFillBackground(false);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_view = new QListView(this);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_view->setUniformItemSizes(false);
    layout->addWidget(m_view);

    m_ownedStringModel = new QStringListModel(this);
    m_model = m_ownedStringModel;
    m_view->setModel(m_model);

    connect(m_view, &QListView::activated, this, [this](const QModelIndex& index) {
        if (!index.isValid()) {
            return;
        }
        emit completionActivated(index.data(Qt::DisplayRole).toString());
        setPopupVisible(false);
    });
}

QtMaterialAutocompletePopup::~QtMaterialAutocompletePopup() = default;

void QtMaterialAutocompletePopup::setAnchorLineEdit(QLineEdit* lineEdit)
{
    if (m_anchorLineEdit == lineEdit) {
        return;
    }

    if (m_anchorLineEdit) {
        m_anchorLineEdit->removeEventFilter(this);
    }

    m_anchorLineEdit = lineEdit;

    if (m_anchorLineEdit) {
        m_anchorLineEdit->installEventFilter(this);
    }

    updatePopupGeometry();
}

QLineEdit* QtMaterialAutocompletePopup::anchorLineEdit() const noexcept
{
    return m_anchorLineEdit;
}

void QtMaterialAutocompletePopup::setModel(QAbstractItemModel* model)
{
    if (!model) {
        model = m_ownedStringModel;
    }
    m_model = model;
    m_view->setModel(model);
    invalidatePopupLayout();
}

QAbstractItemModel* QtMaterialAutocompletePopup::model() const noexcept
{
    return m_model;
}

void QtMaterialAutocompletePopup::setSuggestions(const QStringList& suggestions)
{
    m_ownedStringModel->setStringList(suggestions);
    if (m_model == m_ownedStringModel) {
        invalidatePopupLayout();
    }
}

QString QtMaterialAutocompletePopup::currentCompletion() const
{
    const QModelIndex index = m_view->currentIndex();
    return index.isValid() ? index.data(Qt::DisplayRole).toString() : QString();
}

void QtMaterialAutocompletePopup::setPopupVisible(bool visible)
{
    if (m_popupVisible == visible) {
        return;
    }
    m_popupVisible = visible;
    if (visible) {
        updatePopupGeometry();
        show();
        raise();
    } else {
        hide();
    }
    emit popupVisibilityChanged(visible);
}

bool QtMaterialAutocompletePopup::isPopupVisible() const noexcept
{
    return m_popupVisible;
}

void QtMaterialAutocompletePopup::setFilterText(const QString& text)
{
    if (m_filterText == text) {
        return;
    }
    m_filterText = text;
    // Filtering model hookup intentionally left for integration branch.
    invalidatePopupLayout();
}

QString QtMaterialAutocompletePopup::filterText() const
{
    return m_filterText;
}

void QtMaterialAutocompletePopup::clearSelection()
{
    m_view->clearSelection();
    m_view->setCurrentIndex(QModelIndex());
}

void QtMaterialAutocompletePopup::selectNext()
{
    QModelIndex current = m_view->currentIndex();
    const int row = current.isValid() ? current.row() + 1 : 0;
    const QModelIndex next = m_view->model() ? m_view->model()->index(row, 0) : QModelIndex();
    if (next.isValid()) {
        m_view->setCurrentIndex(next);
        m_view->scrollTo(next);
    }
}

void QtMaterialAutocompletePopup::selectPrevious()
{
    QModelIndex current = m_view->currentIndex();
    const int row = current.isValid() ? current.row() - 1 : 0;
    const QModelIndex prev = m_view->model() ? m_view->model()->index(qMax(0, row), 0) : QModelIndex();
    if (prev.isValid()) {
        m_view->setCurrentIndex(prev);
        m_view->scrollTo(prev);
    }
}

void QtMaterialAutocompletePopup::acceptCurrent()
{
    const QString text = currentCompletion();
    if (!text.isEmpty()) {
        emit completionActivated(text);
        setPopupVisible(false);
    }
}

QSize QtMaterialAutocompletePopup::sizeHint() const
{
    ensureSpecResolved();
    return m_spec.minPopupSize.expandedTo(QSize(width(), qMin(m_spec.maxPopupSize.height(), m_spec.itemMinSize.height() * m_spec.visibleItemCount)));
}

QSize QtMaterialAutocompletePopup::minimumSizeHint() const
{
    ensureSpecResolved();
    return m_spec.minPopupSize;
}

bool QtMaterialAutocompletePopup::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_anchorLineEdit) {
        switch (event->type()) {
        case QEvent::Move:
        case QEvent::Resize:
        case QEvent::Show:
            updatePopupGeometry();
            break;
        case QEvent::Hide:
            setPopupVisible(false);
            break;
        default:
            break;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void QtMaterialAutocompletePopup::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    ensureSpecResolved();

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    const QRectF rect = this->rect().adjusted(1, 1, -1, -1);
    const qreal radius = m_spec.cornerRadius;

    // QtMaterialFocusIndicator::paintRectFocusRing(&p, rect, m_spec.focusRingColor, radius, m_spec.focusRingWidth);
    // QtMaterialShadowRenderer::paintRoundedShadow(&p, rect, radius, m_spec.shadowColor,
    //                                              static_cast<int>(m_spec.elevationRole), 2);

    QPainterPath path;
    path.addRoundedRect(rect, radius, radius);
    p.fillPath(path, m_spec.containerColor);
}

void QtMaterialAutocompletePopup::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    invalidatePopupLayout();
}

void QtMaterialAutocompletePopup::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Down:
        selectNext();
        event->accept();
        return;
    case Qt::Key_Up:
        selectPrevious();
        event->accept();
        return;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        acceptCurrent();
        event->accept();
        return;
    case Qt::Key_Escape:
        setPopupVisible(false);
        event->accept();
        return;
    default:
        break;
    }
    QWidget::keyPressEvent(event);
}

void QtMaterialAutocompletePopup::ensureSpecResolved() const
{
    if (!m_specDirty) {
        return;
    }

    QtMaterial::SpecFactory factory;
    m_spec = factory.autocompletePopupSpec(QtMaterial::ThemeManager::instance().theme());
    m_specDirty = false;
}

void QtMaterialAutocompletePopup::invalidatePopupLayout()
{
    updateGeometry();
    update();
}

void QtMaterialAutocompletePopup::updatePopupGeometry()
{
    syncToAnchorGeometry();
    updatePopupPalette();
}

void QtMaterialAutocompletePopup::updatePopupPalette()
{
    ensureSpecResolved();
    QPalette pal = palette();
    pal.setColor(QPalette::Base, m_spec.containerColor);
    pal.setColor(QPalette::Text, m_spec.textColor);
    setPalette(pal);
    if (m_view) {
        m_view->setPalette(pal);
    }
}

void QtMaterialAutocompletePopup::syncToAnchorGeometry()
{
    if (!m_anchorLineEdit) {
        return;
    }

    const QRect anchorRect = QRect(m_anchorLineEdit->mapToGlobal(QPoint(0, m_anchorLineEdit->height())),
                                   QSize(m_anchorLineEdit->width(), 1));
    ensureSpecResolved();
    const int height = qMin(m_spec.maxPopupSize.height(), m_spec.itemMinSize.height() * m_spec.visibleItemCount);
    setGeometry(QRect(anchorRect.topLeft(), QSize(qMax(anchorRect.width(), m_spec.minPopupSize.width()), height)));
}

void QtMaterialAutocompletePopup::syncSelectionFromCurrentIndex()
{
    // Placeholder for integration branch.
}
