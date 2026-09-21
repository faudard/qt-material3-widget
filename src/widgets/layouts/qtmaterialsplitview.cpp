#include "qtmaterial/widgets/layouts/qtmaterialsplitview.h"

namespace QtMaterial {

QtMaterialSplitView::QtMaterialSplitView(QWidget* parent)
    : QtMaterialSplitView(Qt::Horizontal, parent)
{
}

QtMaterialSplitView::QtMaterialSplitView(Qt::Orientation orientation, QWidget* parent)
    : QSplitter(orientation, parent)
{
    setObjectName(QStringLiteral("QtMaterialSplitView"));
    setAccessibleName(tr("Split view"));
    setChildrenCollapsible(false);
    setHandleWidth(8);
}

void QtMaterialSplitView::setPaneCollapsible(int index, bool collapsible)
{
    if (index < 0 || index >= count()) {
        return;
    }
    setCollapsible(index, collapsible);
}

bool QtMaterialSplitView::paneCollapsible(int index) const
{
    return index >= 0 && index < count() && isCollapsible(index);
}

void QtMaterialSplitView::setPaneCollapsed(int index, bool collapsed)
{
    if (index < 0 || index >= count() || paneCollapsed(index) == collapsed) {
        return;
    }

    QList<int> currentSizes = sizes();
    if (collapsed) {
        m_lastExpandedSizes = currentSizes;
        currentSizes[index] = 0;
        setCollapsible(index, true);
        setSizes(currentSizes);
    } else if (m_lastExpandedSizes.size() == count()) {
        setSizes(m_lastExpandedSizes);
    } else {
        currentSizes[index] = qMax(1, currentSizes.value(index));
        setSizes(currentSizes);
    }

    Q_EMIT paneCollapsedChanged(index, collapsed);
}

bool QtMaterialSplitView::paneCollapsed(int index) const
{
    const QList<int> currentSizes = sizes();
    return index >= 0 && index < currentSizes.size() && currentSizes.at(index) == 0;
}

} // namespace QtMaterial
