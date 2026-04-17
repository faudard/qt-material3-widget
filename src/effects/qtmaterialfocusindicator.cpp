#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include <QPainter>
namespace QtMaterial {
void QtMaterialFocusIndicator::paintRectFocusRing(QPainter* painter, const QRectF& rect, const QColor& color, qreal radius, qreal width)
{
    if (!painter) return;
    painter->save();
    QPen pen(color, width);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawRoundedRect(rect.adjusted(width / 2.0, width / 2.0, -width / 2.0, -width / 2.0), radius, radius);
    painter->restore();
}
void QtMaterialFocusIndicator::paintPathFocusRing(QPainter* painter, const QPainterPath& path, const QColor& color, qreal width)
{
    if (!painter) return;
    painter->save();
    QPen pen(color, width);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path);
    painter->restore();
}
} // namespace QtMaterial
