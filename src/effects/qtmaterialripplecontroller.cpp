#include "qtmaterial/effects/qtmaterialripplecontroller.h"

#include <QPainter>
#include <QEasingCurve>
#include <QWidget>
#include <QtGlobal>
#include <algorithm>
#include <cmath>

namespace QtMaterial {

namespace {
constexpr int kFrameIntervalMs = 16;
constexpr int kFallbackDurationMs = 280;
}

QtMaterialRippleController::QtMaterialRippleController(QWidget* target)
    : QObject(target)
    , m_target(target)
    , m_timer(new QTimer(this))
{
    m_timer->setInterval(kFrameIntervalMs);
    connect(m_timer, &QTimer::timeout, this, &QtMaterialRippleController::advance);
    m_clock.start();
}

QtMaterialRippleController::~QtMaterialRippleController() = default;

void QtMaterialRippleController::setClipPath(const QPainterPath& path)
{
    m_clipPath = path;
}

void QtMaterialRippleController::setClipRect(const QRectF& rect, qreal radius)
{
    QPainterPath path;
    path.addRoundedRect(rect, radius, radius);
    setClipPath(path);
}

void QtMaterialRippleController::setDuration(int durationMs)
{
    m_durationMs = qMax(1, durationMs);
}

int QtMaterialRippleController::duration() const noexcept
{
    return m_durationMs;
}

void QtMaterialRippleController::setBaseOpacity(qreal opacity) noexcept
{
    m_baseOpacity = qBound<qreal>(0.0, opacity, 1.0);
}

qreal QtMaterialRippleController::baseOpacity() const noexcept
{
    return m_baseOpacity;
}

void QtMaterialRippleController::setBoundsMode(BoundsMode mode) noexcept
{
    m_boundsMode = mode;
}

QtMaterialRippleController::BoundsMode
QtMaterialRippleController::boundsMode() const noexcept
{
    return m_boundsMode;
}

void QtMaterialRippleController::setOriginMode(OriginMode mode) noexcept
{
    m_originMode = mode;
}

QtMaterialRippleController::OriginMode
QtMaterialRippleController::originMode() const noexcept
{
    return m_originMode;
}

void QtMaterialRippleController::setEnabled(bool enabled)
{
    if (m_enabled == enabled) {
        return;
    }
    m_enabled = enabled;
    if (!m_enabled) {
        clear();
    }
}

bool QtMaterialRippleController::isEnabled() const noexcept
{
    return m_enabled;
}

void QtMaterialRippleController::setReducedMotion(bool reducedMotion)
{
    if (m_reducedMotion == reducedMotion) {
        return;
    }
    m_reducedMotion = reducedMotion;
    if (m_reducedMotion) {
        clear();
    }
}

bool QtMaterialRippleController::reducedMotion() const noexcept
{
    return m_reducedMotion;
}

void QtMaterialRippleController::addRipple(const QPointF& pointerPosition)
{
    if (!m_enabled || m_reducedMotion || !m_target || !m_target->isEnabled()) {
        return;
    }

    if (!m_clock.isValid()) {
        m_clock.start();
    }

    Ripple ripple;
    ripple.center = resolvedCenterFor(pointerPosition);
    ripple.endRadius = targetRadiusFor(ripple.center);
    ripple.startedMs = m_clock.elapsed();

    m_ripples.push_back(ripple);

    if (m_timer && !m_timer->isActive()) {
        m_timer->start();
    }

    m_target->update();
}

void QtMaterialRippleController::clear()
{
    m_ripples.clear();

    if (m_timer) {
        m_timer->stop();
    }

    if (m_target) {
        m_target->update();
    }
}

bool QtMaterialRippleController::isActive() const noexcept
{
    return !m_ripples.isEmpty();
}

void QtMaterialRippleController::paint(QPainter* painter, const QColor& color)
{
    if (!painter || m_ripples.isEmpty()) {
        return;
    }

    const qint64 now = m_clock.elapsed();
    const int duration = qMax(1, m_durationMs > 0 ? m_durationMs : kFallbackDurationMs);
    const QEasingCurve radiusCurve(QEasingCurve::OutCubic);
    const QEasingCurve opacityCurve(QEasingCurve::OutQuad);

    painter->save();

    if (m_boundsMode == BoundsMode::Bounded && !m_clipPath.isEmpty()) {
        painter->setClipPath(m_clipPath);
    }

    painter->setPen(Qt::NoPen);

    for (const Ripple& ripple : m_ripples) {
        const qreal rawProgress = ripple.fixedProgress >= 0.0
            ? ripple.fixedProgress
            : qreal(now - ripple.startedMs) / qreal(duration);
        const qreal t = qBound<qreal>(0.0, rawProgress, 1.0);

        const qreal radius = ripple.endRadius * radiusCurve.valueForProgress(t);
        const qreal opacity = m_baseOpacity * (1.0 - opacityCurve.valueForProgress(t));

        QColor brush = color;
        brush.setAlphaF(qBound<qreal>(0.0, color.alphaF() * opacity, 1.0));

        painter->setBrush(brush);
        painter->drawEllipse(ripple.center, radius, radius);
    }

    painter->restore();
}

void QtMaterialRippleController::advance()
{
    if (m_ripples.isEmpty()) {
        if (m_timer) {
            m_timer->stop();
        }
        return;
    }

    const qint64 now = m_clock.elapsed();
    const int duration = qMax(1, m_durationMs > 0 ? m_durationMs : kFallbackDurationMs);

    for (int i = m_ripples.size() - 1; i >= 0; --i) {
        if (m_ripples.at(i).fixedProgress >= 0.0) {
            continue;
        }
        const qint64 elapsed = now - m_ripples.at(i).startedMs;
        if (elapsed >= duration) {
            m_ripples.removeAt(i);
        }
    }

    const bool hasAnimatedRipple = std::any_of(
        m_ripples.cbegin(),
        m_ripples.cend(),
        [](const Ripple& ripple) { return ripple.fixedProgress < 0.0; });
    if (!hasAnimatedRipple && m_timer) {
        m_timer->stop();
    }

    if (m_target) {
        m_target->update();
    }
}

QPointF QtMaterialRippleController::resolvedCenterFor(
    const QPointF& pointerPosition) const
{
    if (m_originMode == OriginMode::Pointer) {
        return pointerPosition;
    }

    if (!m_clipPath.isEmpty()) {
        return m_clipPath.boundingRect().center();
    }

    if (m_target) {
        return QRectF(QPointF(0.0, 0.0), QSizeF(m_target->size())).center();
    }

    return pointerPosition;
}

qreal QtMaterialRippleController::targetRadiusFor(const QPointF& center) const
{
    QRectF bounds;

    if (!m_clipPath.isEmpty()) {
        bounds = m_clipPath.boundingRect();
    } else if (m_target) {
        bounds = QRectF(QPointF(0.0, 0.0), QSizeF(m_target->size()));
    } else {
        return 24.0;
    }

    const qreal dx = qMax(qAbs(center.x() - bounds.left()), qAbs(center.x() - bounds.right()));
    const qreal dy = qMax(qAbs(center.y() - bounds.top()), qAbs(center.y() - bounds.bottom()));

    return std::sqrt(dx * dx + dy * dy);
}

} // namespace QtMaterial
