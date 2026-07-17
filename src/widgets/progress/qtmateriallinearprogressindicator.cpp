#include "qtmaterial/widgets/progress/qtmateriallinearprogressindicator.h"
#include <memory>

#include <QEvent>
#include <QHideEvent>
#include <QPainter>
#include <QShowEvent>
#include <QSizePolicy>
#include <QStyle>
#include <QVariantAnimation>
#include <algorithm>
#include <cmath>
#include "qtmaterial/specs/qtmaterialprogressspecresolver.h"

namespace QtMaterial {
namespace {
qreal clampProgress(qreal value) {
    if (std::isnan(value)) {
        return 0.0;
    }
    return qBound(0.0, value, 1.0);
}

} // namespace

class QtMaterialLinearProgressIndicatorPrivate
{
public:
    ProgressIndicatorSpec spec;
    mutable ProgressIndicatorSpec resolvedSpec;
    mutable bool specDirty = true;
    QtMaterialAsyncState asyncState;
    qreal value = 0.0;
    qreal phase = 0.0;
    QtMaterialLinearProgressIndicator::Mode mode = QtMaterialLinearProgressIndicator::Mode::Determinate;
    bool invertedAppearance = false;
    QPointer<QVariantAnimation> animation;
};

QtMaterialLinearProgressIndicator::QtMaterialLinearProgressIndicator(QWidget* parent)
    : QtMaterialWidget(parent)
    , d(std::make_unique<QtMaterialLinearProgressIndicatorPrivate>())
{
    setMaterialComponent(QStringLiteral("LinearProgressIndicator"));
    syncAsyncStateFromProgress();
    initAnimation();
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

QtMaterialLinearProgressIndicator::QtMaterialLinearProgressIndicator(const ProgressIndicatorSpec& spec, QWidget* parent)
    : QtMaterialWidget(parent)
    , d(std::make_unique<QtMaterialLinearProgressIndicatorPrivate>())
{
    d->spec = spec;
    d->specDirty = true;
    setMaterialComponent(QStringLiteral("LinearProgressIndicator"));
    syncAsyncStateFromProgress();
    initAnimation();
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

QtMaterialLinearProgressIndicator::~QtMaterialLinearProgressIndicator() = default;

qreal QtMaterialLinearProgressIndicator::value() const noexcept { return d->value; }

void QtMaterialLinearProgressIndicator::setValue(qreal value) {
    const qreal normalized = clampProgress(value);
    if (qFuzzyCompare(d->value + 1.0, normalized + 1.0)) {
        return;
    }
    d->value = normalized;
    if (d->mode == Mode::Determinate) {
        d->asyncState.setProgress(d->value);
        syncMaterialStateFromAsyncState();
        emit asyncStateChanged();
    }
    emit valueChanged(d->value);
    update();
}

void QtMaterialLinearProgressIndicator::resetValue() { setValue(0.0); }

QtMaterialLinearProgressIndicator::Mode QtMaterialLinearProgressIndicator::mode() const noexcept { return d->mode; }

void QtMaterialLinearProgressIndicator::setMode(Mode mode) {
    if (d->mode == mode) {
        return;
    }
    d->mode = mode;
    syncAsyncStateFromProgress();
    updateAnimationState();
    emit modeChanged(d->mode);
    emit asyncStateChanged();
    update();
}

bool QtMaterialLinearProgressIndicator::isBusy() const noexcept { return d->asyncState.isBusy(); }

void QtMaterialLinearProgressIndicator::setBusy(bool busy) {
    if (d->asyncState.isBusy() == busy) {
        return;
    }
    d->asyncState.setBusy(busy);
    syncMaterialStateFromAsyncState();
    emit asyncStateChanged();
    update();
}

bool QtMaterialLinearProgressIndicator::isIndeterminate() const noexcept {
    return d->mode == Mode::Indeterminate;
}

void QtMaterialLinearProgressIndicator::setIndeterminate(bool indeterminate) {
    setMode(indeterminate ? Mode::Indeterminate : Mode::Determinate);
}

QString QtMaterialLinearProgressIndicator::statusText() const { return d->asyncState.statusText(); }

void QtMaterialLinearProgressIndicator::setStatusText(const QString& text) {
    if (d->asyncState.statusText() == text) {
        return;
    }
    d->asyncState.setStatusText(text);
    setAccessibleDescription(text);
    syncMaterialStateFromAsyncState();
    emit asyncStateChanged();
}

QtMaterialAsyncState QtMaterialLinearProgressIndicator::asyncState() const { return d->asyncState; }

void QtMaterialLinearProgressIndicator::setAsyncState(const QtMaterialAsyncState& state) {
    d->asyncState = state;
    d->mode = d->asyncState.isIndeterminate() ? Mode::Indeterminate : Mode::Determinate;
    if (d->asyncState.hasProgress()) {
        d->value = d->asyncState.progress();
    }
    setAccessibleDescription(d->asyncState.statusText());
    syncMaterialStateFromAsyncState();
    updateAnimationState();
    emit modeChanged(d->mode);
    emit valueChanged(d->value);
    emit asyncStateChanged();
    update();
}

bool QtMaterialLinearProgressIndicator::invertedAppearance() const noexcept { return d->invertedAppearance; }

void QtMaterialLinearProgressIndicator::setInvertedAppearance(bool inverted) {
    if (d->invertedAppearance == inverted) {
        return;
    }
    d->invertedAppearance = inverted;
    emit invertedAppearanceChanged(d->invertedAppearance);
    update();
}

QColor QtMaterialLinearProgressIndicator::activeColor() const { return d->spec.activeColor; }

void QtMaterialLinearProgressIndicator::setActiveColor(const QColor& color) {
    if (d->spec.activeColor == color) return;
    d->spec.activeColor = color;
    d->specDirty = true;
    emit specChanged();
    update();
}

void QtMaterialLinearProgressIndicator::resetActiveColor() { setActiveColor(QColor()); }

QColor QtMaterialLinearProgressIndicator::trackColor() const { return d->spec.trackColor; }

void QtMaterialLinearProgressIndicator::setTrackColor(const QColor& color) {
    if (d->spec.trackColor == color) return;
    d->spec.trackColor = color;
    d->specDirty = true;
    emit specChanged();
    update();
}

void QtMaterialLinearProgressIndicator::resetTrackColor() { setTrackColor(QColor()); }

int QtMaterialLinearProgressIndicator::trackGap() const noexcept { return d->spec.trackGap; }

void QtMaterialLinearProgressIndicator::setTrackGap(int gap) {
    gap = qMax(0, gap);
    if (d->spec.trackGap == gap) return;
    d->spec.trackGap = gap;
    d->specDirty = true;
    emit specChanged();
    update();
}

int QtMaterialLinearProgressIndicator::stopIndicatorSize() const noexcept { return d->spec.stopIndicatorSize; }

void QtMaterialLinearProgressIndicator::setStopIndicatorSize(int size) {
    size = qMax(0, size);
    if (d->spec.stopIndicatorSize == size) return;
    d->spec.stopIndicatorSize = size;
    d->specDirty = true;
    emit specChanged();
    updateGeometry();
    update();
}

ProgressIndicatorSpec QtMaterialLinearProgressIndicator::spec() const { return d->spec; }

void QtMaterialLinearProgressIndicator::setSpec(const ProgressIndicatorSpec& spec) {
    d->spec = spec;
    d->specDirty = true;
    initAnimation();
    emit specChanged();
    updateGeometry();
    update();
}

QSize QtMaterialLinearProgressIndicator::sizeHint() const {
    const int h = qMax(1, d->spec.linearHeight) + d->spec.margins.top() + d->spec.margins.bottom();
    return QSize(240, h);
}

QSize QtMaterialLinearProgressIndicator::minimumSizeHint() const {
    const int h = qMax(1, d->spec.linearHeight) + d->spec.margins.top() + d->spec.margins.bottom();
    return QSize(64, h);
}

void QtMaterialLinearProgressIndicator::paintEvent(QPaintEvent*) {
    ensureSpecResolved();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QRectF r = rect().adjusted(d->resolvedSpec.margins.left(), d->resolvedSpec.margins.top(), -d->resolvedSpec.margins.right(), -d->resolvedSpec.margins.bottom());
    const qreal h = qMax(1.0, static_cast<double>(d->resolvedSpec.linearHeight));
    QRectF track(r.left(), r.center().y() - h / 2.0, r.width(), h);
    const qreal radius = d->resolvedSpec.cornerRadius < 0.0
        ? h / 2.0
        : qMin(d->resolvedSpec.cornerRadius, h / 2.0);
    painter.setPen(Qt::NoPen);
    painter.setBrush(d->resolvedSpec.trackColor);
    painter.drawRoundedRect(track, radius, radius);
    painter.setBrush(d->resolvedSpec.activeColor);
    if (d->mode == Mode::Determinate) {
        const qreal gap = d->value > 0.0 && d->value < 1.0 ? qMin<qreal>(d->resolvedSpec.trackGap, track.width()) : 0.0;
        const qreal w = qMax<qreal>(0.0, track.width() * d->value - gap);
        QRectF active = track;
        active.setWidth(w);
        if (d->invertedAppearance || layoutDirection() == Qt::RightToLeft) {
            active.moveRight(track.right());
        }
        if (active.width() > 0.0) {
            painter.drawRoundedRect(active, radius, radius);
        }
        if (d->resolvedSpec.stopIndicatorSize > 0 && d->value > 0.0 && d->value < 1.0) {
            painter.setBrush(d->resolvedSpec.stopIndicatorColor);
            const qreal s = qMin<qreal>(d->resolvedSpec.stopIndicatorSize, h);
            const qreal x = (d->invertedAppearance || layoutDirection() == Qt::RightToLeft)
                ? active.left() - gap / 2.0
                : active.right() + gap / 2.0;
            painter.drawEllipse(QPointF(x, track.center().y()), s / 2.0, s / 2.0);
        }
        return;
    }
    const qreal segmentWidth = track.width() * 0.32;
    const qreal travel = track.width() + segmentWidth;
    qreal x = track.left() - segmentWidth + travel * d->phase;
    if (d->invertedAppearance || layoutDirection() == Qt::RightToLeft) {
        x = track.right() - travel * d->phase;
    }
    QRectF active(x, track.top(), segmentWidth, track.height());
    active = active.intersected(track);
    if (!active.isEmpty()) {
        painter.drawRoundedRect(active, radius, radius);
    }
}

void QtMaterialLinearProgressIndicator::showEvent(QShowEvent* event) {
    QtMaterialWidget::showEvent(event);
    updateAnimationState();
}

void QtMaterialLinearProgressIndicator::hideEvent(QHideEvent* event) {
    QtMaterialWidget::hideEvent(event);
    updateAnimationState();
}

void QtMaterialLinearProgressIndicator::changeEvent(QEvent* event)
{
    QtMaterialWidget::changeEvent(event);
    if (event->type() == QEvent::StyleChange) {
        updateGeometry();
        update();
    }
}

void QtMaterialLinearProgressIndicator::initAnimation() {
    ensureSpecResolved();
    if (!d->animation) {
        d->animation = new QVariantAnimation(this);
        connect(d->animation, &QVariantAnimation::valueChanged, this, [this](const QVariant& value) {
            d->phase = value.toReal();
            update();
        });
    }
    d->animation->setStartValue(0.0);
    d->animation->setEndValue(1.0);
    d->animation->setDuration(qMax(250, d->resolvedSpec.animationDurationMs));
    d->animation->setLoopCount(-1);
}

void QtMaterialLinearProgressIndicator::updateAnimationState() {
    if (!d->animation) return;
    if (d->mode == Mode::Indeterminate && isVisible()) {
        if (d->animation->state() != QVariantAnimation::Running) d->animation->start();
    } else {
        if (d->animation->state() != QVariantAnimation::Stopped) d->animation->stop();
        d->phase = 0.0;
    }
}

void QtMaterialLinearProgressIndicator::syncAsyncStateFromProgress() {
    if (d->mode == Mode::Indeterminate) {
        d->asyncState.clearProgress();
        d->asyncState.setIndeterminate(true);
        d->asyncState.setBusy(true);
    } else {
        d->asyncState.setIndeterminate(false);
        d->asyncState.setProgress(d->value);
    }
    syncMaterialStateFromAsyncState();
}

void QtMaterialLinearProgressIndicator::syncMaterialStateFromAsyncState() {
    setMaterialState(d->asyncState.toPropertyString());
}

QString QtMaterial::QtMaterialLinearProgressIndicator::accessibleValueText() const
{
    const QString status = d->asyncState.statusText().trimmed();
    if (d->mode == Mode::Indeterminate) {
        return status.isEmpty() ? tr("In progress") : status;
    }

    const qreal clamped = std::isnan(d->value) ? 0.0 : qBound(0.0, d->value, 1.0);
    const QString progress = tr("%1%").arg(qRound(clamped * 100.0));
    return status.isEmpty() ? progress : tr("%1, %2").arg(status, progress);
}

void QtMaterial::QtMaterialLinearProgressIndicator::updateAccessibleState()
{
    if (accessibleName().trimmed().isEmpty()) {
        setAccessibleName(tr("Progress indicator"));
    }
    setAccessibleDescription(accessibleValueText());
}


void QtMaterialLinearProgressIndicator::ensureSpecResolved() const
{
    if (!d->specDirty) {
        return;
    }

    const ProgressSpecResolver resolver;
    d->resolvedSpec = resolver.resolve(theme(), d->spec);
    d->specDirty = false;
}

void QtMaterialLinearProgressIndicator::themeChangedEvent(const Theme& changedTheme)
{
    QtMaterialWidget::themeChangedEvent(changedTheme);
    d->specDirty = true;
    ensureSpecResolved();
    initAnimation();
    updateGeometry();
    update();
}

} // namespace QtMaterial
