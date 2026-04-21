#include "qtmaterial/effects/qtmaterialripplecontroller.h"

#include <QPainter>
#include <QEasingCurve>
#include <QWidget>
#include <QtGlobal>
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

void QtMaterialRippleController::addRipple(const QPointF& center)
{
    if (!m_clock.isValid()) {
        m_clock.start();
    }

    Ripple ripple;
    ripple.center = center;
    ripple.endRadius = targetRadiusFor(center);
    ripple.startedMs = m_clock.elapsed();

    m_ripples.push_back(ripple);

    if (m_timer && !m_timer->isActive()) {
        m_timer->start();
    }

    if (m_target) {
        m_target->update();
    }
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

    if (!m_clipPath.isEmpty()) {
        painter->setClipPath(m_clipPath);
    }

    painter->setPen(Qt::NoPen);

    for (const Ripple& ripple : m_ripples) {
        const qreal rawProgress =
            qreal(now - ripple.startedMs) / qreal(duration);
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
        const qint64 elapsed = now - m_ripples.at(i).startedMs;
        if (elapsed >= duration) {
            m_ripples.removeAt(i);
        }
    }

    if (m_ripples.isEmpty() && m_timer) {
        m_timer->stop();
    }

    if (m_target) {
        m_target->update();
    }
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