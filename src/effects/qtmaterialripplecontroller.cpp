#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include <QPainter>
#include <QWidget>
namespace QtMaterial {
QtMaterialRippleController::QtMaterialRippleController(QWidget* target) : QObject(target), m_target(target) {}
QtMaterialRippleController::~QtMaterialRippleController() = default;
void QtMaterialRippleController::setClipPath(const QPainterPath& path) { m_clipPath = path; }
void QtMaterialRippleController::setClipRect(const QRectF& rect, qreal radius)
{
    QPainterPath path;
    path.addRoundedRect(rect, radius, radius);
    setClipPath(path);
}
void QtMaterialRippleController::addRipple(const QPointF& center)
{
    Ripple ripple;
    ripple.center = center;
    ripple.radius = 24.0;
    ripple.opacity = 0.18;
    m_ripples.push_back(ripple);
    if (m_target) m_target->update();
}
void QtMaterialRippleController::clear()
{
    m_ripples.clear();
    if (m_target) m_target->update();
}
bool QtMaterialRippleController::isActive() const noexcept { return !m_ripples.isEmpty(); }
void QtMaterialRippleController::paint(QPainter* painter, const QColor& color)
{
    if (!painter || m_ripples.isEmpty()) return;
    painter->save();
    if (!m_clipPath.isEmpty()) {
        painter->setClipPath(m_clipPath);
    }
    painter->setPen(Qt::NoPen);
    for (const Ripple& ripple : m_ripples) {
        QColor brush = color;
        brush.setAlphaF(ripple.opacity);
        painter->setBrush(brush);
        painter->drawEllipse(ripple.center, ripple.radius, ripple.radius);
    }
    painter->restore();
}
} // namespace QtMaterial
