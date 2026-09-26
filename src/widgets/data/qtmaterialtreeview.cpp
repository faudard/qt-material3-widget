#include "qtmaterial/widgets/data/qtmaterialtreeview.h"

#include <QAbstractItemView>

namespace QtMaterial {

QtMaterialTreeView::QtMaterialTreeView(QWidget* parent)
    : QTreeView(parent)
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

bool QtMaterialTreeView::dense() const noexcept
{
    return m_dense;
}

void QtMaterialTreeView::setDense(bool dense)
{
    if (m_dense == dense) {
        return;
    }

    m_dense = dense;
    setIndentation(m_dense ? 16 : 20);
    setIconSize(m_dense ? QSize(18, 18) : QSize(20, 20));
    viewport()->update();
    Q_EMIT denseChanged(m_dense);
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
