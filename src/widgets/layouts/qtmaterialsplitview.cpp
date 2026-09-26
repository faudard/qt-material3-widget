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

    QWidget* pane = widget(index);
    if (!pane) {
        return;
    }

    QList<int> currentSizes = sizes();
    if (collapsed) {
        const int currentSize = currentSizes.value(index);
        if (currentSize > 0) {
            m_lastExpandedSize.insert(pane, currentSize);
        }
        if (!m_preCollapseCollapsible.contains(pane)) {
            m_preCollapseCollapsible.insert(pane, isCollapsible(index));
        }

        setCollapsible(index, true);
        currentSizes[index] = 0;
        setSizes(currentSizes);
    } else {
        const int fallbackSize =
            orientation() == Qt::Horizontal
                ? pane->sizeHint().width()
                : pane->sizeHint().height();
        currentSizes[index] =
            qMax(1, m_lastExpandedSize.take(pane, qMax(1, fallbackSize)));
        setSizes(currentSizes);

        const auto collapsibleIt = m_preCollapseCollapsible.find(pane);
        if (collapsibleIt != m_preCollapseCollapsible.end()) {
            setCollapsible(index, collapsibleIt.value());
            m_preCollapseCollapsible.erase(collapsibleIt);
        }
    }

    Q_EMIT paneCollapsedChanged(index, collapsed);
}

bool QtMaterialSplitView::paneCollapsed(int index) const
{
    const QList<int> currentSizes = sizes();
    return index >= 0 && index < currentSizes.size() && currentSizes.at(index) == 0;
}

} // namespace QtMaterial
