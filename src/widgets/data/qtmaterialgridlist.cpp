#include "qtmaterial/widgets/data/qtmaterialgridlist.h"

#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include <QResizeEvent>
#include <QStyledItemDelegate>
#include <QTextDocument>

namespace QtMaterial {

class MaterialGridListDelegate final : public QStyledItemDelegate {
public:
    explicit MaterialGridListDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

    void setSpec(const GridListSpec& spec) { m_spec = spec; }

    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const override
    {
        return m_spec.itemSize;
    }

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        const QRect card = option.rect.adjusted(4, 4, -4, -4);
        QColor bg = m_spec.itemBackgroundColor;
        QColor fg = m_spec.foregroundColor;
        if (option.state & QStyle::State_Selected) {
            bg = m_spec.itemSelectedColor;
            fg = m_spec.itemSelectedTextColor;
        } else if (option.state & QStyle::State_MouseOver) {
            bg = m_spec.itemHoverColor;
        }

        painter->setPen(Qt::NoPen);
        painter->setBrush(bg);
        painter->drawRoundedRect(card, m_spec.itemRadius, m_spec.itemRadius);

        const QRect imageRect = card.adjusted(12, 12, -12, -card.height() / 2);
        painter->setBrush(m_spec.supportingTextColor.lighter(160));
        painter->drawRoundedRect(imageRect, qMax(4, m_spec.itemRadius / 2), qMax(4, m_spec.itemRadius / 2));

        const QString title = index.data(Qt::DisplayRole).toString();
        const QString supporting = index.data(Qt::UserRole + 1).toString();

        QRect textRect = card.adjusted(12, card.height() / 2 + 4, -12, -12);
        painter->setPen(fg);
        painter->setFont(m_spec.titleFont);
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine, title);

        painter->setPen(m_spec.supportingTextColor);
        painter->setFont(m_spec.supportingFont);
        textRect.translate(0, painter->fontMetrics().height() + 4);
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, supporting);

        if (option.state & QStyle::State_HasFocus) {
            painter->setPen(QPen(m_spec.focusRingColor, m_spec.focusRingWidth));
            painter->setBrush(Qt::NoBrush);
            painter->drawRoundedRect(card.adjusted(1, 1, -1, -1), m_spec.itemRadius, m_spec.itemRadius);
        }

        painter->restore();
    }

private:
    GridListSpec m_spec = defaultGridListSpec();
};

QtMaterialGridList::QtMaterialGridList(QWidget* parent)
    : QListView(parent), m_spec(defaultGridListSpec())
{
    setObjectName(QStringLiteral("QtMaterialGridList"));
    setViewMode(QListView::IconMode);
    setResizeMode(QListView::Adjust);
    setMovement(QListView::Static);
    setWrapping(true);
    setUniformItemSizes(true);
    setMouseTracking(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setFocusPolicy(Qt::StrongFocus);
    setItemDelegate(new MaterialGridListDelegate(this));
    applySpec();
}

QtMaterialGridList::~QtMaterialGridList() = default;

GridListSpec QtMaterialGridList::spec() const
{
    return m_spec;
}

void QtMaterialGridList::setSpec(const GridListSpec& spec)
{
    m_spec = spec;
    applySpec();
}

int QtMaterialGridList::columns() const
{
    return m_columns;
}

void QtMaterialGridList::setColumns(int columns)
{
    m_columns = qMax(0, columns);
    updateGridSize();
}

void QtMaterialGridList::resizeEvent(QResizeEvent* event)
{
    QListView::resizeEvent(event);
    updateGridSize();
}

void QtMaterialGridList::paintEvent(QPaintEvent* event)
{
    QListView::paintEvent(event);
}

void QtMaterialGridList::applySpec()
{
    setSpacing(m_spec.spacing);
    setGridSize(m_spec.itemSize + QSize(m_spec.spacing, m_spec.spacing));
    setFont(m_spec.titleFont);

    if (auto* delegate = dynamic_cast<MaterialGridListDelegate*>(itemDelegate())) {
        delegate->setSpec(m_spec);
    }

    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, m_spec.backgroundColor);
    palette.setColor(QPalette::Window, m_spec.backgroundColor);
    palette.setColor(QPalette::Text, m_spec.foregroundColor);
    palette.setColor(QPalette::Highlight, m_spec.itemSelectedColor);
    palette.setColor(QPalette::HighlightedText, m_spec.itemSelectedTextColor);
    setPalette(palette);
    viewport()->setPalette(palette);
    setStyleSheet(QStringLiteral("QListView { background:%1; border:0; outline:0; }").arg(m_spec.backgroundColor.name()));
    updateGridSize();
}

void QtMaterialGridList::updateGridSize()
{
    if (m_columns <= 0) {
        setGridSize(m_spec.itemSize + QSize(m_spec.spacing, m_spec.spacing));
        return;
    }
    const int available = qMax(1, viewport()->width() - (m_columns + 1) * m_spec.spacing);
    const int width = qMax(80, available / m_columns);
    QSize size(width, m_spec.itemSize.height());
    setGridSize(size + QSize(m_spec.spacing, m_spec.spacing));
}

} // namespace QtMaterial
