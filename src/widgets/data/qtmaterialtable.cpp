
#include "qtmaterial/widgets/data/qtmaterialtable.h"
#include <QTableView>

#include <QAbstractItemModel>
#include <QAccessible>
#include <QHeaderView>
#include <QKeyEvent>
#include <QPainter>
#include <QPen>
#include <QStyle>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>
#include <memory>

namespace QtMaterial {

struct QtMaterialTablePrivate
{
    TableSpec m_spec = defaultTableSpec();
    bool m_dense = false;
    QString m_accessibilitySummary;
};


namespace {

class MaterialTableDelegate final : public QStyledItemDelegate
{
public:
    explicit MaterialTableDelegate(QObject* parent = nullptr)
        : QStyledItemDelegate(parent)
    {
    }

    void setSpec(const TableSpec& spec) { d_ptr.m_spec = spec; }

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        QStyleOptionViewItem opt(option);
        initStyleOption(&opt, index);

        painter->save();
        if (opt.state & QStyle::State_Selected) {
            painter->fillRect(opt.rect, d_ptr.m_spec.rowSelectedColor);
            opt.palette.setColor(QPalette::Text, d_ptr.m_spec.rowSelectedTextColor);
        } else if (opt.state & QStyle::State_MouseOver) {
            painter->fillRect(opt.rect, d_ptr.m_spec.rowHoverColor);
        }

        opt.font = d_ptr.m_spec.bodyFont;
        QStyledItemDelegate::paint(painter, opt, index);
        painter->restore();
    }

private:
    TableSpec d_ptr.m_spec = defaultTableSpec();
};

QString pluralize(int value, QStringView singular, QStringView plural)
{
    return QString::number(value) + QLatin1Char(' ') + (value == 1 ? singular.toString() : plural.toString());
}


QString tableHeaderText(const QtMaterialTable* table, int column);
QString tableCellText(const QModelIndex& index);

} // namespace

QtMaterialTable::QtMaterialTable(QWidget* parent)
    : QTableView(parent)
    , d_ptr(std::make_unique<QtMaterialTablePrivate>())
{

    setObjectName(QStringLiteral("QtMaterialTable"));
    setAccessibleName(QStringLiteral("Table"));
    setAlternatingRowColors(false);
    setMouseTracking(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setShowGrid(true);
    setFocusPolicy(Qt::StrongFocus);
    setSortingEnabled(true);
    setItemDelegate(new MaterialTableDelegate(this));

    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->setHighlightSections(false);
    verticalHeader()->setVisible(false);

    connect(selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &QtMaterialTable::syncAccessibility);

    applySpec();
    syncAccessibility();
}

QtMaterialTable::~QtMaterialTable() = default;

TableSpec QtMaterialTable::spec() const
{
    return d_ptr->m_spec;
}

void QtMaterialTable::setSpec(const TableSpec& spec)
{
    d_ptr->m_spec = spec;
    applySpec();
    viewport()->update();
    update();
}

bool QtMaterialTable::dense() const
{
    return d_ptr->m_dense;
}

void QtMaterialTable::setDense(bool dense)
{
    if (d_ptr->m_dense == dense) {
        return;
    }

    d_ptr->m_dense = dense;
    applySpec();
    Q_EMIT denseChanged(d_ptr->m_dense);
}

QString QtMaterialTable::accessibilitySummary() const
{
    return d_ptr->m_accessibilitySummary;
}

QString QtMaterialTable::currentCellAccessibleText() const
{
    const QModelIndex index = currentIndex();
    if (!index.isValid()) {
        return QString();
    }

    const QString header = tableHeaderText(this, index.column());
    const QString value = tableCellText(index);
    const QString position = tr("row %1, column %2").arg(index.row() + 1).arg(index.column() + 1);

    if (header.isEmpty()) {
        return QStringLiteral("%1: %2").arg(position, value);
    }
    return QStringLiteral("%1, %2: %3").arg(position, header, value);
}

QString QtMaterialTable::rowAccessibleText(int row) const
{
    const QAbstractItemModel* currentModel = table->model();
    if (!currentModel || row < 0 || row >= currentModel->rowCount(rootIndex())) {
        return QString();
    }

    QStringList cells;
    for (int column = 0; column < currentModel->columnCount(rootIndex()); ++column) {
        const QModelIndex idx = currentModel->index(row, column, rootIndex());
        if (!idx.isValid()) {
            continue;
        }

        const QString header = tableHeaderText(this, column);
        const QString value = tableCellText(idx);
        if (value.isEmpty()) {
            continue;
        }
        cells.push_back(header.isEmpty() ? value : QStringLiteral("%1: %2").arg(header, value));
    }

    if (cells.isEmpty()) {
        return tr("Row %1").arg(row + 1);
    }
    return tr("Row %1, %2").arg(row + 1).arg(cells.join(QStringLiteral(", ")));
}

void QtMaterialTable::activateCurrentRow()
{
    const QModelIndex index = currentIndex();
    if (!index.isValid()) {
        return;
    }

    selectRow(index.row());
    Q_EMIT activated(index);
    Q_EMIT rowActivated(index.row());
}

void QtMaterialTable::paintEvent(QPaintEvent* event)
{
    QTableView::paintEvent(event);

    if (!hasFocus()) {
        return;
    }

    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(d_ptr->m_spec.focusRingColor, d_ptr->m_spec.focusRingWidth));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(viewport()->rect().adjusted(1, 1, -2, -2),
                            d_ptr->m_spec.cornerRadius,
                            d_ptr->m_spec.cornerRadius);
}

void QtMaterialTable::focusInEvent(QFocusEvent* event)
{
    QTableView::focusInEvent(event);
    syncAccessibility();
    viewport()->update();
}

void QtMaterialTable::focusOutEvent(QFocusEvent* event)
{
    QTableView::focusOutEvent(event);
    viewport()->update();
}

void QtMaterialTable::keyPressEvent(QKeyEvent* event)
{
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

void QtMaterialTable::setModel(QAbstractItemModel* model)
{
    if (this->model()) {
        disconnect(this->model(), nullptr, this, nullptr);
    }

    QTableView::setModel(model);

    if (selectionModel()) {
        connect(selectionModel(), &QItemSelectionModel::selectionChanged,
                this, &QtMaterialTable::syncAccessibility,
                Qt::UniqueConnection);
    }

    if (model) {
        connect(model, &QAbstractItemModel::modelReset,
                this, &QtMaterialTable::syncAccessibility);
        connect(model, &QAbstractItemModel::rowsInserted,
                this, &QtMaterialTable::syncAccessibility);
        connect(model, &QAbstractItemModel::rowsRemoved,
                this, &QtMaterialTable::syncAccessibility);
        connect(model, &QAbstractItemModel::columnsInserted,
                this, &QtMaterialTable::syncAccessibility);
        connect(model, &QAbstractItemModel::columnsRemoved,
                this, &QtMaterialTable::syncAccessibility);
        connect(model, &QAbstractItemModel::dataChanged,
                this, &QtMaterialTable::syncAccessibility);
        connect(model, &QAbstractItemModel::headerDataChanged,
                this, &QtMaterialTable::syncAccessibility);
    }

    syncAccessibility();
}

void QtMaterialTable::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
    QTableView::currentChanged(current, previous);
    syncAccessibility();
}

void QtMaterialTable::applySpec()
{
    const int rowHeight = d_ptr->m_dense ? qMax(32, d_ptr->m_spec.rowHeight - 12) : d_ptr->m_spec.rowHeight;
    verticalHeader()->setDefaultSectionSize(rowHeight);
    horizontalHeader()->setDefaultSectionSize(d_ptr->m_spec.headerHeight);
    horizontalHeader()->setMinimumHeight(d_ptr->m_spec.headerHeight);
    setGridStyle(Qt::SolidLine);
    setFont(d_ptr->m_spec.bodyFont);

    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, d_ptr->m_spec.backgroundColor);
    palette.setColor(QPalette::Text, d_ptr->m_spec.foregroundColor);
    palette.setColor(QPalette::Highlight, d_ptr->m_spec.rowSelectedColor);
    palette.setColor(QPalette::HighlightedText, d_ptr->m_spec.rowSelectedTextColor);
    palette.setColor(QPalette::Window, d_ptr->m_spec.backgroundColor);
    palette.setColor(QPalette::Button, d_ptr->m_spec.headerBackgroundColor);
    palette.setColor(QPalette::ButtonText, d_ptr->m_spec.headerForegroundColor);
    setPalette(palette);
    viewport()->setPalette(palette);

    if (auto* delegate = dynamic_cast<MaterialTableDelegate*>(itemDelegate())) {
        delegate->setSpec(d_ptr->m_spec);
    }

    const QString style = QStringLiteral(
                              "QTableView { background: %1; color: %2; gridline-color: %3; border: 1px solid %3; border-radius: %4px; }"
                              "QHeaderView::section { background: %5; color: %6; padding: 0 12px; border: 0; border-bottom: 1px solid %3; font-weight: 600; }"
                              "QTableView::item { padding: 0 12px; border: 0; }"
                              "QTableView::item:hover { background: rgba(%7,%8,%9,%10); }"
                              "QScrollBar { background: transparent; }")
                              .arg(d_ptr->m_spec.backgroundColor.name(), d_ptr->m_spec.foregroundColor.name(), d_ptr->m_spec.gridColor.name())
                              .arg(d_ptr->m_spec.cornerRadius)
                              .arg(d_ptr->m_spec.headerBackgroundColor.name(), d_ptr->m_spec.headerForegroundColor.name())
                              .arg(d_ptr->m_spec.rowHoverColor.red())
                              .arg(d_ptr->m_spec.rowHoverColor.green())
                              .arg(d_ptr->m_spec.rowHoverColor.blue())
                              .arg(d_ptr->m_spec.rowHoverColor.alpha());
    setStyleSheet(style);
}

void QtMaterialTable::syncAccessibility()
{
    const QAbstractItemModel* currentModel = model();
    const int rows = currentModel ? currentModel->rowCount(rootIndex()) : 0;
    const int columns = currentModel ? currentModel->columnCount(rootIndex()) : 0;

    QStringList parts;
    parts << pluralize(rows, QStringLiteral("row"), QStringLiteral("rows"));
    parts << pluralize(columns, QStringLiteral("column"), QStringLiteral("columns"));

    if (currentIndex().isValid()) {
        parts << currentCellAccessibleText();
    }

    const QString summary = parts.join(QStringLiteral(", "));
    setAccessibleDescription(summary);

    if (summary != d_ptr->m_accessibilitySummary) {
        d_ptr->m_accessibilitySummary = summary;
        Q_EMIT accessibilitySummaryChanged(d_ptr->m_accessibilitySummary);
        QAccessibleEvent event(this, QAccessible::DescriptionChanged);
        event.setChild(0);
        QAccessible::updateAccessibility(&event);
        // QAccessible::updateAccessibility({this, 0, QAccessible::DescriptionChanged});
    }
}

QString tableHeaderText(const QtMaterialTable* table, int column)
{
    const QAbstractItemModel* currentModel = table->model();
    if (!currentModel || column < 0 || column >= currentModel->columnCount(table->rootIndex())) {
        return QString();
    }

    return currentModel->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString();
}

QString tableCellText(const QModelIndex& index)
{
    if (!index.isValid()) {
        return QString();
    }

    const QVariant accessible = index.data(Qt::AccessibleTextRole);
    if (accessible.isValid() && !accessible.toString().isEmpty()) {
        return accessible.toString();
    }

    return index.data(Qt::DisplayRole).toString();
}

} // namespace QtMaterial
