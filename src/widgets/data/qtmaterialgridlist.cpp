#include "qtmaterial/widgets/data/qtmaterialgridlist.h"

#include "qtmaterial/core/qtmaterialthemecontextbinding.h"

#include <algorithm>

#include <QEvent>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QListWidgetItem>
#include <QMetaObject>
#include <QPainter>
#include <QPalette>
#include <QPainterPath>
#include <QResizeEvent>
#include <QScrollBar>
#include <QStyle>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>

#include "qtmaterial/specs/qtmaterialdataspecresolver.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"

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

class GridListDelegate final : public QStyledItemDelegate
{
public:
    explicit GridListDelegate(
        QtMaterialGridList* grid)
        : QStyledItemDelegate(grid)
        , m_grid(grid)
    {
    }

    QSize sizeHint(
        const QStyleOptionViewItem&,
        const QModelIndex&) const override
    {
        return m_grid ? m_grid->cellExtent() : QSize(160, 144);
    }

    void paint(
        QPainter* painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override
    {
        if (!painter || !m_grid) {
            return;
        }

        const GridListSpec& spec = m_grid->resolvedSpec();
        const bool enabled =
            option.state.testFlag(QStyle::State_Enabled);
        const bool selected =
            option.state.testFlag(QStyle::State_Selected);
        const bool hovered =
            option.state.testFlag(QStyle::State_MouseOver);
        const bool focused =
            option.state.testFlag(QStyle::State_HasFocus);

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);

        const qreal outlineInset =
            qMax(spec.outlineWidth, spec.focusRingWidth) / 2.0;
        const QRectF cardRect =
            QRectF(option.rect).adjusted(
                outlineInset,
                outlineInset,
                -outlineInset,
                -outlineInset);

        QColor fill = selected
            ? spec.itemSelectedColor
            : spec.itemBackgroundColor;
        painter->setPen(
            spec.outlineWidth > 0
            ? QPen(spec.itemOutlineColor, spec.outlineWidth)
            : QPen(Qt::NoPen));
        painter->setBrush(fill);
        painter->drawRoundedRect(
            cardRect,
            spec.itemRadius,
            spec.itemRadius);

        if (hovered && enabled && !selected) {
            painter->setPen(Qt::NoPen);
            painter->setBrush(spec.itemHoverColor);
            painter->drawRoundedRect(
                cardRect,
                spec.itemRadius,
                spec.itemRadius);
        }

        QRect content =
            option.rect.marginsRemoved(spec.itemContentMargins);
        const QIcon icon = qvariant_cast<QIcon>(
            index.data(Qt::DecorationRole));
        if (!icon.isNull() && spec.iconSize.isValid()) {
            const QRect iconRect(
                content.center().x() - spec.iconSize.width() / 2,
                content.top(),
                spec.iconSize.width(),
                spec.iconSize.height());
            icon.paint(
                painter,
                iconRect,
                Qt::AlignCenter,
                enabled ? QIcon::Normal : QIcon::Disabled);
            content.setTop(
                iconRect.bottom() + 1 + spec.iconTextSpacing);
        }

        const QColor titleColor = !enabled
            ? spec.disabledTextColor
            : selected
                ? spec.itemSelectedTextColor
                : spec.foregroundColor;
        const QColor supportingColor = !enabled
            ? spec.disabledSupportingTextColor
            : selected
                ? spec.itemSelectedTextColor
                : spec.supportingTextColor;

        painter->setFont(spec.titleFont);
        painter->setPen(titleColor);
        const QFontMetrics titleMetrics(spec.titleFont);
        const QString title = index.data(kTitleRole).toString();
        const QString elidedTitle = titleMetrics.elidedText(
            title,
            Qt::ElideRight,
            content.width());
        const QRect titleRect(
            content.left(),
            content.top(),
            content.width(),
            titleMetrics.height());
        painter->drawText(
            titleRect,
            Qt::AlignHCenter | Qt::AlignTop,
            elidedTitle);

        const QString supporting =
            index.data(kSupportingTextRole).toString();
        if (!supporting.trimmed().isEmpty()) {
            painter->setFont(spec.supportingFont);
            painter->setPen(supportingColor);
            const QFontMetrics supportingMetrics(spec.supportingFont);
            const QRect supportingRect(
                content.left(),
                titleRect.bottom() + 1
                    + spec.titleSupportingSpacing,
                content.width(),
                supportingMetrics.height());
            painter->drawText(
                supportingRect,
                Qt::AlignHCenter | Qt::AlignTop,
                supportingMetrics.elidedText(
                    supporting,
                    Qt::ElideRight,
                    supportingRect.width()));
        }

        if (focused && spec.focusRingWidth > 0) {
            painter->setBrush(Qt::NoBrush);
            painter->setPen(
                QPen(spec.focusRingColor, spec.focusRingWidth));
            const QRectF focusRect = cardRect.adjusted(
                spec.focusRingWidth,
                spec.focusRingWidth,
                -spec.focusRingWidth,
                -spec.focusRingWidth);
            painter->drawRoundedRect(
                focusRect,
                qMax(0, spec.itemRadius - spec.focusRingWidth),
                qMax(0, spec.itemRadius - spec.focusRingWidth));
        }

        painter->restore();
    }

private:
    const QtMaterialGridList* m_grid = nullptr;
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
    GridListSpec explicitSpec;
    bool explicitSpecSet = false;
    QtMaterialThemeContextBinding* themeBinding = nullptr;

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

    d_ptr->themeBinding =
        new QtMaterialThemeContextBinding(this, this);
    connect(
        d_ptr->themeBinding,
        &QtMaterialThemeContextBinding::effectiveThemeContextChanged,
        this,
        &QtMaterialGridList::effectiveThemeContextChanged);
    connect(
        d_ptr->themeBinding,
        &QtMaterialThemeContextBinding::themeChanged,
        this,
        [this](const Theme&) {
            if (d_ptr->explicitSpecSet) {
                return;
            }
            d_ptr->specDirty = true;
            ensureSpecResolved();
            applyResolvedSpec();
        });

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
    if (themeContext() == context) {
        return;
    }

    d_ptr->themeBinding->setThemeContext(context);
    Q_EMIT themeContextChanged(context);
}

ThemeContext*
QtMaterialGridList::themeContext() const noexcept
{
    return d_ptr->themeBinding->themeContext();
}

ThemeContext*
QtMaterialGridList::effectiveThemeContext() const noexcept
{
    return d_ptr->themeBinding->effectiveThemeContext();
}

const GridListSpec&
QtMaterialGridList::resolvedSpec() const
{
    ensureSpecResolved();
    return d_ptr->spec;
}

GridListSpec QtMaterialGridList::spec() const
{
    return resolvedSpec();
}

void QtMaterialGridList::setSpec(
    const GridListSpec& spec)
{
    d_ptr->explicitSpec = spec;
    d_ptr->explicitSpecSet = true;
    d_ptr->specDirty = true;
    ensureSpecResolved();
    applyResolvedSpec();
}

bool QtMaterialGridList::hasExplicitSpec() const noexcept
{
    return d_ptr->explicitSpecSet;
}

void QtMaterialGridList::resetSpec()
{
    if (!d_ptr->explicitSpecSet) {
        return;
    }

    d_ptr->explicitSpecSet = false;
    d_ptr->specDirty = true;
    ensureSpecResolved();
    applyResolvedSpec();
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

bool QtMaterialGridList::hasCellExtentOverride() const noexcept
{
    return d_ptr->cellExtentExplicit;
}

void QtMaterialGridList::resetCellExtent()
{
    if (!d_ptr->cellExtentExplicit) {
        return;
    }

    const QSize previous = cellExtent();
    d_ptr->cellExtentExplicit = false;
    applyResolvedSpec();

    if (previous != cellExtent()) {
        Q_EMIT cellExtentChanged(cellExtent());
    }
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
    return QListWidget::event(event);
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

void QtMaterialGridList::ensureSpecResolved() const
{
    if (!d_ptr->specDirty) {
        return;
    }

    if (d_ptr->explicitSpecSet) {
        d_ptr->spec = d_ptr->explicitSpec;
        d_ptr->specDirty = false;
        return;
    }

    d_ptr->spec =
        DataSpecResolver().gridListSpec(
            d_ptr->themeBinding->theme(),
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
