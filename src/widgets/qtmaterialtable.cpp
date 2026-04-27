#include "qtmaterial/widgets/qtmaterialtable.h"

#include <QFocusEvent>
#include <QHeaderView>
#include <QPainter>
#include <QScrollBar>
#include <QStyledItemDelegate>

namespace QtMaterial {

class MaterialTableDelegate final : public QStyledItemDelegate {
public:
    explicit MaterialTableDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

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

QtMaterialTable::QtMaterialTable(QWidget* parent)
    : QTableView(parent), m_spec(defaultTableSpec())
{
    setObjectName(QStringLiteral("QtMaterialTable"));
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
    applySpec();
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
    painter.drawRoundedRect(viewport()->rect().adjusted(1, 1, -2, -2), m_spec.cornerRadius, m_spec.cornerRadius);
}

void QtMaterialTable::focusInEvent(QFocusEvent* event)
{
    QTableView::focusInEvent(event);
    viewport()->update();
}

void QtMaterialTable::focusOutEvent(QFocusEvent* event)
{
    QTableView::focusOutEvent(event);
    viewport()->update();
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
        .arg(m_spec.backgroundColor.name(),
             m_spec.foregroundColor.name(),
             m_spec.gridColor.name())
        .arg(m_spec.cornerRadius)
        .arg(m_spec.headerBackgroundColor.name(),
             m_spec.headerForegroundColor.name())
        .arg(m_spec.rowHoverColor.red())
        .arg(m_spec.rowHoverColor.green())
        .arg(m_spec.rowHoverColor.blue())
        .arg(m_spec.rowHoverColor.alpha());
    setStyleSheet(style);
}

} // namespace QtMaterial
