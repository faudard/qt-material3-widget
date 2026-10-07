#include "qtmaterial/widgets/data/qtmaterialtable.h"


#include "qtmaterial/core/private/qtmaterialthemecontextbinding_p.h"
#include <QAbstractItemModel>
#include <QAccessible>
#include <QDataStream>
#include <QFocusEvent>
#include <QHeaderView>
#include <QIODevice>
#include <QItemSelectionModel>
#include <QKeyEvent>
#include <QPainter>
#include <QPalette>
#include <QScrollBar>
#include <QStyle>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/specs/qtmaterialdataspecresolver.h"
#include "../resolution/qtmaterialdataspecresolution_p.h"

namespace QtMaterial {
namespace {

class MaterialTableDelegate final
    : public QStyledItemDelegate
{
public:
    explicit MaterialTableDelegate(
        QObject* parent = nullptr)
        : QStyledItemDelegate(parent)
    {
    }

    void setSpec(const TableSpec& spec)
    {
        m_spec = spec;
    }

    void paint(
        QPainter* painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override
    {
        if (!painter || !index.isValid()) {
            return;
        }

        QStyleOptionViewItem resolved(option);
        initStyleOption(
            &resolved,
            index);

        const bool enabled =
            resolved.state.testFlag(
                QStyle::State_Enabled);
        const bool selected =
            resolved.state.testFlag(
                QStyle::State_Selected);
        const bool hovered =
            resolved.state.testFlag(
                QStyle::State_MouseOver);
        const bool pressed =
            resolved.state.testFlag(
                QStyle::State_Sunken);

        resolved.font =
            m_spec.bodyFont;

        resolved.palette.setColor(
            QPalette::Text,
            enabled
            ? m_spec.foregroundColor
            : m_spec.disabledTextColor);
        resolved.palette.setColor(
            QPalette::HighlightedText,
            m_spec.rowSelectedTextColor);
        resolved.palette.setColor(
            QPalette::Highlight,
            m_spec.rowSelectedColor);

        painter->save();

        if (selected) {
            painter->fillRect(
                resolved.rect,
                m_spec.rowSelectedColor);
        } else if (pressed) {
            painter->fillRect(
                resolved.rect,
                m_spec.rowPressedColor);
        } else if (hovered) {
            painter->fillRect(
                resolved.rect,
                m_spec.rowHoverColor);
        } else {
            painter->fillRect(
                resolved.rect,
                m_spec.backgroundColor);
        }

        QStyledItemDelegate::paint(
            painter,
            resolved,
            index);

        painter->restore();
    }

private:
    TableSpec m_spec =
        defaultTableSpec();
};

QString pluralize(
    int value,
    QStringView singular,
    QStringView plural)
{
    return QString::number(value)
        + QLatin1Char(' ')
        + (
            value == 1
            ? singular.toString()
            : plural.toString());
}

QString tableHeaderText(
    const QtMaterialTable* table,
    int column)
{
    const QAbstractItemModel* model =
        table ? table->model() : nullptr;

    if (
        !model
        || column < 0
        || column
            >= model->columnCount(
                table->rootIndex())) {
        return QString();
    }

    return model
        ->headerData(
            column,
            Qt::Horizontal,
            Qt::DisplayRole)
        .toString();
}

QString tableCellText(
    const QModelIndex& index)
{
    if (!index.isValid()) {
        return QString();
    }

    const QVariant accessible =
        index.data(
            Qt::AccessibleTextRole);

    if (
        accessible.isValid()
        && !accessible
            .toString()
            .isEmpty()) {
        return accessible.toString();
    }

    return index
        .data(Qt::DisplayRole)
        .toString();
}

TableSpec normalizedExplicitSpec(
    TableSpec spec)
{
    spec.rowHeight =
        qMax(24, spec.rowHeight);
    spec.denseRowHeight =
        qMax(24, spec.denseRowHeight);
    spec.headerHeight =
        qMax(24, spec.headerHeight);
    spec.gridWidth =
        qMax(0, spec.gridWidth);
    spec.focusRingWidth =
        qMax(0, spec.focusRingWidth);
    spec.cellHorizontalPadding =
        qMax(0, spec.cellHorizontalPadding);
    spec.headerHorizontalPadding =
        qMax(0, spec.headerHorizontalPadding);
    spec.minimumColumnWidth =
        qMax(24, spec.minimumColumnWidth);
    spec.cornerRadius =
        spec.cornerRadius < 0.0
        ? 12.0
        : spec.cornerRadius;

    return spec;
}

} // namespace

class QtMaterialTablePrivate
{
public:
    TableSpec spec =
        defaultTableSpec();
    TableSpec explicitSpec =
        defaultTableSpec();

    bool specDirty = true;
    bool explicitSpecSet = false;
    bool dense = false;

    QString accessibilitySummary;


    QtMaterialThemeContextBinding* themeBinding = nullptr;
MaterialTableDelegate* delegate = nullptr;
};

QtMaterialTable::QtMaterialTable(
    QWidget* parent)
    : QTableView(parent)
    , d_ptr(
        std::make_unique<
            QtMaterialTablePrivate>())
{
    d_ptr->themeBinding =
        new QtMaterialThemeContextBinding(this, this);

    connect(
        d_ptr->themeBinding,
        &QtMaterialThemeContextBinding::effectiveThemeContextChanged,
        this,
        &QtMaterialTable::effectiveThemeContextChanged);
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

    setObjectName(
        QStringLiteral("QtMaterialTable"));
    setAccessibleName(
        QStringLiteral("Table"));
    setAlternatingRowColors(false);
    setMouseTracking(true);
    setSelectionBehavior(
        QAbstractItemView::SelectRows);
    setSelectionMode(
        QAbstractItemView::SingleSelection);
    setShowGrid(true);
    setFocusPolicy(Qt::StrongFocus);
    setSortingEnabled(true);
    setEditTriggers(
        QAbstractItemView::DoubleClicked
        | QAbstractItemView::EditKeyPressed);

    connect(
        viewport(),
        &QWidget::customContextMenuRequested,
        this,
        [this](const QPoint& position) {
            Q_EMIT contextMenuRequested(
                indexAt(position),
                viewport()->mapToGlobal(position));
        });

    d_ptr->delegate =
        new MaterialTableDelegate(this);
    setItemDelegate(d_ptr->delegate);

    horizontalHeader()
        ->setStretchLastSection(true);
    horizontalHeader()
        ->setHighlightSections(false);
    verticalHeader()
        ->setVisible(false);
    ensureSpecResolved();
    applyResolvedSpec();

    if (selectionModel()) {
        connect(
            selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &QtMaterialTable::
                syncAccessibility);
    }

    syncAccessibility();
}

QtMaterialTable::~QtMaterialTable() =
    default;

void QtMaterialTable::setThemeContext(
    ThemeContext* context)
{
    if (d_ptr->themeBinding->themeContext() == context) {
        return;
    }

    d_ptr->themeBinding->setThemeContext(context);
    Q_EMIT themeContextChanged(context);
}

ThemeContext*
QtMaterialTable::themeContext() const noexcept
{
    return d_ptr->themeBinding->themeContext();
}

ThemeContext*
QtMaterialTable::effectiveThemeContext() const noexcept
{
    return d_ptr->themeBinding->effectiveThemeContext();
}

TableSpec QtMaterialTable::spec() const
{
    return resolvedSpec();
}

const TableSpec&
QtMaterialTable::resolvedSpec() const
{
    ensureSpecResolved();
    return d_ptr->spec;
}

void QtMaterialTable::setSpec(
    const TableSpec& spec)
{
    d_ptr->explicitSpec =
        normalizedExplicitSpec(spec);
    d_ptr->explicitSpecSet = true;
    d_ptr->specDirty = true;

    ensureSpecResolved();
    applyResolvedSpec();
}

void QtMaterialTable::resetSpec()
{
    if (!d_ptr->explicitSpecSet) {
        return;
    }

    d_ptr->explicitSpecSet = false;
    d_ptr->specDirty = true;

    ensureSpecResolved();
    applyResolvedSpec();
}

bool QtMaterialTable::
hasExplicitSpec() const noexcept
{
    return d_ptr->explicitSpecSet;
}

bool QtMaterialTable::dense() const
{
    return d_ptr->dense;
}

void QtMaterialTable::setDense(
    bool dense)
{
    if (d_ptr->dense == dense) {
        return;
    }

    d_ptr->dense = dense;
    d_ptr->specDirty = true;

    ensureSpecResolved();
    applyResolvedSpec();

    Q_EMIT denseChanged(
        d_ptr->dense);
}

bool QtMaterialTable::multiSelectionEnabled() const noexcept
{
    return selectionMode() == QAbstractItemView::ExtendedSelection
        || selectionMode() == QAbstractItemView::MultiSelection;
}

void QtMaterialTable::setMultiSelectionEnabled(bool enabled)
{
    const bool current = multiSelectionEnabled();
    if (current == enabled) {
        return;
    }
    setSelectionMode(
        enabled
            ? QAbstractItemView::ExtendedSelection
            : QAbstractItemView::SingleSelection);
    syncAccessibility();
    Q_EMIT multiSelectionEnabledChanged(enabled);
}

bool QtMaterialTable::columnReorderingEnabled() const noexcept
{
    return horizontalHeader()->sectionsMovable();
}

void QtMaterialTable::setColumnReorderingEnabled(bool enabled)
{
    if (columnReorderingEnabled() == enabled) {
        return;
    }

    horizontalHeader()->setSectionsMovable(enabled);
    Q_EMIT columnReorderingEnabledChanged(enabled);
}

bool QtMaterialTable::cellSelectionEnabled() const noexcept
{
    return selectionBehavior() == QAbstractItemView::SelectItems;
}

void QtMaterialTable::setCellSelectionEnabled(bool enabled)
{
    if (cellSelectionEnabled() == enabled) {
        return;
    }

    setSelectionBehavior(
        enabled
            ? QAbstractItemView::SelectItems
            : QAbstractItemView::SelectRows);
    Q_EMIT cellSelectionEnabledChanged(enabled);
}

bool QtMaterialTable::dragDropEnabled() const noexcept
{
    return dragDropMode() == QAbstractItemView::InternalMove;
}

void QtMaterialTable::setDragDropEnabled(bool enabled)
{
    if (dragDropEnabled() == enabled) {
        return;
    }

    setDragEnabled(enabled);
    viewport()->setAcceptDrops(enabled);
    setDropIndicatorShown(enabled);
    setDefaultDropAction(Qt::MoveAction);
    setDragDropMode(
        enabled
            ? QAbstractItemView::InternalMove
            : QAbstractItemView::NoDragDrop);

    Q_EMIT dragDropEnabledChanged(enabled);
}

bool QtMaterialTable::inlineEditingEnabled() const noexcept
{
    return editTriggers() != QAbstractItemView::NoEditTriggers;
}

void QtMaterialTable::setInlineEditingEnabled(bool enabled)
{
    if (inlineEditingEnabled() == enabled) {
        return;
    }

    setEditTriggers(
        enabled
            ? QAbstractItemView::EditTriggers(
                QAbstractItemView::DoubleClicked
                | QAbstractItemView::EditKeyPressed)
            : QAbstractItemView::NoEditTriggers);
    Q_EMIT inlineEditingEnabledChanged(enabled);
}

bool QtMaterialTable::contextMenuEnabled() const noexcept
{
    return viewport()->contextMenuPolicy() == Qt::CustomContextMenu;
}

void QtMaterialTable::setContextMenuEnabled(bool enabled)
{
    if (contextMenuEnabled() == enabled) {
        return;
    }

    viewport()->setContextMenuPolicy(
        enabled
            ? Qt::CustomContextMenu
            : Qt::DefaultContextMenu);
    Q_EMIT contextMenuEnabledChanged(enabled);
}

QString
QtMaterialTable::accessibilitySummary() const
{
    return d_ptr->accessibilitySummary;
}

QString
QtMaterialTable::
currentCellAccessibleText() const
{
    const QModelIndex index =
        currentIndex();

    if (!index.isValid()) {
        return QString();
    }

    const QString header =
        tableHeaderText(
            this,
            index.column());
    const QString value =
        tableCellText(index);
    const QString position =
        tr("row %1, column %2")
            .arg(index.row() + 1)
            .arg(index.column() + 1);

    if (header.isEmpty()) {
        return QStringLiteral("%1: %2")
            .arg(position, value);
    }

    return QStringLiteral("%1, %2: %3")
        .arg(position, header, value);
}

QString QtMaterialTable::rowAccessibleText(
    int row) const
{
    const QAbstractItemModel* currentModel =
        model();

    if (
        !currentModel
        || row < 0
        || row
            >= currentModel->rowCount(
                rootIndex())) {
        return QString();
    }

    QStringList cells;

    for (
        int column = 0;
        column
            < currentModel->columnCount(
                rootIndex());
        ++column) {
        const QModelIndex index =
            currentModel->index(
                row,
                column,
                rootIndex());

        if (!index.isValid()) {
            continue;
        }

        const QString header =
            tableHeaderText(
                this,
                column);
        const QString value =
            tableCellText(index);

        if (value.isEmpty()) {
            continue;
        }

        cells.push_back(
            header.isEmpty()
            ? value
            : QStringLiteral("%1: %2")
                .arg(header, value));
    }

    if (cells.isEmpty()) {
        return tr("Row %1")
            .arg(row + 1);
    }

    return tr("Row %1, %2")
        .arg(row + 1)
        .arg(
            cells.join(
                QStringLiteral(", ")));
}

QByteArray QtMaterialTable::saveWorkspaceState() const
{
    constexpr quint32 magic = 0x514d5457; // QMTW
    constexpr quint32 version = 1;

    QByteArray state;
    QDataStream stream(&state, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_12);

    const QHeaderView* header = horizontalHeader();
    stream
        << magic
        << version
        << qint32(header ? header->count() : 0)
        << (header ? header->saveState() : QByteArray())
        << dense()
        << multiSelectionEnabled()
        << columnReorderingEnabled()
        << cellSelectionEnabled()
        << dragDropEnabled()
        << inlineEditingEnabled()
        << contextMenuEnabled()
        << isSortingEnabled()
        << qint32(header ? header->sortIndicatorSection() : -1)
        << qint32(
            header
                ? int(header->sortIndicatorOrder())
                : int(Qt::AscendingOrder));

    return state;
}

bool QtMaterialTable::restoreWorkspaceState(
    const QByteArray& state)
{
    constexpr quint32 magic = 0x514d5457; // QMTW
    constexpr quint32 version = 1;

    QDataStream stream(state);
    stream.setVersion(QDataStream::Qt_5_12);

    quint32 storedMagic = 0;
    quint32 storedVersion = 0;
    qint32 sectionCount = 0;
    QByteArray headerState;
    bool storedDense = false;
    bool storedMultiSelection = false;
    bool storedColumnReordering = false;
    bool storedCellSelection = false;
    bool storedDragDrop = false;
    bool storedInlineEditing = false;
    bool storedContextMenu = false;
    bool storedSorting = false;
    qint32 sortSection = -1;
    qint32 sortOrder = int(Qt::AscendingOrder);

    stream
        >> storedMagic
        >> storedVersion
        >> sectionCount
        >> headerState
        >> storedDense
        >> storedMultiSelection
        >> storedColumnReordering
        >> storedCellSelection
        >> storedDragDrop
        >> storedInlineEditing
        >> storedContextMenu
        >> storedSorting
        >> sortSection
        >> sortOrder;

    if (
        stream.status() != QDataStream::Ok
        || !stream.atEnd()
        || storedMagic != magic
        || storedVersion != version
        || sectionCount < 0
        || sortOrder < int(Qt::AscendingOrder)
        || sortOrder > int(Qt::DescendingOrder)) {
        return false;
    }

    QHeaderView* header = horizontalHeader();
    if (!header || header->count() != sectionCount) {
        return false;
    }
    if (
        sortSection < -1
        || sortSection >= sectionCount) {
        return false;
    }

    const QByteArray previousHeaderState =
        header->saveState();

    // Validate Qt's native header payload before changing any Material policy.
    // Some policy changes (notably density/spec application) intentionally
    // update header metrics, so the saved header presentation must be restored
    // last to preserve exact user widths/order/visibility.
    if (!header->restoreState(headerState)) {
        header->restoreState(previousHeaderState);
        return false;
    }
    if (!header->restoreState(previousHeaderState)) {
        return false;
    }

    setDense(storedDense);
    setMultiSelectionEnabled(storedMultiSelection);
    setColumnReorderingEnabled(storedColumnReordering);
    setCellSelectionEnabled(storedCellSelection);
    setDragDropEnabled(storedDragDrop);
    setInlineEditingEnabled(storedInlineEditing);
    setContextMenuEnabled(storedContextMenu);
    setSortingEnabled(storedSorting);
    if (storedSorting && sortSection >= 0) {
        sortByColumn(
            sortSection,
            static_cast<Qt::SortOrder>(sortOrder));
    }

    // Header state is authoritative for presentation. Restore it after the
    // Material policies so setDense()/spec resolution cannot overwrite saved
    // section sizes.
    if (!header->restoreState(headerState)) {
        // The exact payload was validated above. This is only a defensive
        // rollback for an unexpected native restore failure.
        header->restoreState(previousHeaderState);
        return false;
    }

    syncAccessibility();
    return true;
}

void QtMaterialTable::
activateCurrentRow()
{
    const QModelIndex index =
        currentIndex();

    if (!index.isValid()) {
        return;
    }

    selectRow(index.row());

    Q_EMIT activated(index);
    Q_EMIT rowActivated(
        index.row());
}

void QtMaterialTable::paintEvent(
    QPaintEvent* event)
{
    ensureSpecResolved();
    QTableView::paintEvent(event);

    if (
        !hasFocus()
        || d_ptr->spec.focusRingWidth <= 0) {
        return;
    }

    QPainter painter(viewport());
    painter.setRenderHint(
        QPainter::Antialiasing);

    QtMaterialFocusIndicator::
        paintRectFocusRing(
            &painter,
            viewport()->rect(),
            d_ptr->spec.focusRingColor,
            d_ptr->spec.cornerRadius,
            d_ptr->spec.focusRingWidth);
}

void QtMaterialTable::focusInEvent(
    QFocusEvent* event)
{
    QTableView::focusInEvent(event);
    syncAccessibility();
    viewport()->update();
}

void QtMaterialTable::focusOutEvent(
    QFocusEvent* event)
{
    QTableView::focusOutEvent(event);
    viewport()->update();
}

void QtMaterialTable::keyPressEvent(
    QKeyEvent* event)
{
    if (!event) {
        return;
    }

    const Qt::KeyboardModifiers commandModifiers =
        event->modifiers()
        & (Qt::ShiftModifier
           | Qt::ControlModifier
           | Qt::AltModifier
           | Qt::MetaModifier);
    const bool plainActivation =
        commandModifiers == Qt::NoModifier;

    switch (event->key()) {
    case Qt::Key_F2:
        if (
            plainActivation
            && inlineEditingEnabled()
            && currentIndex().isValid()) {
            edit(currentIndex());
            event->accept();
            return;
        }
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space:
        if (plainActivation) {
            activateCurrentRow();
            event->accept();
            return;
        }
        break;
    default:
        break;
    }

    // Modifier-based selection (Shift/Ctrl/Meta + arrows/Space), Home/End
    // and page navigation stay under QTableView ownership. F2 is handled
    // explicitly above because Qt's platform EditKeyPressed mapping differs
    // on macOS.
    QTableView::keyPressEvent(event);
}

void QtMaterialTable::setModel(
    QAbstractItemModel* model)
{
    if (this->model()) {
        disconnect(
            this->model(),
            nullptr,
            this,
            nullptr);
    }

    QTableView::setModel(model);

    if (selectionModel()) {
        connect(
            selectionModel(),
            &QItemSelectionModel::
                selectionChanged,
            this,
            &QtMaterialTable::
                syncAccessibility,
            Qt::UniqueConnection);
    }

    if (model) {
        connect(
            model,
            &QAbstractItemModel::modelReset,
            this,
            &QtMaterialTable::
                syncAccessibility);
        connect(
            model,
            &QAbstractItemModel::rowsInserted,
            this,
            &QtMaterialTable::
                syncAccessibility);
        connect(
            model,
            &QAbstractItemModel::rowsRemoved,
            this,
            &QtMaterialTable::
                syncAccessibility);
        connect(
            model,
            &QAbstractItemModel::columnsInserted,
            this,
            &QtMaterialTable::
                syncAccessibility);
        connect(
            model,
            &QAbstractItemModel::columnsRemoved,
            this,
            &QtMaterialTable::
                syncAccessibility);
        connect(
            model,
            &QAbstractItemModel::dataChanged,
            this,
            &QtMaterialTable::
                syncAccessibility);
        connect(
            model,
            &QAbstractItemModel::
                headerDataChanged,
            this,
            &QtMaterialTable::
                syncAccessibility);
    }

    syncAccessibility();
}

void QtMaterialTable::currentChanged(
    const QModelIndex& current,
    const QModelIndex& previous)
{
    QTableView::currentChanged(
        current,
        previous);
    syncAccessibility();
}

void QtMaterialTable::
ensureSpecResolved() const
{
    if (!d_ptr->specDirty) {
        return;
    }

    if (d_ptr->explicitSpecSet) {
        d_ptr->spec =
            d_ptr->explicitSpec;
        d_ptr->specDirty = false;
        return;
    }
    d_ptr->spec =
        DataSpecResolution::tableSpec(
            d_ptr->themeBinding,
            d_ptr->dense
            ? Density::Compact
            : Density::Default);

    d_ptr->specDirty = false;
}

void QtMaterialTable::applyResolvedSpec()
{
    ensureSpecResolved();

    const TableSpec& spec =
        d_ptr->spec;

    const int rowHeight =
        d_ptr->dense
        ? spec.denseRowHeight
        : spec.rowHeight;

    verticalHeader()
        ->setDefaultSectionSize(
            qMax(24, rowHeight));

    horizontalHeader()
        ->setDefaultSectionSize(
            spec.headerHeight);
    horizontalHeader()
        ->setMinimumHeight(
            spec.headerHeight);
    horizontalHeader()
        ->setMinimumSectionSize(
            spec.minimumColumnWidth);
    horizontalHeader()
        ->setFont(
            spec.headerFont);

    setGridStyle(Qt::SolidLine);
    setShowGrid(spec.gridWidth > 0);
    setFont(spec.bodyFont);

    QPalette resolvedPalette =
        palette();

    resolvedPalette.setColor(
        QPalette::Window,
        spec.backgroundColor);
    resolvedPalette.setColor(
        QPalette::Base,
        spec.backgroundColor);
    resolvedPalette.setColor(
        QPalette::Text,
        spec.foregroundColor);
    resolvedPalette.setColor(
        QPalette::Disabled,
        QPalette::Text,
        spec.disabledTextColor);
    resolvedPalette.setColor(
        QPalette::Highlight,
        spec.rowSelectedColor);
    resolvedPalette.setColor(
        QPalette::HighlightedText,
        spec.rowSelectedTextColor);
    resolvedPalette.setColor(
        QPalette::Button,
        spec.headerBackgroundColor);
    resolvedPalette.setColor(
        QPalette::ButtonText,
        spec.headerForegroundColor);

    setPalette(resolvedPalette);
    viewport()->setPalette(
        resolvedPalette);
    horizontalHeader()->setPalette(
        resolvedPalette);

    if (d_ptr->delegate) {
        d_ptr->delegate->setSpec(spec);
    }

    const QString style =
        QStringLiteral(
            "QTableView {"
            " background: %1;"
            " color: %2;"
            " gridline-color: %3;"
            " border: %4px solid %3;"
            " border-radius: %5px;"
            "}"
            "QHeaderView::section {"
            " background: %6;"
            " color: %7;"
            " padding: 0 %8px;"
            " border: 0;"
            " border-bottom: %4px solid %3;"
            "}"
            "QTableView::item {"
            " padding: 0 %9px;"
            " border: 0;"
            "}"
            "QScrollBar {"
            " background: transparent;"
            "}")
        .arg(
            spec.backgroundColor.name(),
            spec.foregroundColor.name(),
            spec.gridColor.name())
        .arg(spec.gridWidth)
        .arg(spec.cornerRadius)
        .arg(
            spec.headerBackgroundColor.name(),
            spec.headerForegroundColor.name())
        .arg(spec.headerHorizontalPadding)
        .arg(spec.cellHorizontalPadding);

    setStyleSheet(style);

    viewport()->update();
    updateGeometry();
    update();
}

void QtMaterialTable::
syncAccessibility()
{
    const QAbstractItemModel* currentModel =
        model();

    const int rows =
        currentModel
        ? currentModel->rowCount(
            rootIndex())
        : 0;
    const int columns =
        currentModel
        ? currentModel->columnCount(
            rootIndex())
        : 0;

    QStringList parts;

    parts << pluralize(
        rows,
        QStringLiteral("row"),
        QStringLiteral("rows"));
    parts << pluralize(
        columns,
        QStringLiteral("column"),
        QStringLiteral("columns"));

    if (currentIndex().isValid()) {
        parts << currentCellAccessibleText();
    }

    const QString summary =
        parts.join(
            QStringLiteral(", "));

    setAccessibleDescription(summary);

    if (
        summary
        == d_ptr->accessibilitySummary) {
        return;
    }

    d_ptr->accessibilitySummary =
        summary;

    Q_EMIT accessibilitySummaryChanged(
        summary);

    QAccessibleEvent event(
        this,
        QAccessible::DescriptionChanged);

    QAccessible::updateAccessibility(
        &event);
}

} // namespace QtMaterial
