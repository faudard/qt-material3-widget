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
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include <memory>
#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/effects/qtmaterialelevationrenderer.h"
#include "qtmaterial/specs/qtmaterialautocompletepopupspecresolver.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include <QMetaObject>

struct QtMaterialAutocompletePopupPrivate {

    mutable bool m_specDirty = true;
    mutable QtMaterial::AutocompletePopupSpec m_spec;
    QPointer<QLineEdit> m_anchorLineEdit;
    QPointer<QAbstractItemModel> m_sourceModel;
    QSortFilterProxyModel* m_filterModel = nullptr;
    QStringListModel* m_ownedStringModel = nullptr;
    QListView* m_view = nullptr;
    QString m_filterText;
    bool m_popupVisible = false;

    QPointer<QtMaterial::ThemeContext> m_themeContext;
    QPointer<QtMaterial::ThemeContext> m_effectiveThemeContext;
    QMetaObject::Connection m_themeChangedConnection;
    QMetaObject::Connection m_themeDestroyedConnection;
    QMetaObject::Connection m_ancestorContextConnection;
    bool m_effectivePopupVisible = false;
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

    connect(
        d_ptr->m_filterModel,
        &QAbstractItemModel::rowsInserted,
        this,
        [this]() {
            invalidatePopupLayout();
            refreshPopupVisibility();
        });
    connect(
        d_ptr->m_filterModel,
        &QAbstractItemModel::rowsRemoved,
        this,
        [this]() {
            invalidatePopupLayout();
            refreshPopupVisibility();
        });
    connect(
        d_ptr->m_filterModel,
        &QAbstractItemModel::modelReset,
        this,
        [this]() {
            invalidatePopupLayout();
            refreshPopupVisibility();
        });
    connect(
        d_ptr->m_filterModel,
        &QAbstractItemModel::layoutChanged,
        this,
        [this]() {
            invalidatePopupLayout();
            refreshPopupVisibility();
        });

    refreshThemeContextConnection();
    ensureSpecResolved();
    updatePopupPalette();
}

QtMaterialAutocompletePopup::~QtMaterialAutocompletePopup() = default;


void QtMaterialAutocompletePopup::setThemeContext(
    ThemeContext* context)
{
    if (d_ptr->m_themeContext.data() == context) {
        return;
    }

    d_ptr->m_themeContext = context;
    const bool changed =
        refreshThemeContextConnection();

    emit themeContextChanged(context);
    if (changed) {
        emit effectiveThemeContextChanged(
            effectiveThemeContext());
    }

    d_ptr->m_specDirty = true;
    ensureSpecResolved();
    updatePopupPalette();
    updateGeometry();
    update();
}

ThemeContext*
QtMaterialAutocompletePopup::themeContext() const noexcept
{
    return d_ptr->m_themeContext.data();
}

ThemeContext*
QtMaterialAutocompletePopup::effectiveThemeContext() const noexcept
{
    if (d_ptr->m_themeContext) {
        return d_ptr->m_themeContext.data();
    }

    QWidget* ancestor =
        d_ptr->m_anchorLineEdit
        ? d_ptr->m_anchorLineEdit->parentWidget()
        : parentWidget();

    while (ancestor) {
        if (auto* materialParent =
                qobject_cast<QtMaterialWidget*>(ancestor)) {
            return materialParent->effectiveThemeContext();
        }
        ancestor = ancestor->parentWidget();
    }

    return ThemeManager::instance().defaultContext();
}

const AutocompletePopupSpec&
QtMaterialAutocompletePopup::resolvedSpec() const
{
    ensureSpecResolved();
    return d_ptr->m_spec;
}

bool QtMaterialAutocompletePopup::refreshThemeContextConnection()
{
    ThemeContext* nextContext =
        effectiveThemeContext();
    const bool changed =
        d_ptr->m_effectiveThemeContext.data() != nextContext;

    QObject::disconnect(d_ptr->m_themeChangedConnection);
    QObject::disconnect(d_ptr->m_themeDestroyedConnection);
    QObject::disconnect(d_ptr->m_ancestorContextConnection);

    d_ptr->m_effectiveThemeContext = nextContext;

    if (!d_ptr->m_themeContext) {
        QWidget* ancestor =
            d_ptr->m_anchorLineEdit
            ? d_ptr->m_anchorLineEdit->parentWidget()
            : parentWidget();

        while (ancestor) {
            if (auto* materialParent =
                    qobject_cast<QtMaterialWidget*>(
                        ancestor)) {
                d_ptr->m_ancestorContextConnection =
                    QObject::connect(
                        materialParent,
                        &QtMaterialWidget::
                            effectiveThemeContextChanged,
                        this,
                        &QtMaterialAutocompletePopup::
                            handleInheritedThemeContextChanged);
                break;
            }
            ancestor = ancestor->parentWidget();
        }
    }

    if (nextContext) {
        d_ptr->m_themeChangedConnection =
            QObject::connect(
                nextContext,
                &ThemeContext::themeChanged,
                this,
                &QtMaterialAutocompletePopup::
                    handleThemeChanged);

        const bool explicitContext =
            nextContext == d_ptr->m_themeContext.data();
        d_ptr->m_themeDestroyedConnection =
            QObject::connect(
                nextContext,
                &QObject::destroyed,
                this,
                [this, explicitContext]() {
                    handleThemeContextDestroyed(
                        explicitContext);
                });
    }

    return changed;
}

void QtMaterialAutocompletePopup::handleThemeChanged(
    const Theme&)
{
    d_ptr->m_specDirty = true;
    ensureSpecResolved();
    updatePopupPalette();
    updateGeometry();
    update();
}

void QtMaterialAutocompletePopup::
handleInheritedThemeContextChanged(
    ThemeContext*)
{
    if (refreshThemeContextConnection()) {
        emit effectiveThemeContextChanged(
            effectiveThemeContext());
    }

    handleThemeChanged(
        effectiveThemeContext()->theme());
}

void QtMaterialAutocompletePopup::
handleThemeContextDestroyed(
    bool explicitContext)
{
    if (explicitContext) {
        d_ptr->m_themeContext.clear();
        emit themeContextChanged(nullptr);
    }

    d_ptr->m_effectiveThemeContext.clear();
    refreshThemeContextConnection();
    emit effectiveThemeContextChanged(
        effectiveThemeContext());

    d_ptr->m_specDirty = true;
    ensureSpecResolved();
    updatePopupPalette();
    updateGeometry();
    update();
}

void QtMaterialAutocompletePopup::setAnchorLineEdit(
    QLineEdit* lineEdit)
{
    if (d_ptr->m_anchorLineEdit == lineEdit) {
        refreshPopupVisibility();
        return;
    }

    if (d_ptr->m_anchorLineEdit) {
        d_ptr->m_anchorLineEdit->removeEventFilter(this);
    }

    d_ptr->m_anchorLineEdit = lineEdit;
    if (d_ptr->m_anchorLineEdit) {
        d_ptr->m_anchorLineEdit->installEventFilter(this);
    }

    if (refreshThemeContextConnection()) {
        emit effectiveThemeContextChanged(
            effectiveThemeContext());
    }

    d_ptr->m_specDirty = true;
    ensureSpecResolved();
    updatePopupPalette();
    updatePopupGeometry();
    refreshPopupVisibility();
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

void QtMaterialAutocompletePopup::setPopupVisible(
    bool visible)
{
    d_ptr->m_popupVisible = visible;
    refreshPopupVisibility();
}

bool
QtMaterialAutocompletePopup::isPopupVisible() const noexcept
{
    return d_ptr->m_effectivePopupVisible;
}

void QtMaterialAutocompletePopup::setFilterText(
    const QString& text)
{
    if (d_ptr->m_filterText == text) {
        refreshPopupVisibility();
        return;
    }

    d_ptr->m_filterText = text;
    d_ptr->m_filterModel->setFilterFixedString(text);

    if (d_ptr->m_filterModel->rowCount() > 0) {
        d_ptr->m_view->setCurrentIndex(
            d_ptr->m_filterModel->index(0, 0));
    } else {
        clearSelection();
    }

    invalidatePopupLayout();
    refreshPopupVisibility();
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

bool QtMaterialAutocompletePopup::eventFilter(
    QObject* watched,
    QEvent* event)
{
    if (watched == d_ptr->m_anchorLineEdit && event) {
        switch (event->type()) {
        case QEvent::Move:
        case QEvent::Resize:
        case QEvent::Show:
            updatePopupGeometry();
            refreshPopupVisibility();
            break;

        case QEvent::ParentChange:
            if (refreshThemeContextConnection()) {
                emit effectiveThemeContextChanged(
                    effectiveThemeContext());
            }
            d_ptr->m_specDirty = true;
            ensureSpecResolved();
            updatePopupPalette();
            refreshPopupVisibility();
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

void QtMaterialAutocompletePopup::paintEvent(
    QPaintEvent*)
{
    ensureSpecResolved();

    const QRectF visualRect =
        QRectF(rect()).adjusted(1, 1, -1, -1);
    if (!visualRect.isValid()) {
        return;
    }

    const qreal radius =
        d_ptr->m_spec.cornerRadius < 0
        ? qMin(
            visualRect.width(),
            visualRect.height()) / 2.0
        : qMin<qreal>(
            d_ptr->m_spec.cornerRadius,
            visualRect.height() / 2.0);

    QPainterPath path;
    path.addRoundedRect(
        visualRect,
        radius,
        radius);

    QPainter painter(this);
    painter.setRenderHint(
        QPainter::Antialiasing,
        true);

    if (d_ptr->m_spec.hasResolvedElevationStyle) {
        QtMaterialElevationRenderer::
            paintPathElevation(
                &painter,
                path,
                d_ptr->m_spec.shadowColor,
                d_ptr->m_spec.elevationStyle);
    }

    painter.fillPath(
        path,
        d_ptr->m_spec.containerColor);

    if (hasFocus()) {
        QPen focusPen(
            d_ptr->m_spec.focusRingColor,
            d_ptr->m_spec.focusRingWidth);
        focusPen.setStyle(Qt::DashLine);
        painter.setPen(focusPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(path);
    }
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

void
QtMaterialAutocompletePopup::ensureSpecResolved() const
{
    if (!d_ptr->m_specDirty) {
        return;
    }

    ThemeContext* context =
        effectiveThemeContext();
    Q_ASSERT(context);

    const AutocompletePopupSpecResolver resolver;
    d_ptr->m_spec =
        resolver.autocompletePopupSpec(
            context->theme());
    d_ptr->m_specDirty = false;
}

void QtMaterialAutocompletePopup::invalidatePopupLayout()
{
    updateGeometry();
    update();
}

void
QtMaterialAutocompletePopup::updatePopupGeometry()
{
    syncToAnchorGeometry();
    updatePopupPalette();
}

void
QtMaterialAutocompletePopup::updatePopupPalette()
{
    ensureSpecResolved();

    QPalette paletteValue = palette();
    paletteValue.setColor(
        QPalette::Window,
        d_ptr->m_spec.containerColor);
    paletteValue.setColor(
        QPalette::Base,
        d_ptr->m_spec.containerColor);
    paletteValue.setColor(
        QPalette::Text,
        d_ptr->m_spec.textColor);
    paletteValue.setColor(
        QPalette::WindowText,
        d_ptr->m_spec.textColor);
    paletteValue.setColor(
        QPalette::Highlight,
        d_ptr->m_spec.selectedContainerColor);
    paletteValue.setColor(
        QPalette::HighlightedText,
        d_ptr->m_spec.selectedTextColor);
    paletteValue.setColor(
        QPalette::Mid,
        d_ptr->m_spec.dividerColor);

    setPalette(paletteValue);

    if (d_ptr->m_view) {
        d_ptr->m_view->setPalette(paletteValue);
        if (d_ptr->m_spec.hasResolvedItemFont) {
            d_ptr->m_view->setFont(d_ptr->m_spec.itemFont);
        }
    }
}

void
QtMaterialAutocompletePopup::syncToAnchorGeometry()
{
    if (!d_ptr->m_anchorLineEdit) {
        return;
    }

    ensureSpecResolved();

    const QPoint globalBottomLeft =
        d_ptr->m_anchorLineEdit->mapToGlobal(
            QPoint(0, d_ptr->m_anchorLineEdit->height()));

    const int visibleRows = qMin(
        d_ptr->m_spec.visibleItemCount,
        qMax(1, d_ptr->m_filterModel->rowCount()));
    const int contentHeight =
        d_ptr->m_spec.itemMinSize.height()
        * visibleRows;
    const int popupHeight = qBound(
        d_ptr->m_spec.minPopupSize.height(),
        contentHeight,
        d_ptr->m_spec.maxPopupSize.height());
    const int popupWidth = qBound(
        d_ptr->m_spec.minPopupSize.width(),
        qMax(
            d_ptr->m_anchorLineEdit->width(),
            d_ptr->m_spec.minPopupSize.width()),
        d_ptr->m_spec.maxPopupSize.width());

    setGeometry(
        QRect(
            globalBottomLeft,
            QSize(popupWidth, popupHeight)));
}

void QtMaterialAutocompletePopup::syncSelectionFromCurrentIndex()
{
    // Selection is synchronized directly through QListView currentIndex().
}

void QtMaterialAutocompletePopup::refreshPopupVisibility()
{
    const bool shouldShow = d_ptr->m_popupVisible;

    if (shouldShow) {
        updatePopupGeometry();
        show();
        raise();
    } else {
        hide();
    }

    setEffectivePopupVisible(
        shouldShow && isVisible());
}

void QtMaterialAutocompletePopup::
setEffectivePopupVisible(bool visible)
{
    if (d_ptr->m_effectivePopupVisible == visible) {
        return;
    }

    d_ptr->m_effectivePopupVisible = visible;
    emit popupVisibilityChanged(visible);
}

bool QtMaterialAutocompletePopup::event(QEvent* event)
{
    const bool handled = QWidget::event(event);

    if (event) {
        switch (event->type()) {
        case QEvent::Show:
            setEffectivePopupVisible(true);
            break;

        case QEvent::Hide:
            setEffectivePopupVisible(false);
            break;

        case QEvent::ParentChange:
            if (refreshThemeContextConnection()) {
                emit effectiveThemeContextChanged(
                    effectiveThemeContext());
            }
            d_ptr->m_specDirty = true;
            ensureSpecResolved();
            updatePopupPalette();
            break;

        default:
            break;
        }
    }

    return handled;
}

