#include "qtmaterial/widgets/data/qtmaterialtreeview.h"

#include <QAbstractItemView>
#include <QAbstractItemModel>
#include <QItemSelectionModel>

namespace QtMaterial {

class QtMaterialTreeViewPrivate final
{
public:
    bool dense = false;
    QString accessibilitySummary;
};

QtMaterialTreeView::QtMaterialTreeView(QWidget* parent)
    : QTreeView(parent)
    , d_ptr(std::make_unique<QtMaterialTreeViewPrivate>())
{
    setObjectName(QStringLiteral("QtMaterialTreeView"));
    setAccessibleName(tr("Tree"));
    setFocusPolicy(Qt::StrongFocus);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setUniformRowHeights(true);
    setAnimated(true);
    setAllColumnsShowFocus(true);
    setExpandsOnDoubleClick(true);
    setIndentation(20);
    setDragDropMode(QAbstractItemView::NoDragDrop);
    connect(this, &QTreeView::expanded, this, [this](const QModelIndex&) { syncAccessibility(); });
    connect(this, &QTreeView::collapsed, this, [this](const QModelIndex&) { syncAccessibility(); });
    syncAccessibility();
}

QtMaterialTreeView::~QtMaterialTreeView() = default;

bool QtMaterialTreeView::dense() const noexcept
{
    return d_ptr->dense;
}

void QtMaterialTreeView::setDense(bool dense)
{
    if (d_ptr->dense == dense) {
        return;
    }

    d_ptr->dense = dense;
    setIndentation(d_ptr->dense ? 16 : 20);
    setIconSize(d_ptr->dense ? QSize(18, 18) : QSize(20, 20));
    viewport()->update();
    Q_EMIT denseChanged(d_ptr->dense);
}

bool QtMaterialTreeView::multiSelectionEnabled() const noexcept
{
    return selectionMode() == QAbstractItemView::ExtendedSelection
        || selectionMode() == QAbstractItemView::MultiSelection;
}

void QtMaterialTreeView::setMultiSelectionEnabled(bool enabled)
{
    if (multiSelectionEnabled() == enabled) {
        return;
    }

    setSelectionMode(
        enabled
            ? QAbstractItemView::ExtendedSelection
            : QAbstractItemView::SingleSelection);
    Q_EMIT multiSelectionEnabledChanged(enabled);
}

bool QtMaterialTreeView::dragDropEnabled() const noexcept
{
    return dragDropMode() == QAbstractItemView::InternalMove;
}

QString QtMaterialTreeView::accessibilitySummary() const
{
    return d_ptr->accessibilitySummary;
}

QString QtMaterialTreeView::currentItemAccessibleText() const
{
    const QModelIndex index = currentIndex();
    if (!index.isValid()) {
        return QString();
    }

    QString text = index.data(Qt::AccessibleTextRole).toString();
    if (text.isEmpty()) {
        text = index.data(Qt::DisplayRole).toString();
    }

    QString state;
    if (model() && model()->hasChildren(index)) {
        state = isExpanded(index)
            ? tr("expanded")
            : tr("collapsed");
    }

    QString result = tr("Row %1, column %2").arg(index.row() + 1).arg(index.column() + 1);
    if (!text.isEmpty()) {
        result += QStringLiteral(", ") + text;
    }
    if (!state.isEmpty()) {
        result += QStringLiteral(", ") + state;
    }
    return result;
}

void QtMaterialTreeView::setModel(QAbstractItemModel* model)
{
    if (this->model()) {
        disconnect(this->model(), nullptr, this, nullptr);
    }

    QTreeView::setModel(model);

    if (selectionModel()) {
        connect(
            selectionModel(),
            &QItemSelectionModel::currentChanged,
            this,
            [this](const QModelIndex&, const QModelIndex&) {
                syncAccessibility();
            });
    }

    if (model) {
        connect(model, &QAbstractItemModel::modelReset, this, &QtMaterialTreeView::syncAccessibility);
        connect(model, &QAbstractItemModel::rowsInserted, this, &QtMaterialTreeView::syncAccessibility);
        connect(model, &QAbstractItemModel::rowsRemoved, this, &QtMaterialTreeView::syncAccessibility);
        connect(model, &QAbstractItemModel::dataChanged, this, [this]() { syncAccessibility(); });
    }

    syncAccessibility();
}

void QtMaterialTreeView::currentChanged(
    const QModelIndex& current,
    const QModelIndex& previous)
{
    QTreeView::currentChanged(current, previous);
    syncAccessibility();
}

void QtMaterialTreeView::syncAccessibility()
{
    const int roots = model() ? model()->rowCount(rootIndex()) : 0;
    QString summary = tr("%1 top-level items").arg(roots);
    const QString current = currentItemAccessibleText();
    if (!current.isEmpty()) {
        summary += QStringLiteral(". ") + current;
    }

    if (d_ptr->accessibilitySummary == summary) {
        return;
    }

    d_ptr->accessibilitySummary = summary;
    setAccessibleDescription(summary);
    Q_EMIT accessibilitySummaryChanged(summary);
}

void QtMaterialTreeView::setDragDropEnabled(bool enabled)
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

} // namespace QtMaterial
