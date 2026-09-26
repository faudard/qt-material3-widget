#include "qtmaterial/widgets/layouts/qtmaterialsplitview.h"

#include <QHash>

namespace QtMaterial {

class QtMaterialSplitViewPrivate final
{
public:
    QHash<QWidget*, int> lastExpandedSize;
    QHash<QWidget*, bool> preCollapseCollapsible;
};

QtMaterialSplitView::QtMaterialSplitView(QWidget* parent)
    : QtMaterialSplitView(Qt::Horizontal, parent)
{
}

QtMaterialSplitView::QtMaterialSplitView(Qt::Orientation orientation, QWidget* parent)
    : QSplitter(orientation, parent)
    , d_ptr(std::make_unique<QtMaterialSplitViewPrivate>())
{
    setObjectName(QStringLiteral("QtMaterialSplitView"));
    setAccessibleName(tr("Split view"));
    setChildrenCollapsible(false);
    setHandleWidth(8);
}

QtMaterialSplitView::~QtMaterialSplitView() = default;

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
            d_ptr->lastExpandedSize.insert(pane, currentSize);
        }
        if (!d_ptr->preCollapseCollapsible.contains(pane)) {
            d_ptr->preCollapseCollapsible.insert(pane, isCollapsible(index));
        }

        setCollapsible(index, true);
        currentSizes[index] = 0;
        setSizes(currentSizes);
    } else {
        const int fallbackSize =
            orientation() == Qt::Horizontal
                ? pane->sizeHint().width()
                : pane->sizeHint().height();
        const int restoredSize =
            d_ptr->lastExpandedSize.value(pane, qMax(1, fallbackSize));
        d_ptr->lastExpandedSize.remove(pane);
        currentSizes[index] = qMax(1, restoredSize);
        setSizes(currentSizes);

        const auto collapsibleIt = d_ptr->preCollapseCollapsible.find(pane);
        if (collapsibleIt != d_ptr->preCollapseCollapsible.end()) {
            setCollapsible(index, collapsibleIt.value());
            d_ptr->preCollapseCollapsible.erase(collapsibleIt);
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
