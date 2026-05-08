#include "qtmaterial/widgets/progress/qtmaterialcircularprogressindicator.h"
#include <memory>

#include <QEvent>
#include <QPainter>
#include <QPalette>
#include <QPen>
#include <QVariantAnimation>
#include <QtGlobal>

#include <cmath>

namespace QtMaterial {
namespace {

qreal clampProgress(qreal value)
{
    if (std::isnan(value)) {
        return 0.0;
    }
    return qBound(0.0, value, 1.0);
}

QColor fallbackActive(const QWidget* widget)
{
    return widget->palette().color(QPalette::Highlight);
}

QColor fallbackTrack(const QWidget* widget)
{
    QColor color = widget->palette().color(QPalette::Mid);
    color.setAlpha(qMax(32, color.alpha() / 2));
    return color;
}


QColor resolvedCircularActiveColor(const ProgressIndicatorSpec& spec, const QWidget* widget)
{
    return spec.activeColor.isValid() ? spec.activeColor : fallbackActive(widget);
}

QColor resolvedCircularTrackColor(const ProgressIndicatorSpec& spec, const QWidget* widget)
{
    return spec.trackColor.isValid() ? spec.trackColor : fallbackTrack(widget);
}

} // namespace

class QtMaterialCircularProgressIndicatorPrivate
{
public:
    ProgressIndicatorSpec spec;
    QtMaterialAsyncState asyncState;
    qreal value = 0.0;
    qreal phase = 0.0;
    QtMaterialCircularProgressIndicator::Mode mode = QtMaterialCircularProgressIndicator::Mode::Determinate;
    QPointer<QVariantAnimation> animation;
};

QtMaterialCircularProgressIndicator::QtMaterialCircularProgressIndicator(QWidget* parent)
    : QtMaterialWidget(parent)
    , d(std::make_unique<QtMaterialCircularProgressIndicatorPrivate>())
{
    init();
}

QtMaterialCircularProgressIndicator::QtMaterialCircularProgressIndicator(const ProgressIndicatorSpec& spec, QWidget* parent)
    : QtMaterialWidget(parent)
    , d(std::make_unique<QtMaterialCircularProgressIndicatorPrivate>())
{
    d->spec = spec;
    init();
}

QtMaterialCircularProgressIndicator::~QtMaterialCircularProgressIndicator() = default;

void QtMaterialCircularProgressIndicator::init()
{
    setMaterialComponent(QStringLiteral("CircularProgressIndicator"));
    setAccessibleName(QStringLiteral("Progress indicator"));
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    syncAsyncStateFromProgress();
    initAnimation();
    syncAccessibleState();
}

qreal QtMaterialCircularProgressIndicator::value() const noexcept
{
    return d->value;
}

void QtMaterialCircularProgressIndicator::setValue(qreal value)
{
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
    syncAccessibleState();
    update();
}

void QtMaterialCircularProgressIndicator::resetValue()
{
    setValue(0.0);
}

QtMaterialCircularProgressIndicator::Mode QtMaterialCircularProgressIndicator::mode() const noexcept
{
    return d->mode;
}

void QtMaterialCircularProgressIndicator::setMode(Mode mode)
{
    if (d->mode == mode) {
        return;
    }

    d->mode = mode;
    syncAsyncStateFromProgress();
    updateAnimationState();
    emit modeChanged(d->mode);
    emit asyncStateChanged();
    syncAccessibleState();
    update();
}

bool QtMaterialCircularProgressIndicator::isBusy() const noexcept
{
    return d->asyncState.isBusy();
}

void QtMaterialCircularProgressIndicator::setBusy(bool busy)
{
    if (d->asyncState.isBusy() == busy) {
        return;
    }

    d->asyncState.setBusy(busy);
    syncMaterialStateFromAsyncState();
    emit asyncStateChanged();
    syncAccessibleState();
    update();
}

bool QtMaterialCircularProgressIndicator::isIndeterminate() const noexcept
{
    return d->mode == Mode::Indeterminate;
}

void QtMaterialCircularProgressIndicator::setIndeterminate(bool indeterminate)
{
    setMode(indeterminate ? Mode::Indeterminate : Mode::Determinate);
}

QString QtMaterialCircularProgressIndicator::statusText() const
{
    return d->asyncState.statusText();
}

void QtMaterialCircularProgressIndicator::setStatusText(const QString& text)
{
    if (d->asyncState.statusText() == text) {
        return;
    }

    d->asyncState.setStatusText(text);
    syncMaterialStateFromAsyncState();
    emit asyncStateChanged();
    syncAccessibleState();
}

QString QtMaterialCircularProgressIndicator::accessibleValueText() const
{
    const QString status = statusText().trimmed();
    if (d->mode == Mode::Indeterminate) {
        return status.isEmpty() ? QStringLiteral("In progress") : status;
    }

    const int percent = qBound(0, qRound(d->value * 100.0), 100);
    const QString progress = QStringLiteral("%1%").arg(percent);
    return status.isEmpty() ? progress : QStringLiteral("%1, %2").arg(status, progress);
}

QString QtMaterialCircularProgressIndicator::accessibilitySummary() const
{
    return accessibleValueText();
}

QtMaterialAsyncState QtMaterialCircularProgressIndicator::asyncState() const
{
    return d->asyncState;
}

void QtMaterialCircularProgressIndicator::setAsyncState(const QtMaterialAsyncState& state)
{
    d->asyncState = state;
    d->mode = d->asyncState.isIndeterminate() ? Mode::Indeterminate : Mode::Determinate;
    if (d->asyncState.hasProgress()) {
        d->value = clampProgress(d->asyncState.progress());
        d->asyncState.setProgress(d->value);
    }

    syncMaterialStateFromAsyncState();
    updateAnimationState();
    emit modeChanged(d->mode);
    emit valueChanged(d->value);
    emit asyncStateChanged();
    syncAccessibleState();
    update();
}

QColor QtMaterialCircularProgressIndicator::activeColor() const
{
    return d->spec.activeColor;
}

void QtMaterialCircularProgressIndicator::setActiveColor(const QColor& color)
{
    if (d->spec.activeColor == color) {
        return;
    }
    d->spec.activeColor = color;
    emit specChanged();
    update();
}

void QtMaterialCircularProgressIndicator::resetActiveColor()
{
    setActiveColor(QColor());
}

QColor QtMaterialCircularProgressIndicator::trackColor() const
{
    return d->spec.trackColor;
}

void QtMaterialCircularProgressIndicator::setTrackColor(const QColor& color)
{
    if (d->spec.trackColor == color) {
        return;
    }
    d->spec.trackColor = color;
    emit specChanged();
    update();
}

void QtMaterialCircularProgressIndicator::resetTrackColor()
{
    setTrackColor(QColor());
}

int QtMaterialCircularProgressIndicator::trackGap() const noexcept
{
    return d->spec.trackGap;
}

void QtMaterialCircularProgressIndicator::setTrackGap(int gap)
{
    gap = qMax(0, gap);
    if (d->spec.trackGap == gap) {
        return;
    }
    d->spec.trackGap = gap;
    emit specChanged();
    update();
}

int QtMaterialCircularProgressIndicator::strokeWidth() const noexcept
{
    return d->spec.circularStrokeWidth;
}

void QtMaterialCircularProgressIndicator::setStrokeWidth(int width)
{
    width = qMax(1, width);
    if (d->spec.circularStrokeWidth == width) {
        return;
    }
    d->spec.circularStrokeWidth = width;
    emit specChanged();
    updateGeometry();
    update();
}

ProgressIndicatorSpec QtMaterialCircularProgressIndicator::spec() const
{
    return d->spec;
}

void QtMaterialCircularProgressIndicator::setSpec(const ProgressIndicatorSpec& spec)
{
    d->spec = spec;
    initAnimation();
    emit specChanged();
    updateGeometry();
    update();
}

QSize QtMaterialCircularProgressIndicator::sizeHint() const
{
    return d->spec.circularSize.expandedTo(QSize(16, 16));
}

QSize QtMaterialCircularProgressIndicator::minimumSizeHint() const
{
    return QSize(24, 24);
}

void QtMaterialCircularProgressIndicator::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const int stroke = qMax(1, d->spec.circularStrokeWidth);
    QRectF arcRect = rect().adjusted(stroke, stroke, -stroke, -stroke);
    if (arcRect.isEmpty()) {
        return;
    }

    QPen trackPen(resolvedCircularTrackColor(d->spec, this), stroke, Qt::SolidLine, Qt::RoundCap);
    QPen activePen(resolvedCircularActiveColor(d->spec, this), stroke, Qt::SolidLine, Qt::RoundCap);

    painter.setPen(trackPen);
    const int gap16 = qMax(0, d->spec.trackGap) * 16;
    painter.drawArc(arcRect, gap16, 360 * 16 - (2 * gap16));

    painter.setPen(activePen);
    if (d->mode == Mode::Determinate) {
        const int span = qRound(360.0 * 16.0 * d->value) - (d->value > 0.0 && d->value < 1.0 ? gap16 : 0);
        if (span > 0) {
            painter.drawArc(arcRect, 90 * 16, -span);
        }
        return;
    }

    const int start = qRound((90.0 - 360.0 * d->phase) * 16.0);
    const int span = -qRound((72.0 + 108.0 * (0.5 + 0.5 * std::sin(d->phase * 6.283185307179586))) * 16.0);
    painter.drawArc(arcRect, start, span);
}

void QtMaterialCircularProgressIndicator::showEvent(QShowEvent* event)
{
    QtMaterialWidget::showEvent(event);
    updateAnimationState();
}

void QtMaterialCircularProgressIndicator::hideEvent(QHideEvent* event)
{
    QtMaterialWidget::hideEvent(event);
    updateAnimationState();
}

void QtMaterialCircularProgressIndicator::changeEvent(QEvent* event)
{
    QtMaterialWidget::changeEvent(event);
    if (event->type() == QEvent::PaletteChange || event->type() == QEvent::StyleChange) {
        update();
    }
}

void QtMaterialCircularProgressIndicator::initAnimation()
{
    if (!d->animation) {
        d->animation = new QVariantAnimation(this);
        connect(d->animation, &QVariantAnimation::valueChanged, this, [this](const QVariant& value) {
            d->phase = value.toReal();
            update();
        });
    }

    d->animation->setStartValue(0.0);
    d->animation->setEndValue(1.0);
    d->animation->setDuration(qMax(250, d->spec.animationDurationMs));
    d->animation->setLoopCount(-1);
}

void QtMaterialCircularProgressIndicator::updateAnimationState()
{
    if (!d->animation) {
        return;
    }

    if (d->mode == Mode::Indeterminate && isVisible()) {
        if (d->animation->state() != QVariantAnimation::Running) {
            d->animation->start();
        }
    } else {
        if (d->animation->state() != QVariantAnimation::Stopped) {
            d->animation->stop();
        }
        d->phase = 0.0;
    }
}

void QtMaterialCircularProgressIndicator::syncAsyncStateFromProgress()
{
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

void QtMaterialCircularProgressIndicator::syncMaterialStateFromAsyncState()
{
    setMaterialState(d->asyncState.toPropertyString());
}

void QtMaterialCircularProgressIndicator::syncAccessibleState()
{
    if (accessibleName().trimmed().isEmpty()) {
        setAccessibleName(QStringLiteral("Progress indicator"));
    }

    const QString summary = accessibilitySummary();
    if (accessibleDescription() == summary) {
        return;
    }

    setAccessibleDescription(summary);
    emit accessibilitySummaryChanged(summary);
}

QColor QtMaterialCircularProgressIndicator::resolvedCircularActiveColor(d->spec, this) const
{
    return d->spec.activeColor.isValid() ? d->spec.activeColor : fallbackActive(this);
}

QColor QtMaterialCircularProgressIndicator::resolvedCircularTrackColor(d->spec, this) const
{
    return d->spec.trackColor.isValid() ? d->spec.trackColor : fallbackTrack(this);
}

} // namespace QtMaterial
