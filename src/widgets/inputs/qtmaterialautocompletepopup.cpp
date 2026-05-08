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
#include <memory>

struct QtMaterialAutocompletePopupPrivate {

    mutable bool m_specDirty = true;
    mutable QtMaterial::AutocompletePopupSpec m_spec;
    QPointer<QLineEdit> m_anchorLineEdit;
    QPointer<QAbstractItemModel> d_ptr->m_sourceModel;
    QStringListModel* m_ownedStringModel = nullptr;
    QListView* m_view = nullptr;
    QString m_filterText;
    bool m_popupVisible = false;
};


QtMaterialAutocompletePopup::QtMaterialAutocompletePopup(QWidget* parent)
    : QWidget(parent)
    , d_ptr(std::make_unique<QtMaterialAutocompletePopupPrivate>())
{
    setAttribute(Qt::WA_Hover, true);
    setFocusPolicy(Qt::StrongFocus);
    setWindowFlag(Qt::Popup, true);
    setAutoFillBackground(false);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    d_ptr->m_ownedStringModel = new QStringListModel(this);


    d_ptr->m_filterModel = new QSortFilterProxyModel(this);


    d_ptr->m_filterModel->setSourceModel(d_ptr->m_ownedStringModel);
    d_ptr->m_filterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    d_ptr->m_filterModel->setFilterRole(Qt::DisplayRole);

    d_ptr->m_view = new QListView(this);
    d_ptr->m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    d_ptr->m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    d_ptr->m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_ptr->m_view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    d_ptr->m_view->setUniformItemSizes(false);
    d_ptr->m_view->setModel(d_ptr->m_filterModel);
    layout->addWidget(d_ptr->m_view);

    connect(d_ptr->m_view, &QListView::activated, this, [this](const QModelIndex& index) {
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
    if (d_ptr->m_anchorLineEdit == lineEdit) {
        return;
    }
    if (d_ptr->m_anchorLineEdit) {
        d_ptr->m_anchorLineEdit->removeEventFilter(this);
    }
    d_ptr->m_anchorLineEdit = lineEdit;
    if (d_ptr->m_anchorLineEdit) {
        d_ptr->m_anchorLineEdit->installEventFilter(this);
    }
    updatePopupGeometry();
}

QLineEdit* QtMaterialAutocompletePopup::anchorLineEdit() const noexcept
{
    return d_ptr->m_anchorLineEdit;
}

void QtMaterialAutocompletePopup::setModel(QAbstractItemModel* model)
{
    if (!model) {
        model = d_ptr->m_ownedStringModel;
    }
    d_ptr->m_sourceModel = model;
    d_ptr->m_filterModel->setSourceModel(model);
    invalidatePopupLayout();
}

QAbstractItemModel* QtMaterialAutocompletePopup::model() const noexcept
{
    return d_ptr->m_sourceModel ? d_ptr->m_sourceModel.data() : d_ptr->m_ownedStringModel;
}

void QtMaterialAutocompletePopup::setSuggestions(const QStringList& suggestions)
{
    d_ptr->m_ownedStringModel->setStringList(suggestions);
    if (!d_ptr->m_sourceModel || d_ptr->m_sourceModel == d_ptr->m_ownedStringModel) {
        setModel(d_ptr->m_ownedStringModel);
    }
    invalidatePopupLayout();
}

QString QtMaterialAutocompletePopup::currentCompletion() const
{
    const QModelIndex index = d_ptr->m_view->currentIndex();
    return index.isValid() ? index.data(Qt::DisplayRole).toString() : QString();
}

void QtMaterialAutocompletePopup::setPopupVisible(bool visible)
{
    if (d_ptr->m_popupVisible == visible) {
        return;
    }
    d_ptr->m_popupVisible = visible;
    if (visible && d_ptr->m_filterModel->rowCount() > 0) {
        updatePopupGeometry();
        show();
        raise();
    } else {
        hide();
    }
    emit popupVisibilityChanged(d_ptr->m_popupVisible && isVisible());
}

bool QtMaterialAutocompletePopup::isPopupVisible() const noexcept
{
    return d_ptr->m_popupVisible && isVisible();
}

void QtMaterialAutocompletePopup::setFilterText(const QString& text)
{
    if (d_ptr->m_filterText == text) {
        return;
    }
    d_ptr->m_filterText = text;
    d_ptr->m_filterModel->setFilterFixedString(text);
    if (d_ptr->m_filterModel->rowCount() > 0) {
        d_ptr->m_view->setCurrentIndex(d_ptr->m_filterModel->index(0, 0));
    }
    invalidatePopupLayout();
}

QString QtMaterialAutocompletePopup::filterText() const
{
    return d_ptr->m_filterText;
}

void QtMaterialAutocompletePopup::clearSelection()
{
    d_ptr->m_view->clearSelection();
    d_ptr->m_view->setCurrentIndex(QModelIndex());
}

void QtMaterialAutocompletePopup::selectNext()
{
    QModelIndex current = d_ptr->m_view->currentIndex();
    const int row = current.isValid() ? current.row() + 1 : 0;
    const QModelIndex next = d_ptr->m_filterModel->index(qMin(row, d_ptr->m_filterModel->rowCount() - 1), 0);
    if (next.isValid()) {
        d_ptr->m_view->setCurrentIndex(next);
        d_ptr->m_view->scrollTo(next);
    }
}

void QtMaterialAutocompletePopup::selectPrevious()
{
    QModelIndex current = d_ptr->m_view->currentIndex();
    const int row = current.isValid() ? current.row() - 1 : 0;
    const QModelIndex prev = d_ptr->m_filterModel->index(qMax(0, row), 0);
    if (prev.isValid()) {
        d_ptr->m_view->setCurrentIndex(prev);
        d_ptr->m_view->scrollTo(prev);
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
    const int visibleRows = qMin(d_ptr->m_spec.visibleItemCount, qMax(1, d_ptr->m_filterModel->rowCount()));
    return d_ptr->m_spec.minPopupSize.expandedTo(QSize(width(), qMin(d_ptr->m_spec.maxPopupSize.height(), d_ptr->m_spec.itemMinSize.height() * visibleRows)));
}

QSize QtMaterialAutocompletePopup::minimumSizeHint() const
{
    ensureSpecResolved();
    return d_ptr->m_spec.minPopupSize;
}

bool QtMaterialAutocompletePopup::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == d_ptr->m_anchorLineEdit) {
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
    const qreal radius = d_ptr->m_spec.cornerRadius;
    QPainterPath path;
    path.addRoundedRect(visualRect, radius, radius);
    p.fillPath(path, d_ptr->m_spec.containerColor);
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
    if (!d_ptr->m_specDirty) {
        return;
    }
    QtMaterial::SpecFactory factory;
    d_ptr->m_spec = factory.autocompletePopupSpec(QtMaterial::ThemeManager::instance().theme());
    d_ptr->m_specDirty = false;
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
    pal.setColor(QPalette::Base, d_ptr->m_spec.containerColor);
    pal.setColor(QPalette::Text, d_ptr->m_spec.textColor);
    setPalette(pal);
    if (d_ptr->m_view) {
        d_ptr->m_view->setPalette(pal);
    }
}

void QtMaterialAutocompletePopup::syncToAnchorGeometry()
{
    if (!d_ptr->m_anchorLineEdit) {
        return;
    }
    ensureSpecResolved();
    const QPoint globalBottomLeft = d_ptr->m_anchorLineEdit->mapToGlobal(QPoint(0, d_ptr->m_anchorLineEdit->height()));
    const int visibleRows = qMin(d_ptr->m_spec.visibleItemCount, qMax(1, d_ptr->m_filterModel->rowCount()));
    const int height = qMin(d_ptr->m_spec.maxPopupSize.height(), d_ptr->m_spec.itemMinSize.height() * visibleRows);
    setGeometry(QRect(globalBottomLeft, QSize(qMax(d_ptr->m_anchorLineEdit->width(), d_ptr->m_spec.minPopupSize.width()), height)));
}

void QtMaterialAutocompletePopup::syncSelectionFromCurrentIndex()
{
    // Selection is synchronized directly through QListView currentIndex().
}
