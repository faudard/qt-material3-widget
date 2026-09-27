#include "qtmaterial/widgets/data/qtmaterialtreeview.h"

#include <QAbstractItemView>

namespace QtMaterial {

class QtMaterialTreeViewPrivate final
{
public:
    bool dense = false;
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
