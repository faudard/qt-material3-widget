#include "qtmaterial/widgets/layouts/qtmaterialsplitview.h"

#include <QHash>
#include <QDataStream>
#include <QHideEvent>
#include <QPointer>
#include <QShowEvent>
#include <QSet>
#include <QKeyEvent>
#include <QMargins>
#include <QMouseEvent>
#include <QSplitterHandle>
#include <QTimer>
#include <QVariantAnimation>

namespace QtMaterial {

class QtMaterialSplitViewPrivate final
{
public:
    QHash<QWidget*, int> lastExpandedSize;
    QHash<QWidget*, bool> preCollapseCollapsible;
    QSet<QWidget*> trackedPanes;
    QSet<QWidget*> explicitPanePolicies;
    QList<int> defaultPaneSizes;
    int keyboardResizeStep = 16;
    bool animatedCollapseEnabled = false;
    int collapseAnimationDuration = 180;
    bool rememberPaneSizes = false;
    QByteArray rememberedState;
    QTimer* persistTimer = nullptr;
    QVariantAnimation* animation = nullptr;
    QPointer<QWidget> animationPane;
    QList<int> animationStart;
    QList<int> animationTarget;
    bool animationCollapsed = false;
    Qt::Orientation animationOrientation = Qt::Horizontal;
    int animationMinimum = 0;
    QSizePolicy animationPolicy;
    bool constraintsRelaxed = false;
};

namespace {

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

        auto* owner = qobject_cast<QtMaterialSplitView*>(splitter());
        if (!owner) { QSplitterHandle::keyPressEvent(event); return; }
        const int handleIndex = owner->indexOf(this);
        if (handleIndex <= 0) { QSplitterHandle::keyPressEvent(event); return; }
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            owner->setPaneCollapsed(handleIndex - 1, !owner->paneCollapsed(handleIndex - 1));
            event->accept();
            return;
        }
        int delta = 0;
        const int step = owner->keyboardResizeStep()
            * (event->modifiers().testFlag(Qt::ShiftModifier) ? 4 : 1);
        if (orientation() == Qt::Horizontal) {
            if (event->key() == Qt::Key_Left) { delta = -step; }
            if (event->key() == Qt::Key_Right) { delta = step; }
        } else {
            if (event->key() == Qt::Key_Up) { delta = -step; }
            if (event->key() == Qt::Key_Down) { delta = step; }
        }
        const int currentPosition = orientation() == Qt::Horizontal ? x() : y();
        const int extent = orientation() == Qt::Horizontal ? owner->width() : owner->height();
        int position = currentPosition + delta;
        if (event->key() == Qt::Key_Home) { position = 0; }
        else if (event->key() == Qt::Key_End) { position = extent; }
        else if (delta == 0) { QSplitterHandle::keyPressEvent(event); return; }
        // Qt moves the boundary before the next pane. In RTL that boundary is
        // at the handle's right edge, rather than its physical left coordinate.
        // Include the grab-area margins used for very thin native handles.
        const QMargins margins = contentsMargins();
        if (orientation() == Qt::Horizontal) {
            position += owner->isRightToLeft() ? width() - margins.right() : margins.left();
        } else {
            position += margins.top();
        }
        moveSplitter(closestLegalPosition(position));

        event->accept();
    }

    void mouseDoubleClickEvent(QMouseEvent* event) override
    {
        auto* owner = qobject_cast<QtMaterialSplitView*>(splitter());
        if (owner && event->button() == Qt::LeftButton) {
            owner->resetPaneSizes();
            if (event) {
                event->accept();
            }
            return;
        }
        QSplitterHandle::mouseDoubleClickEvent(event);
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
    d_ptr->animation = new QVariantAnimation(this);
    d_ptr->animation->setStartValue(0.0);
    d_ptr->animation->setEndValue(1.0);
    d_ptr->animation->setEasingCurve(QEasingCurve::InOutCubic);
    connect(d_ptr->animation, &QVariantAnimation::valueChanged, this, [this](const QVariant& value) {
        if (!d_ptr->animationPane || this->orientation() != d_ptr->animationOrientation
            || count() != d_ptr->animationStart.size()) { finishCollapseAnimation(); return; }
        QList<int> interpolated;
        const qreal progress = value.toReal();
        for (int i = 0; i < count(); ++i) {
            interpolated.push_back(qRound(d_ptr->animationStart.at(i)
                + (d_ptr->animationTarget.at(i) - d_ptr->animationStart.at(i)) * progress));
        }
        setSizes(interpolated);
    });
    connect(d_ptr->animation, &QVariantAnimation::finished, this, &QtMaterialSplitView::finishCollapseAnimation);
    d_ptr->persistTimer = new QTimer(this);
    d_ptr->persistTimer->setSingleShot(true);
    d_ptr->persistTimer->setInterval(200);
    connect(d_ptr->persistTimer, &QTimer::timeout, this, &QtMaterialSplitView::persistPaneState);
    connect(this, &QSplitter::splitterMoved, this, [this]() {
        d_ptr->persistTimer->start();
    });
}

QtMaterialSplitView::~QtMaterialSplitView()
{
    d_ptr->animation->stop();
    d_ptr->persistTimer->stop();
    // QSplitter deletes its panes after our private state has been destroyed.
    // Disconnect callbacks that capture that state while it is still alive.
    for (QWidget* pane : d_ptr->trackedPanes) {
        disconnect(pane, nullptr, this, nullptr);
    }
    disconnect(d_ptr->animation, nullptr, this, nullptr);
    disconnect(d_ptr->persistTimer, nullptr, this, nullptr);
    disconnect(this, nullptr, this, nullptr);
}

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
    trackPane(widget(index));
    d_ptr->explicitPanePolicies.insert(widget(index));
    if (d_ptr->preCollapseCollapsible.contains(widget(index))) {
        d_ptr->preCollapseCollapsible[widget(index)] = collapsible;
        return;
    }
    setCollapsible(index, collapsible);
}

bool QtMaterialSplitView::paneCollapsible(int index) const
{
    if (index < 0 || index >= count()) { return false; }
    QWidget* pane = widget(index);
    if (d_ptr->preCollapseCollapsible.contains(pane)) {
        return d_ptr->preCollapseCollapsible.value(pane);
    }
    // Qt's isCollapsible() also returns true for its internal default flag,
    // even when that pane inherits childrenCollapsible() == false.
    return isCollapsible(index)
        && (d_ptr->explicitPanePolicies.contains(pane) || childrenCollapsible());
}

void QtMaterialSplitView::setPaneCollapsed(int index, bool collapsed)
{
    if (index < 0 || index >= count() || paneCollapsed(index) == collapsed) {
        return;
    }

    finishCollapseAnimation();
    QWidget* pane = widget(index);
    if (!pane) { return; }
    trackPane(pane);
    const QList<int> startSizes = sizes();
    QList<int> targetSizes = startSizes;
    if (collapsed) {
        bool anotherExpanded = false;
        for (int i = 0; i < count(); ++i) { anotherExpanded |= i != index && startSizes.value(i) > 0; }
        if (isVisible() && !anotherExpanded) { return; }
        d_ptr->lastExpandedSize.insert(pane, qMax(1, startSizes.value(index)));
        d_ptr->preCollapseCollapsible.insert(pane, paneCollapsible(index));
        setCollapsible(index, true);
        targetSizes[index] = 0;
    } else {
        const int hint = orientation() == Qt::Horizontal ? pane->sizeHint().width() : pane->sizeHint().height();
        targetSizes[index] = qMax(1, d_ptr->lastExpandedSize.value(pane, qMax(1, hint)));
        int total = 0, otherTotal = 0;
        for (int i = 0; i < startSizes.size(); ++i) {
            total += startSizes.at(i);
            if (i != index) { otherTotal += startSizes.at(i); }
        }
        if (otherTotal > 0) {
            targetSizes[index] = qMin(targetSizes[index], qMax(1, total));
            const int remainder = qMax(0, total - targetSizes[index]);
            for (int i = 0; i < startSizes.size(); ++i) {
                if (i != index) { targetSizes[i] = qRound(qreal(startSizes.at(i)) * remainder / otherTotal); }
            }
        }
    }
    // Normalize target sizes with Qt before interpolating a constant total.
    setSizes(targetSizes);
    targetSizes = sizes();
    if (!d_ptr->animatedCollapseEnabled || d_ptr->collapseAnimationDuration == 0 || !isVisible()) {
        d_ptr->animationPane = pane;
        d_ptr->animationTarget = targetSizes;
        d_ptr->animationCollapsed = collapsed;
        finishCollapseAnimation();
        return;
    }
    d_ptr->animationPane = pane;
    d_ptr->animationTarget = targetSizes;
    d_ptr->animationCollapsed = collapsed;
    d_ptr->animationOrientation = orientation();
    d_ptr->animationMinimum = orientation() == Qt::Horizontal ? pane->minimumWidth() : pane->minimumHeight();
    d_ptr->animationPolicy = pane->sizePolicy();
    d_ptr->constraintsRelaxed = true;
    QSizePolicy policy = pane->sizePolicy();
    if (orientation() == Qt::Horizontal) {
        pane->setMinimumWidth(0);
        policy.setHorizontalPolicy(QSizePolicy::Ignored);
    } else {
        pane->setMinimumHeight(0);
        policy.setVerticalPolicy(QSizePolicy::Ignored);
    }
    pane->setSizePolicy(policy);
    setSizes(startSizes);
    d_ptr->animationStart = startSizes;
    d_ptr->animation->setDuration(d_ptr->collapseAnimationDuration);
    d_ptr->animation->start();
}

bool QtMaterialSplitView::paneCollapsed(int index) const
{
    const QList<int> currentSizes = sizes();
    if (index < 0 || index >= count()) { return false; }
    QWidget* pane = widget(index);
    if (d_ptr->animationPane == pane) { return d_ptr->animationCollapsed; }
    return d_ptr->preCollapseCollapsible.contains(pane)
        || (isVisible() && currentSizes.value(index) == 0);
}

void QtMaterialSplitView::setPaneMinimumExtent(
    int index,
    int extent)
{
    QWidget* pane =
        index >= 0 && index < count()
            ? widget(index)
            : nullptr;
    if (!pane) {
        return;
    }

    const int normalized = qMax(0, extent);
    if (d_ptr->animationPane == pane && d_ptr->constraintsRelaxed
        && d_ptr->animationOrientation == orientation()) {
        d_ptr->animationMinimum = normalized;
        return;
    }
    if (orientation() == Qt::Horizontal) {
        pane->setMinimumWidth(normalized);
    } else {
        pane->setMinimumHeight(normalized);
    }
}

int QtMaterialSplitView::paneMinimumExtent(
    int index) const
{
    QWidget* pane =
        index >= 0 && index < count()
            ? widget(index)
            : nullptr;
    if (!pane) {
        return 0;
    }

    if (d_ptr->animationPane == pane && d_ptr->constraintsRelaxed
        && d_ptr->animationOrientation == orientation()) {
        return d_ptr->animationMinimum;
    }

    return orientation() == Qt::Horizontal
        ? pane->minimumWidth()
        : pane->minimumHeight();
}

void QtMaterialSplitView::setPaneMaximumExtent(
    int index,
    int extent)
{
    QWidget* pane =
        index >= 0 && index < count()
            ? widget(index)
            : nullptr;
    if (!pane) {
        return;
    }

    const int normalized =
        extent <= 0
            ? QWIDGETSIZE_MAX
            : extent;
    if (orientation() == Qt::Horizontal) {
        pane->setMaximumWidth(normalized);
    } else {
        pane->setMaximumHeight(normalized);
    }
}

int QtMaterialSplitView::paneMaximumExtent(
    int index) const
{
    QWidget* pane =
        index >= 0 && index < count()
            ? widget(index)
            : nullptr;
    if (!pane) {
        return QWIDGETSIZE_MAX;
    }

    return orientation() == Qt::Horizontal
        ? pane->maximumWidth()
        : pane->maximumHeight();
}

void QtMaterialSplitView::resetPaneSizes()
{
    finishCollapseAnimation();
    const int paneCount = count();
    if (paneCount <= 0) {
        return;
    }
    // Restore each pane's original collapsibility before expanding on reset.
    QList<int> collapsedIndexes;
    for (int i = 0; i < paneCount; ++i) {
        if (paneCollapsed(i)) { collapsedIndexes.push_back(i); }
        if (d_ptr->preCollapseCollapsible.contains(widget(i))) {
            setPaneCollapsible(i, d_ptr->preCollapseCollapsible.take(widget(i)));
        }
    }
    d_ptr->lastExpandedSize.clear();

    const int splitterExtent =
        orientation() == Qt::Horizontal
            ? width()
            : height();
    const int handlesExtent =
        qMax(0, paneCount - 1) * handleWidth();
    const int availableExtent =
        qMax(
            paneCount,
            splitterExtent - handlesExtent);
    const int equalExtent =
        qMax(1, availableExtent / paneCount);

    QList<int> equalSizes;
    equalSizes.reserve(paneCount);
    for (int index = 0; index < paneCount; ++index) {
        equalSizes.push_back(equalExtent);
    }
    setSizes(d_ptr->defaultPaneSizes.size() == paneCount ? d_ptr->defaultPaneSizes : equalSizes);
    persistPaneState();
    for (int index : collapsedIndexes) { Q_EMIT paneCollapsedChanged(index, paneCollapsed(index)); }
}

void QtMaterialSplitView::finishCollapseAnimation()
{
    d_ptr->animation->stop();
    const QPointer<QWidget> pane = d_ptr->animationPane;
    if (!pane) { d_ptr->constraintsRelaxed = false; return; }
    const int index = indexOf(pane);
    if (d_ptr->constraintsRelaxed) {
        pane->setSizePolicy(d_ptr->animationPolicy);
        if (d_ptr->animationOrientation == Qt::Horizontal) { pane->setMinimumWidth(d_ptr->animationMinimum); }
        else { pane->setMinimumHeight(d_ptr->animationMinimum); }
        d_ptr->constraintsRelaxed = false;
    }
    if (index >= 0 && d_ptr->animationTarget.size() == count()) { setSizes(d_ptr->animationTarget); }
    const bool collapsed = d_ptr->animationCollapsed;
    d_ptr->animationPane.clear();
    if (index < 0) { return; }
    if (!collapsed) {
        if (d_ptr->preCollapseCollapsible.contains(pane)) {
            setPaneCollapsible(index, d_ptr->preCollapseCollapsible.take(pane));
        }
        d_ptr->lastExpandedSize.remove(pane);
    }
    persistPaneState();
    Q_EMIT paneCollapsedChanged(index, paneCollapsed(index));
}

void QtMaterialSplitView::trackPane(QWidget* pane)
{
    if (d_ptr->trackedPanes.contains(pane)) { return; }
    d_ptr->trackedPanes.insert(pane);
    connect(pane, &QObject::destroyed, this, [this, pane]() {
        d_ptr->trackedPanes.remove(pane);
        d_ptr->explicitPanePolicies.remove(pane);
        d_ptr->lastExpandedSize.remove(pane);
        d_ptr->preCollapseCollapsible.remove(pane);
    });
}

QList<int> QtMaterialSplitView::defaultPaneSizes() const { return d_ptr->defaultPaneSizes; }
void QtMaterialSplitView::setDefaultPaneSizes(const QList<int>& sizes)
{
    d_ptr->defaultPaneSizes.clear();
    for (int size : sizes) { d_ptr->defaultPaneSizes.push_back(qMax(1, size)); }
}
int QtMaterialSplitView::keyboardResizeStep() const noexcept { return d_ptr->keyboardResizeStep; }
void QtMaterialSplitView::setKeyboardResizeStep(int step) { d_ptr->keyboardResizeStep = qBound(1, step, 1024); }
bool QtMaterialSplitView::animatedCollapseEnabled() const noexcept { return d_ptr->animatedCollapseEnabled; }
void QtMaterialSplitView::setAnimatedCollapseEnabled(bool enabled)
{
    if (!enabled) { finishCollapseAnimation(); }
    d_ptr->animatedCollapseEnabled = enabled;
}
int QtMaterialSplitView::collapseAnimationDuration() const noexcept { return d_ptr->collapseAnimationDuration; }
void QtMaterialSplitView::setCollapseAnimationDuration(int duration) { d_ptr->collapseAnimationDuration = qBound(0, duration, 10000); }

QByteArray QtMaterialSplitView::savePaneState() const
{
    QByteArray state;
    QDataStream stream(&state, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_12);
    stream << quint32(0x514d5356) << quint32(1) << qint32(count()) << qint32(orientation()) << saveState();
    for (int i = 0; i < count(); ++i) {
        QWidget* pane = widget(i);
        stream << qint32(d_ptr->lastExpandedSize.value(pane, 0))
            << qint32(d_ptr->preCollapseCollapsible.contains(pane)
                ? int(d_ptr->preCollapseCollapsible.value(pane)) : -1);
    }
    return state;
}

bool QtMaterialSplitView::restorePaneState(const QByteArray& state)
{
    QDataStream stream(state);
    stream.setVersion(QDataStream::Qt_5_12);
    quint32 magic = 0, version = 0;
    qint32 paneCount = 0, savedOrientation = 0;
    QByteArray nativeState;
    stream >> magic >> version >> paneCount >> savedOrientation;
    if (magic != 0x514d5356 || version != 1 || paneCount != count()
        || savedOrientation != orientation() || stream.status() != QDataStream::Ok) { return false; }
    stream >> nativeState;
    QList<int> expanded;
    QList<int> policies;
    for (int i = 0; i < paneCount; ++i) {
        qint32 size = 0, policy = -1;
        stream >> size >> policy;
        if (size < 0 || size > QWIDGETSIZE_MAX || policy < -1 || policy > 1) { return false; }
        expanded.push_back(size);
        policies.push_back(policy);
    }
    if (stream.status() != QDataStream::Ok || !stream.atEnd()) { return false; }
    // Validate native bytes on a disposable splitter before changing this one.
    QSplitter probe(orientation());
    for (int i = 0; i < count(); ++i) { probe.addWidget(new QWidget); }
    if (!probe.restoreState(nativeState)) { return false; }
    finishCollapseAnimation();
    const QList<int> oldSizes = sizes();
    if (!restoreState(nativeState)) { return false; }
    d_ptr->lastExpandedSize.clear();
    d_ptr->preCollapseCollapsible.clear();
    for (int i = 0; i < count(); ++i) {
        trackPane(widget(i));
        if (expanded.at(i) > 0) { d_ptr->lastExpandedSize.insert(widget(i), expanded.at(i)); }
        if (policies.at(i) >= 0) { d_ptr->preCollapseCollapsible.insert(widget(i), policies.at(i) != 0); }
    }
    for (int i = 0; i < count(); ++i) {
        if ((oldSizes.value(i) == 0) != paneCollapsed(i)) { Q_EMIT paneCollapsedChanged(i, paneCollapsed(i)); }
    }
    return true;
}

bool QtMaterialSplitView::rememberPaneSizes() const noexcept { return d_ptr->rememberPaneSizes; }
void QtMaterialSplitView::setRememberPaneSizes(bool remember)
{
    d_ptr->rememberPaneSizes = remember;
    if (!remember) { d_ptr->rememberedState.clear(); }
}
void QtMaterialSplitView::persistPaneState()
{
    if (count() > 0 && !d_ptr->animationPane) {
        const QByteArray state = savePaneState();
        if (d_ptr->rememberPaneSizes) { d_ptr->rememberedState = state; }
        Q_EMIT paneStateChanged(state);
    }
}
void QtMaterialSplitView::restoreSavedPaneState()
{
    if (d_ptr->rememberPaneSizes && !d_ptr->rememberedState.isEmpty()) {
        restorePaneState(d_ptr->rememberedState);
    }
}
void QtMaterialSplitView::showEvent(QShowEvent* event)
{
    QSplitter::showEvent(event);
    restoreSavedPaneState();
}
void QtMaterialSplitView::hideEvent(QHideEvent* event)
{
    finishCollapseAnimation();
    persistPaneState();
    QSplitter::hideEvent(event);
}

} // namespace QtMaterial
