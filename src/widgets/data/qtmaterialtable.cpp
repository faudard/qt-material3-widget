#include "qtmaterial/widgets/data/qtmaterialtable.h"


#include "qtmaterial/core/qtmaterialthemecontextbinding.h"
#include <QAbstractItemModel>
#include <QAccessible>
#include <QFocusEvent>
#include <QHeaderView>
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

    connect(
        selectionModel(),
        &QItemSelectionModel::selectionChanged,
        this,
        &QtMaterialTable::
            syncAccessibility);

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

    switch (event->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space:
        activateCurrentRow();
        event->accept();
        return;
    default:
        break;
    }

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

    const DataSpecResolver resolver;

    d_ptr->spec =
        resolver.tableSpec(
            d_ptr->themeBinding->theme(),
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
    event.setChild(0);

    QAccessible::updateAccessibility(
        &event);
}

} // namespace QtMaterial
