#include "qtmaterial/widgets/layouts/qtmaterialsplitview.h"

#include <QHash>
#include <QKeyEvent>
#include <QSplitterHandle>

namespace QtMaterial {

class QtMaterialSplitViewPrivate final
{
public:
    QHash<QWidget*, int> lastExpandedSize;
    QHash<QWidget*, bool> preCollapseCollapsible;
};

namespace {

constexpr int kKeyboardResizeStep = 16;

class QtMaterialSplitHandle final : public QSplitterHandle
{
public:
    QtMaterialSplitHandle(
        Qt::Orientation orientation,
        QSplitter* parent)
        : QSplitterHandle(orientation, parent)
    {
        setFocusPolicy(Qt::StrongFocus);
        setAccessibleName(
            QtMaterialSplitView::tr("Split handle"));
        setAccessibleDescription(
            QtMaterialSplitView::tr(
                "Use arrow keys to resize adjacent panes"));
    }

protected:
    void keyPressEvent(QKeyEvent* event) override
    {
        if (!event) {
            return;
        }

        int physicalDelta = 0;
        if (orientation() == Qt::Horizontal) {
            if (event->key() == Qt::Key_Left) {
                physicalDelta = -kKeyboardResizeStep;
            } else if (event->key() == Qt::Key_Right) {
                physicalDelta = kKeyboardResizeStep;
            }
        } else {
            if (event->key() == Qt::Key_Up) {
                physicalDelta = -kKeyboardResizeStep;
            } else if (event->key() == Qt::Key_Down) {
                physicalDelta = kKeyboardResizeStep;
            }
        }

        if (physicalDelta == 0) {
            QSplitterHandle::keyPressEvent(event);
            return;
        }

        if (event->modifiers().testFlag(Qt::ShiftModifier)) {
            physicalDelta *= 4;
        }

        QSplitter* owner = splitter();
        if (!owner) {
            QSplitterHandle::keyPressEvent(event);
            return;
        }

        int handleIndex = -1;
        for (int index = 1; index < owner->count(); ++index) {
            if (owner->handle(index) == this) {
                handleIndex = index;
                break;
            }
        }

        if (handleIndex <= 0) {
            QSplitterHandle::keyPressEvent(event);
            return;
        }

        QList<int> paneSizes = owner->sizes();
        if (handleIndex >= paneSizes.size()) {
            QSplitterHandle::keyPressEvent(event);
            return;
        }

        int logicalDelta = physicalDelta;
        if (orientation() == Qt::Horizontal
            && owner->layoutDirection() == Qt::RightToLeft) {
            logicalDelta = -logicalDelta;
        }

        const int firstIndex = handleIndex - 1;
        const int secondIndex = handleIndex;
        const int total =
            paneSizes.at(firstIndex)
            + paneSizes.at(secondIndex);

        int firstSize =
            qBound(
                0,
                paneSizes.at(firstIndex) + logicalDelta,
                total);
        int secondSize = total - firstSize;

        paneSizes[firstIndex] = firstSize;
        paneSizes[secondIndex] = secondSize;
        owner->setSizes(paneSizes);

        event->accept();
    }
};

} // namespace

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

QSplitterHandle* QtMaterialSplitView::createHandle()
{
    return new QtMaterialSplitHandle(
        orientation(),
        this);
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
