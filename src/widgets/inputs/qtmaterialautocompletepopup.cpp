#include "qtmaterial/widgets/inputs/qtmaterialautocompletepopup.h"

#include <QAbstractItemModel>
#include <QEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QListView>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QSortFilterProxyModel>
#include <QStringListModel>
#include <QVBoxLayout>

#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialshadowrenderer.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"

QtMaterialAutocompletePopup::QtMaterialAutocompletePopup(QWidget* parent)
    : QWidget(parent)
    , m_ownedStringModel(new QStringListModel(this))
    , m_filterModel(new QSortFilterProxyModel(this))
{
    setAttribute(Qt::WA_Hover, true);
    setFocusPolicy(Qt::StrongFocus);
    setWindowFlag(Qt::Popup, true);
    setAutoFillBackground(false);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_filterModel->setSourceModel(m_ownedStringModel);
    m_filterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_filterModel->setFilterRole(Qt::DisplayRole);

    m_view = new QListView(this);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_view->setUniformItemSizes(false);
    m_view->setModel(m_filterModel);
    layout->addWidget(m_view);

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
    m_sourceModel = model;
    m_filterModel->setSourceModel(model);
    invalidatePopupLayout();
}

QAbstractItemModel* QtMaterialAutocompletePopup::model() const noexcept
{
    return m_sourceModel ? m_sourceModel.data() : m_ownedStringModel;
}

void QtMaterialAutocompletePopup::setSuggestions(const QStringList& suggestions)
{
    m_ownedStringModel->setStringList(suggestions);
    if (!m_sourceModel || m_sourceModel == m_ownedStringModel) {
        setModel(m_ownedStringModel);
    }
    invalidatePopupLayout();
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
    if (visible && m_filterModel->rowCount() > 0) {
        updatePopupGeometry();
        show();
        raise();
    } else {
        hide();
    }
    emit popupVisibilityChanged(m_popupVisible && isVisible());
}

bool QtMaterialAutocompletePopup::isPopupVisible() const noexcept
{
    return m_popupVisible && isVisible();
}

void QtMaterialAutocompletePopup::setFilterText(const QString& text)
{
    if (m_filterText == text) {
        return;
    }
    m_filterText = text;
    m_filterModel->setFilterFixedString(text);
    if (m_filterModel->rowCount() > 0) {
        m_view->setCurrentIndex(m_filterModel->index(0, 0));
    }
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
    const QModelIndex next = m_filterModel->index(qMin(row, m_filterModel->rowCount() - 1), 0);
    if (next.isValid()) {
        m_view->setCurrentIndex(next);
        m_view->scrollTo(next);
    }
}

void QtMaterialAutocompletePopup::selectPrevious()
{
    QModelIndex current = m_view->currentIndex();
    const int row = current.isValid() ? current.row() - 1 : 0;
    const QModelIndex prev = m_filterModel->index(qMax(0, row), 0);
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
    const int visibleRows = qMin(m_spec.visibleItemCount, qMax(1, m_filterModel->rowCount()));
    return m_spec.minPopupSize.expandedTo(QSize(width(), qMin(m_spec.maxPopupSize.height(), m_spec.itemMinSize.height() * visibleRows)));
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

void QtMaterialAutocompletePopup::paintEvent(QPaintEvent*)
{
    ensureSpecResolved();
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    const QRectF visualRect = rect().adjusted(1, 1, -1, -1);
    const qreal radius = m_spec.cornerRadius;
    QPainterPath path;
    path.addRoundedRect(visualRect, radius, radius);
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
    ensureSpecResolved();
    const QPoint globalBottomLeft = m_anchorLineEdit->mapToGlobal(QPoint(0, m_anchorLineEdit->height()));
    const int visibleRows = qMin(m_spec.visibleItemCount, qMax(1, m_filterModel->rowCount()));
    const int height = qMin(m_spec.maxPopupSize.height(), m_spec.itemMinSize.height() * visibleRows);
    setGeometry(QRect(globalBottomLeft, QSize(qMax(m_anchorLineEdit->width(), m_spec.minPopupSize.width()), height)));
}

void QtMaterialAutocompletePopup::syncSelectionFromCurrentIndex()
{
    // Selection is synchronized directly through QListView currentIndex().
}
