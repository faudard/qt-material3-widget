
#include "qtmaterial/widgets/data/qtmaterialtable.h"

#include <QAbstractItemModel>
#include <QAccessible>
#include <QHeaderView>
#include <QKeyEvent>
#include <QPainter>
#include <QPen>
#include <QStyle>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>

namespace QtMaterial {
namespace {

class MaterialTableDelegate final : public QStyledItemDelegate
{
public:
    explicit MaterialTableDelegate(QObject* parent = nullptr)
        : QStyledItemDelegate(parent)
    {
    }

    void setSpec(const TableSpec& spec) { m_spec = spec; }

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        QStyleOptionViewItem opt(option);
        initStyleOption(&opt, index);

        painter->save();
        if (opt.state & QStyle::State_Selected) {
            painter->fillRect(opt.rect, m_spec.rowSelectedColor);
            opt.palette.setColor(QPalette::Text, m_spec.rowSelectedTextColor);
        } else if (opt.state & QStyle::State_MouseOver) {
            painter->fillRect(opt.rect, m_spec.rowHoverColor);
        }

        opt.font = m_spec.bodyFont;
        QStyledItemDelegate::paint(painter, opt, index);
        painter->restore();
    }

private:
    TableSpec m_spec = defaultTableSpec();
};

QString pluralize(int value, QStringView singular, QStringView plural)
{
    return QString::number(value) + QLatin1Char(' ') + (value == 1 ? singular.toString() : plural.toString());
}

} // namespace

QtMaterialTable::QtMaterialTable(QWidget* parent)
    : QTableView(parent)
    , m_spec(defaultTableSpec())
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
    return m_spec;
}

void QtMaterialTable::setSpec(const TableSpec& spec)
{
    m_spec = spec;
    applySpec();
    viewport()->update();
    update();
}

bool QtMaterialTable::dense() const
{
    return m_dense;
}

void QtMaterialTable::setDense(bool dense)
{
    if (m_dense == dense) {
        return;
    }

    m_dense = dense;
    applySpec();
    Q_EMIT denseChanged(m_dense);
}

QString QtMaterialTable::accessibilitySummary() const
{
    return m_accessibilitySummary;
}

QString QtMaterialTable::currentCellAccessibleText() const
{
    const QModelIndex index = currentIndex();
    if (!index.isValid()) {
        return QString();
    }

    const QString header = headerText(index.column());
    const QString value = cellText(index);
    const QString position = tr("row %1, column %2").arg(index.row() + 1).arg(index.column() + 1);

    if (header.isEmpty()) {
        return QStringLiteral("%1: %2").arg(position, value);
    }
    return QStringLiteral("%1, %2: %3").arg(position, header, value);
}

QString QtMaterialTable::rowAccessibleText(int row) const
{
    const QAbstractItemModel* currentModel = model();
    if (!currentModel || row < 0 || row >= currentModel->rowCount(rootIndex())) {
        return QString();
    }

    QStringList cells;
    for (int column = 0; column < currentModel->columnCount(rootIndex()); ++column) {
        const QModelIndex idx = currentModel->index(row, column, rootIndex());
        if (!idx.isValid()) {
            continue;
        }

        const QString header = headerText(column);
        const QString value = cellText(idx);
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
    painter.setPen(QPen(m_spec.focusRingColor, m_spec.focusRingWidth));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(viewport()->rect().adjusted(1, 1, -2, -2),
                            m_spec.cornerRadius,
                            m_spec.cornerRadius);
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
    const int rowHeight = m_dense ? qMax(32, m_spec.rowHeight - 12) : m_spec.rowHeight;
    verticalHeader()->setDefaultSectionSize(rowHeight);
    horizontalHeader()->setDefaultSectionSize(m_spec.headerHeight);
    horizontalHeader()->setMinimumHeight(m_spec.headerHeight);
    setGridStyle(Qt::SolidLine);
    setFont(m_spec.bodyFont);

    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, m_spec.backgroundColor);
    palette.setColor(QPalette::Text, m_spec.foregroundColor);
    palette.setColor(QPalette::Highlight, m_spec.rowSelectedColor);
    palette.setColor(QPalette::HighlightedText, m_spec.rowSelectedTextColor);
    palette.setColor(QPalette::Window, m_spec.backgroundColor);
    palette.setColor(QPalette::Button, m_spec.headerBackgroundColor);
    palette.setColor(QPalette::ButtonText, m_spec.headerForegroundColor);
    setPalette(palette);
    viewport()->setPalette(palette);

    if (auto* delegate = dynamic_cast<MaterialTableDelegate*>(itemDelegate())) {
        delegate->setSpec(m_spec);
    }

    const QString style = QStringLiteral(
                              "QTableView { background: %1; color: %2; gridline-color: %3; border: 1px solid %3; border-radius: %4px; }"
                              "QHeaderView::section { background: %5; color: %6; padding: 0 12px; border: 0; border-bottom: 1px solid %3; font-weight: 600; }"
                              "QTableView::item { padding: 0 12px; border: 0; }"
                              "QTableView::item:hover { background: rgba(%7,%8,%9,%10); }"
                              "QScrollBar { background: transparent; }")
                              .arg(m_spec.backgroundColor.name(), m_spec.foregroundColor.name(), m_spec.gridColor.name())
                              .arg(m_spec.cornerRadius)
                              .arg(m_spec.headerBackgroundColor.name(), m_spec.headerForegroundColor.name())
                              .arg(m_spec.rowHoverColor.red())
                              .arg(m_spec.rowHoverColor.green())
                              .arg(m_spec.rowHoverColor.blue())
                              .arg(m_spec.rowHoverColor.alpha());
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

    if (summary != m_accessibilitySummary) {
        m_accessibilitySummary = summary;
        Q_EMIT accessibilitySummaryChanged(m_accessibilitySummary);
        QAccessibleEvent event(this, QAccessible::DescriptionChanged);
        event.setChild(0);
        QAccessible::updateAccessibility(&event);
        // QAccessible::updateAccessibility({this, 0, QAccessible::DescriptionChanged});
    }
}

QString QtMaterialTable::headerText(int column) const
{
    const QAbstractItemModel* currentModel = model();
    if (!currentModel || column < 0 || column >= currentModel->columnCount(rootIndex())) {
        return QString();
    }

    return currentModel->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString();
}

QString QtMaterialTable::cellText(const QModelIndex& index) const
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
