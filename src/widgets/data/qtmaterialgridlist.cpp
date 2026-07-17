#include "qtmaterial/widgets/data/qtmaterialgridlist.h"

#include <algorithm>

#include <QEvent>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QListWidgetItem>
#include <QMetaObject>
#include <QPainter>
#include <QPalette>
#include <QPainterPath>
#include <QPointer>
#include <QResizeEvent>
#include <QScrollBar>
#include <QStyle>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>

#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/specs/qtmaterialdataspecresolver.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"

namespace QtMaterial {
namespace {

constexpr int kTitleRole =
    Qt::UserRole + 120;
constexpr int kSupportingTextRole =
    kTitleRole + 1;

QString displayText(
    const QString& title,
    const QString& supportingText)
{
    if (supportingText.trimmed().isEmpty()) {
        return title;
    }

    return title
        + QLatin1Char('\n')
        + supportingText;
}

QAbstractItemView::SelectionMode
toQtSelectionMode(
    QtMaterialGridList::SelectionMode mode)
{
    switch (mode) {
    case QtMaterialGridList::SelectionMode::
        NoSelection:
        return QAbstractItemView::NoSelection;
    case QtMaterialGridList::SelectionMode::
        MultiSelection:
        return QAbstractItemView::MultiSelection;
    case QtMaterialGridList::SelectionMode::
        SingleSelection:
    default:
        return QAbstractItemView::SingleSelection;
    }
}

QListWidgetItem* gridItemAt(
    const QtMaterialGridList* self,
    int index)
{
    if (
        !self
        || index < 0
        || index >= self->count()) {
        return nullptr;
    }

    return self->item(index);
}

int nextEnabledIndex(
    const QtMaterialGridList* self,
    int from,
    int delta)
{
    if (
        !self
        || self->count() == 0
        || delta == 0) {
        return -1;
    }

    int candidate = from;
    if (candidate < 0) {
        candidate =
            delta > 0
            ? -1
            : self->count();
    }

    for (;;) {
        candidate += delta;

        if (
            candidate < 0
            || candidate >= self->count()) {
            return -1;
        }

        if (self->isItemEnabled(candidate)) {
            return candidate;
        }
    }
}

int firstEnabledIndex(
    const QtMaterialGridList* self)
{
    if (!self) {
        return -1;
    }

    for (
        int index = 0;
        index < self->count();
        ++index) {
        if (self->isItemEnabled(index)) {
            return index;
        }
    }

    return -1;
}

int lastEnabledIndex(
    const QtMaterialGridList* self)
{
    if (!self) {
        return -1;
    }

    for (
        int index = self->count() - 1;
        index >= 0;
        --index) {
        if (self->isItemEnabled(index)) {
            return index;
        }
    }

    return -1;
}

void updateItemText(
    QListWidgetItem* item)
{
    if (!item) {
        return;
    }

    const QString title =
        item->data(kTitleRole).toString();
    const QString supportingText =
        item->data(
            kSupportingTextRole).toString();

    const QString display =
        displayText(
            title,
            supportingText);

    item->setText(display);
    item->setToolTip(display);
}

class GridListDelegate final
    : public QStyledItemDelegate
{
public:
    explicit GridListDelegate(
        QtMaterialGridList* owner)
        : QStyledItemDelegate(owner)
        , m_owner(owner)
    {
    }

    QSize sizeHint(
        const QStyleOptionViewItem&,
        const QModelIndex&) const override
    {
        return m_owner
            ? m_owner->cellExtent()
            : QSize(160, 144);
    }

    void paint(
        QPainter* painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override
    {
        if (
            !painter
            || !m_owner
            || !index.isValid()) {
            return;
        }

        const GridListSpec& spec =
            m_owner->resolvedSpec();

        const bool enabled =
            option.state
            .testFlag(QStyle::State_Enabled);
        const bool selected =
            option.state
            .testFlag(QStyle::State_Selected);
        const bool hovered =
            option.state
            .testFlag(QStyle::State_MouseOver);
        const bool focused =
            option.state
            .testFlag(QStyle::State_HasFocus);

        QRectF itemRect =
            QRectF(option.rect).adjusted(
                2.0,
                2.0,
                -2.0,
                -2.0);

        const qreal radius =
            qBound<qreal>(
                0.0,
                spec.itemRadius,
                qMin(
                    itemRect.width(),
                    itemRect.height())
                    / 2.0);

        QColor background =
            spec.itemBackgroundColor;
        QColor foreground =
            enabled
            ? spec.foregroundColor
            : spec.disabledTextColor;

        if (selected) {
            background =
                spec.itemSelectedColor;
            foreground =
                enabled
                ? spec.itemSelectedTextColor
                : spec.disabledTextColor;
        } else if (hovered && enabled) {
            background =
                spec.itemHoverColor;
        }

        QPainterPath path;
        path.addRoundedRect(
            itemRect,
            radius,
            radius);

        painter->save();
        painter->setRenderHint(
            QPainter::Antialiasing,
            true);
        painter->fillPath(
            path,
            background);

        if (
            focused
            && spec.focusRingWidth > 0) {
            painter->setPen(
                QPen(
                    spec.focusRingColor,
                    spec.focusRingWidth));
            painter->setBrush(Qt::NoBrush);
            painter->drawPath(path);
        }

        const QIcon icon =
            qvariant_cast<QIcon>(
                index.data(
                    Qt::DecorationRole));

        const int horizontalPadding = 12;
        const int verticalPadding = 10;
        const int iconExtent =
            icon.isNull()
            ? 0
            : qMin(
                48,
                qMax(
                    24,
                    static_cast<int>(
                        itemRect.height() * 0.34)));

        int contentTop =
            static_cast<int>(itemRect.top())
            + verticalPadding;

        if (!icon.isNull()) {
            const QRect iconRect(
                static_cast<int>(
                    itemRect.center().x())
                    - iconExtent / 2,
                contentTop,
                iconExtent,
                iconExtent);

            icon.paint(
                painter,
                iconRect,
                Qt::AlignCenter,
                enabled
                    ? QIcon::Normal
                    : QIcon::Disabled);

            contentTop =
                iconRect.bottom() + 8;
        }

        const QRect textRect(
            static_cast<int>(itemRect.left())
                + horizontalPadding,
            contentTop,
            qMax(
                0,
                static_cast<int>(itemRect.width())
                    - 2 * horizontalPadding),
            qMax(
                0,
                static_cast<int>(itemRect.bottom())
                    - contentTop
                    - verticalPadding));

        const QString title =
            index.data(kTitleRole).toString();
        const QString supporting =
            index.data(
                kSupportingTextRole).toString();

        QFont titleFont =
            spec.titleFont;
        if (titleFont.family().isEmpty()) {
            titleFont = option.font;
        }

        painter->setFont(titleFont);
        painter->setPen(foreground);

        QFontMetrics titleMetrics(titleFont);
        const QString elidedTitle =
            titleMetrics.elidedText(
                title,
                Qt::ElideRight,
                textRect.width());

        const int titleHeight =
            titleMetrics.height();

        QRect titleRect = textRect;
        if (supporting.trimmed().isEmpty()) {
            titleRect.setTop(
                textRect.center().y()
                - titleHeight / 2);
            titleRect.setHeight(titleHeight);
        } else {
            titleRect.setHeight(titleHeight);
        }

        painter->drawText(
            titleRect,
            Qt::AlignHCenter
                | Qt::AlignVCenter,
            elidedTitle);

        if (!supporting.trimmed().isEmpty()) {
            QFont supportingFont =
                spec.supportingFont;
            if (
                supportingFont.family().isEmpty()) {
                supportingFont = option.font;
            }

            painter->setFont(
                supportingFont);
            painter->setPen(
                enabled
                ? spec.supportingTextColor
                : spec.disabledTextColor);

            QFontMetrics supportingMetrics(
                supportingFont);
            const QString elidedSupporting =
                supportingMetrics.elidedText(
                    supporting,
                    Qt::ElideRight,
                    textRect.width());

            QRect supportingRect = textRect;
            supportingRect.setTop(
                titleRect.bottom() + 2);
            supportingRect.setHeight(
                supportingMetrics.height());

            painter->drawText(
                supportingRect,
                Qt::AlignHCenter
                    | Qt::AlignVCenter,
                elidedSupporting);
        }

        painter->restore();
    }

private:
    QtMaterialGridList* m_owner = nullptr;
};

} // namespace

struct QtMaterialGridListPrivate
{
    QtMaterialGridList::SelectionMode
        selectionMode =
            QtMaterialGridList::SelectionMode::
                SingleSelection;

    int columns = 3;
    QSize cellExtent = QSize(160, 144);

    bool columnsExplicit = false;
    bool cellExtentExplicit = false;

    QString accessibilitySummary;

    mutable GridListSpec spec;
    mutable bool specDirty = true;

    QPointer<ThemeContext> themeContext;
    QPointer<ThemeContext> effectiveThemeContext;

    QMetaObject::Connection
        themeChangedConnection;
    QMetaObject::Connection
        themeDestroyedConnection;
    QMetaObject::Connection
        ancestorContextConnection;

    GridListDelegate* delegate = nullptr;
};

QtMaterialGridList::QtMaterialGridList(
    QWidget* parent)
    : QListWidget(parent)
    , d_ptr(
        std::make_unique<
            QtMaterialGridListPrivate>())
{
    setObjectName(
        QStringLiteral("QtMaterialGridList"));
    setAccessibleName(
        QStringLiteral("Grid list"));
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setViewMode(QListView::IconMode);
    setResizeMode(QListView::Adjust);
    setMovement(QListView::Static);
    setWrapping(true);
    setFlow(QListView::LeftToRight);
    setUniformItemSizes(true);

    d_ptr->delegate =
        new GridListDelegate(this);
    setItemDelegate(d_ptr->delegate);

    QListWidget::setSelectionMode(
        toQtSelectionMode(
            d_ptr->selectionMode));

    refreshThemeContextConnection();
    ensureSpecResolved();
    applyResolvedSpec();
    updateAccessibilitySummary();

    connect(
        this,
        &QListWidget::currentRowChanged,
        this,
        [this](int row) {
            updateAccessibilitySummary();
            Q_EMIT currentIndexChanged(row);
        });

    connect(
        this,
        &QListWidget::itemClicked,
        this,
        [this](QListWidgetItem* item) {
            const int row =
                this->row(item);

            if (
                row >= 0
                && isItemEnabled(row)) {
                Q_EMIT gridItemClicked(row);
            }
        });

    connect(
        this,
        &QListWidget::itemActivated,
        this,
        [this](QListWidgetItem* item) {
            const int row =
                this->row(item);

            if (
                row >= 0
                && isItemEnabled(row)) {
                Q_EMIT gridItemActivated(row);
            }
        });

    connect(
        this,
        &QListWidget::itemSelectionChanged,
        this,
        [this]() {
            updateAccessibilitySummary();
            Q_EMIT gridSelectionChanged();
        });
}

QtMaterialGridList::~QtMaterialGridList() =
    default;

void QtMaterialGridList::setThemeContext(
    ThemeContext* context)
{
    if (
        d_ptr->themeContext.data()
        == context) {
        return;
    }

    d_ptr->themeContext = context;

    const bool changed =
        refreshThemeContextConnection();

    Q_EMIT themeContextChanged(context);

    if (changed) {
        Q_EMIT effectiveThemeContextChanged(
            effectiveThemeContext());
    }

    d_ptr->specDirty = true;
    ensureSpecResolved();
    applyResolvedSpec();
}

ThemeContext*
QtMaterialGridList::themeContext() const noexcept
{
    return d_ptr->themeContext.data();
}

ThemeContext*
QtMaterialGridList::
effectiveThemeContext() const noexcept
{
    if (d_ptr->themeContext) {
        return d_ptr->themeContext.data();
    }

    QWidget* ancestor = parentWidget();

    while (ancestor) {
        if (
            auto* materialParent =
                qobject_cast<
                    QtMaterialWidget*>(
                        ancestor)) {
            return materialParent
                ->effectiveThemeContext();
        }

        ancestor =
            ancestor->parentWidget();
    }

    return ThemeManager::instance()
        .defaultContext();
}

const GridListSpec&
QtMaterialGridList::resolvedSpec() const
{
    ensureSpecResolved();
    return d_ptr->spec;
}

int QtMaterialGridList::addGridItem(
    const QString& title,
    const QString& supportingText,
    const QIcon& icon)
{
    insertGridItem(
        count(),
        title,
        supportingText,
        icon);

    return count() - 1;
}

void QtMaterialGridList::insertGridItem(
    int index,
    const QString& title,
    const QString& supportingText,
    const QIcon& icon)
{
    ensureSpecResolved();

    index =
        qBound(
            0,
            index,
            count());

    auto* item =
        new QListWidgetItem(
            icon,
            QString());

    item->setData(kTitleRole, title);
    item->setData(
        kSupportingTextRole,
        supportingText);
    item->setTextAlignment(
        Qt::AlignCenter);
    item->setSizeHint(
        d_ptr->cellExtent);

    updateItemText(item);
    QListWidget::insertItem(
        index,
        item);

    item->setStatusTip(
        itemAccessibleText(index));
    item->setWhatsThis(
        itemAccessibleText(index));

    if (
        currentRow() < 0
        && isItemEnabled(index)) {
        setCurrentRow(index);
    }

    updateGridSize();
    updateAccessibilitySummary();
}

QString QtMaterialGridList::itemTitle(
    int index) const
{
    if (
        auto* item =
            gridItemAt(this, index)) {
        return item
            ->data(kTitleRole)
            .toString();
    }

    return QString();
}

void QtMaterialGridList::setItemTitle(
    int index,
    const QString& title)
{
    auto* item =
        gridItemAt(this, index);

    if (
        !item
        || item->data(
            kTitleRole).toString()
            == title) {
        return;
    }

    item->setData(kTitleRole, title);
    updateItemText(item);
    item->setStatusTip(
        itemAccessibleText(index));
    item->setWhatsThis(
        itemAccessibleText(index));
    updateAccessibilitySummary();
    viewport()->update();
}

QString
QtMaterialGridList::itemSupportingText(
    int index) const
{
    if (
        auto* item =
            gridItemAt(this, index)) {
        return item
            ->data(
                kSupportingTextRole)
            .toString();
    }

    return QString();
}

void QtMaterialGridList::
setItemSupportingText(
    int index,
    const QString& supportingText)
{
    auto* item =
        gridItemAt(this, index);

    if (
        !item
        || item->data(
            kSupportingTextRole)
            .toString()
            == supportingText) {
        return;
    }

    item->setData(
        kSupportingTextRole,
        supportingText);
    updateItemText(item);
    item->setStatusTip(
        itemAccessibleText(index));
    item->setWhatsThis(
        itemAccessibleText(index));
    updateAccessibilitySummary();
    viewport()->update();
}

QIcon QtMaterialGridList::itemIcon(
    int index) const
{
    if (
        auto* item =
            gridItemAt(this, index)) {
        return item->icon();
    }

    return QIcon();
}

void QtMaterialGridList::setItemIcon(
    int index,
    const QIcon& icon)
{
    if (
        auto* item =
            gridItemAt(this, index)) {
        item->setIcon(icon);
        viewport()->update(
            visualItemRect(item));
    }
}

bool QtMaterialGridList::isItemEnabled(
    int index) const
{
    if (
        auto* item =
            gridItemAt(this, index)) {
        return item->flags()
            .testFlag(Qt::ItemIsEnabled);
    }

    return false;
}

void QtMaterialGridList::setItemEnabled(
    int index,
    bool enabled)
{
    auto* item =
        gridItemAt(this, index);

    if (
        !item
        || isItemEnabled(index)
            == enabled) {
        return;
    }

    Qt::ItemFlags flags =
        item->flags();

    if (enabled) {
        flags |=
            Qt::ItemIsEnabled
            | Qt::ItemIsSelectable;
    } else {
        flags &= ~Qt::ItemIsEnabled;
        flags &= ~Qt::ItemIsSelectable;
        item->setSelected(false);

        if (currentRow() == index) {
            setCurrentRow(
                nextEnabledIndex(
                    this,
                    index,
                    1));
        }
    }

    item->setFlags(flags);
    item->setStatusTip(
        itemAccessibleText(index));
    item->setWhatsThis(
        itemAccessibleText(index));
    updateAccessibilitySummary();
    viewport()->update(
        visualItemRect(item));
}

int QtMaterialGridList::currentIndex() const
{
    return currentRow();
}

void QtMaterialGridList::setCurrentIndex(
    int index)
{
    if (index < 0) {
        setCurrentRow(-1);
        return;
    }

    if (!isItemEnabled(index)) {
        return;
    }

    setCurrentRow(index);
}

QtMaterialGridList::SelectionMode
QtMaterialGridList::
gridSelectionMode() const
{
    return d_ptr->selectionMode;
}

void QtMaterialGridList::
setGridSelectionMode(
    SelectionMode mode)
{
    if (
        d_ptr->selectionMode
        == mode) {
        return;
    }

    d_ptr->selectionMode = mode;

    QListWidget::setSelectionMode(
        toQtSelectionMode(mode));

    if (
        mode
        == SelectionMode::NoSelection) {
        clearSelection();
    } else if (
        mode
            == SelectionMode::
                SingleSelection
        && selectedItems().size() > 1) {
        const int keep = currentRow();

        clearSelection();

        if (keep >= 0) {
            setItemSelected(
                keep,
                true);
        }
    }

    updateAccessibilitySummary();
    Q_EMIT gridSelectionChanged();
}

QStringList
QtMaterialGridList::
selectedIndexesAsStrings() const
{
    QStringList result;

    const QList<int> indexes =
        selectedIndexesList();

    result.reserve(indexes.size());

    for (int index : indexes) {
        result.push_back(
            QString::number(index));
    }

    return result;
}

QList<int>
QtMaterialGridList::
selectedIndexesList() const
{
    QList<int> result;

    const auto items =
        selectedItems();

    result.reserve(items.size());

    for (QListWidgetItem* item : items) {
        result.push_back(row(item));
    }

    std::sort(
        result.begin(),
        result.end());

    return result;
}

void QtMaterialGridList::setItemSelected(
    int index,
    bool selected)
{
    if (
        d_ptr->selectionMode
            == SelectionMode::NoSelection
        || !isItemEnabled(index)) {
        return;
    }

    if (
        auto* item =
            gridItemAt(this, index)) {
        item->setSelected(selected);
        updateAccessibilitySummary();
    }
}

void QtMaterialGridList::
clearGridSelection()
{
    clearSelection();
    updateAccessibilitySummary();
}

int QtMaterialGridList::columns() const
{
    ensureSpecResolved();

    return d_ptr->columnsExplicit
        ? d_ptr->columns
        : d_ptr->spec.columns;
}

void QtMaterialGridList::setColumns(
    int columns)
{
    columns =
        qMax(1, columns);

    const int oldValue =
        this->columns();

    d_ptr->columnsExplicit = true;
    d_ptr->columns = columns;

    if (oldValue == columns) {
        return;
    }

    updateGridSize();
    updateAccessibilitySummary();

    Q_EMIT columnsChanged(columns);
}

QSize QtMaterialGridList::cellExtent() const
{
    ensureSpecResolved();

    return d_ptr->cellExtentExplicit
        ? d_ptr->cellExtent
        : d_ptr->spec.itemSize;
}

void QtMaterialGridList::setCellExtent(
    const QSize& size)
{
    ensureSpecResolved();

    const QSize normalized(
        qMax(
            d_ptr->spec.minimumCellWidth,
            size.width()),
        qMax(48, size.height()));

    const QSize oldValue =
        cellExtent();

    d_ptr->cellExtentExplicit = true;
    d_ptr->cellExtent = normalized;

    if (oldValue == normalized) {
        return;
    }

    updateAllItemMetrics();
    updateGridSize();

    Q_EMIT cellExtentChanged(normalized);
}

QString
QtMaterialGridList::itemAccessibleText(
    int index) const
{
    const auto* item =
        gridItemAt(this, index);

    if (!item) {
        return QString();
    }

    QStringList parts;
    parts << itemTitle(index);

    const QString supporting =
        itemSupportingText(index)
            .trimmed();

    if (!supporting.isEmpty()) {
        parts << supporting;
    }

    parts << tr("Item %1 of %2")
        .arg(index + 1)
        .arg(count());

    if (!isItemEnabled(index)) {
        parts << tr("Disabled");
    }

    if (item->isSelected()) {
        parts << tr("Selected");
    }

    return parts.join(
        QStringLiteral(", "));
}

QString
QtMaterialGridList::
currentItemAccessibleText() const
{
    return itemAccessibleText(
        currentRow());
}

QString
QtMaterialGridList::
accessibilitySummary() const
{
    return d_ptr->accessibilitySummary;
}

QSize QtMaterialGridList::sizeHint() const
{
    ensureSpecResolved();

    const QSize extent =
        cellExtent();
    const int columnCount =
        qMax(1, columns());
    const int rowCount =
        qMax(
            1,
            (count() + columnCount - 1)
                / columnCount);

    const int width =
        extent.width() * columnCount
        + d_ptr->spec.spacing
            * qMax(0, columnCount - 1)
        + frameWidth() * 2;

    const int height =
        extent.height() * qMin(rowCount, 3)
        + d_ptr->spec.spacing
            * qMax(0, qMin(rowCount, 3) - 1)
        + frameWidth() * 2;

    return QSize(width, height);
}

bool QtMaterialGridList::event(
    QEvent* event)
{
    const bool handled =
        QListWidget::event(event);

    if (
        event
        && event->type()
            == QEvent::ParentChange) {
        if (
            refreshThemeContextConnection()) {
            Q_EMIT
                effectiveThemeContextChanged(
                    effectiveThemeContext());
        }

        d_ptr->specDirty = true;
        ensureSpecResolved();
        applyResolvedSpec();
    }

    return handled;
}

void QtMaterialGridList::keyPressEvent(
    QKeyEvent* event)
{
    if (!event) {
        return;
    }

    const int current =
        currentRow();
    const bool rtl =
        layoutDirection()
        == Qt::RightToLeft;

    int target = -1;

    switch (event->key()) {
    case Qt::Key_Left:
        target =
            nextEnabledIndex(
                this,
                current,
                rtl ? 1 : -1);
        break;
    case Qt::Key_Right:
        target =
            nextEnabledIndex(
                this,
                current,
                rtl ? -1 : 1);
        break;
    case Qt::Key_Up:
        target =
            nextEnabledIndex(
                this,
                current,
                -columns());
        break;
    case Qt::Key_Down:
        target =
            nextEnabledIndex(
                this,
                current,
                columns());
        break;
    case Qt::Key_Home:
        target =
            firstEnabledIndex(this);
        break;
    case Qt::Key_End:
        target =
            lastEnabledIndex(this);
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space:
        if (
            current >= 0
            && isItemEnabled(current)) {
            Q_EMIT
                gridItemActivated(current);
            event->accept();
            return;
        }
        break;
    default:
        break;
    }

    if (target >= 0) {
        setCurrentRow(target);
        event->accept();
        return;
    }

    QListWidget::keyPressEvent(event);
}

void QtMaterialGridList::paintEvent(
    QPaintEvent* event)
{
    ensureSpecResolved();
    QListWidget::paintEvent(event);
}

void QtMaterialGridList::resizeEvent(
    QResizeEvent* event)
{
    QListWidget::resizeEvent(event);
    updateGridSize();
}

bool QtMaterialGridList::
refreshThemeContextConnection()
{
    ThemeContext* nextContext =
        effectiveThemeContext();

    const bool changed =
        d_ptr->effectiveThemeContext.data()
        != nextContext;

    QObject::disconnect(
        d_ptr->themeChangedConnection);
    QObject::disconnect(
        d_ptr->themeDestroyedConnection);
    QObject::disconnect(
        d_ptr->ancestorContextConnection);

    d_ptr->effectiveThemeContext =
        nextContext;

    if (!d_ptr->themeContext) {
        QWidget* ancestor =
            parentWidget();

        while (ancestor) {
            if (
                auto* materialParent =
                    qobject_cast<
                        QtMaterialWidget*>(
                            ancestor)) {
                d_ptr->ancestorContextConnection =
                    QObject::connect(
                        materialParent,
                        &QtMaterialWidget::
                            effectiveThemeContextChanged,
                        this,
                        &QtMaterialGridList::
                            handleInheritedThemeContextChanged);
                break;
            }

            ancestor =
                ancestor->parentWidget();
        }
    }

    if (nextContext) {
        d_ptr->themeChangedConnection =
            QObject::connect(
                nextContext,
                &ThemeContext::themeChanged,
                this,
                &QtMaterialGridList::
                    handleThemeChanged);

        const bool explicitContext =
            nextContext
            == d_ptr->themeContext.data();

        d_ptr->themeDestroyedConnection =
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

void QtMaterialGridList::handleThemeChanged(
    const Theme&)
{
    d_ptr->specDirty = true;
    ensureSpecResolved();
    applyResolvedSpec();
}

void QtMaterialGridList::
handleInheritedThemeContextChanged(
    ThemeContext*)
{
    if (
        refreshThemeContextConnection()) {
        Q_EMIT effectiveThemeContextChanged(
            effectiveThemeContext());
    }

    d_ptr->specDirty = true;
    ensureSpecResolved();
    applyResolvedSpec();
}

void QtMaterialGridList::
handleThemeContextDestroyed(
    bool explicitContext)
{
    if (explicitContext) {
        d_ptr->themeContext.clear();
        Q_EMIT themeContextChanged(nullptr);
    }

    d_ptr->effectiveThemeContext.clear();
    refreshThemeContextConnection();

    Q_EMIT effectiveThemeContextChanged(
        effectiveThemeContext());

    d_ptr->specDirty = true;
    ensureSpecResolved();
    applyResolvedSpec();
}

void QtMaterialGridList::
ensureSpecResolved() const
{
    if (!d_ptr->specDirty) {
        return;
    }

    ThemeContext* context =
        effectiveThemeContext();
    Q_ASSERT(context);

    const DataSpecResolver resolver;
    d_ptr->spec =
        resolver.gridListSpec(
            context->theme(),
            Density::Default);

    d_ptr->specDirty = false;
}

void QtMaterialGridList::applyResolvedSpec()
{
    ensureSpecResolved();

    if (!d_ptr->columnsExplicit) {
        d_ptr->columns =
            d_ptr->spec.columns;
    }
    if (!d_ptr->cellExtentExplicit) {
        d_ptr->cellExtent =
            d_ptr->spec.itemSize;
    }

    setSpacing(
        d_ptr->spec.spacing);

    QPalette resolvedPalette =
        palette();

    resolvedPalette.setColor(
        QPalette::Window,
        d_ptr->spec.backgroundColor);
    resolvedPalette.setColor(
        QPalette::Base,
        d_ptr->spec.backgroundColor);
    resolvedPalette.setColor(
        QPalette::Text,
        d_ptr->spec.foregroundColor);
    resolvedPalette.setColor(
        QPalette::WindowText,
        d_ptr->spec.foregroundColor);
    resolvedPalette.setColor(
        QPalette::Highlight,
        d_ptr->spec.itemSelectedColor);
    resolvedPalette.setColor(
        QPalette::HighlightedText,
        d_ptr->spec.itemSelectedTextColor);
    resolvedPalette.setColor(
        QPalette::Disabled,
        QPalette::Text,
        d_ptr->spec.disabledTextColor);

    setPalette(resolvedPalette);
    viewport()->setPalette(
        resolvedPalette);

    if (
        !d_ptr->spec.titleFont
            .family().isEmpty()) {
        setFont(
            d_ptr->spec.titleFont);
    }

    updateAllItemMetrics();
    updateGridSize();
    updateGeometry();
    viewport()->update();
}

void QtMaterialGridList::updateGridSize()
{
    ensureSpecResolved();

    QSize gridSize =
        cellExtent();

    const int columnCount =
        qMax(1, columns());

    if (viewport()) {
        const int scrollWidth =
            verticalScrollBar()
            ->sizeHint().width();

        const int available =
            qMax(
                0,
                viewport()->width()
                    - scrollWidth);

        if (available > 0) {
            const int totalSpacing =
                spacing()
                * qMax(
                    0,
                    columnCount - 1);

            const int width =
                qMax(
                    d_ptr->spec.minimumCellWidth,
                    (available - totalSpacing)
                        / columnCount);

            gridSize.setWidth(width);
        }
    }

    setGridSize(gridSize);

    for (
        int index = 0;
        index < count();
        ++index) {
        if (
            auto* item =
                gridItemAt(this, index)) {
            item->setSizeHint(gridSize);
        }
    }
}

void QtMaterialGridList::
updateAllItemMetrics()
{
    const QSize extent =
        cellExtent();

    for (
        int index = 0;
        index < count();
        ++index) {
        if (
            auto* item =
                gridItemAt(this, index)) {
            item->setSizeHint(extent);
            item->setStatusTip(
                itemAccessibleText(index));
            item->setWhatsThis(
                itemAccessibleText(index));
        }
    }
}

void QtMaterialGridList::
updateAccessibilitySummary()
{
    const int selectedCount =
        selectedItems().size();

    QStringList parts;
    parts << tr(
        "%n item(s)",
        nullptr,
        count());

    if (currentRow() >= 0) {
        parts << tr("current %1")
            .arg(currentRow() + 1);
    }

    if (selectedCount > 0) {
        parts << tr(
            "%n selected",
            nullptr,
            selectedCount);
    }

    const QString summary =
        parts.join(
            QStringLiteral(", "));

    if (
        d_ptr->accessibilitySummary
        == summary) {
        return;
    }

    d_ptr->accessibilitySummary =
        summary;

    setAccessibleDescription(summary);

    Q_EMIT accessibilitySummaryChanged(
        summary);
}

} // namespace QtMaterial
