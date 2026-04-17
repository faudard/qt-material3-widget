#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include <QPainter>
namespace QtMaterial {
void QtMaterialStateLayerPainter::paintRect(QPainter* painter, const QRectF& rect, const QColor& color, qreal opacity, qreal radius)
{
    if (!painter) return;
    painter->save();
    QColor fill = color;
    fill.setAlphaF(opacity);
    painter->setPen(Qt::NoPen);
    painter->setBrush(fill);
    painter->drawRoundedRect(rect, radius, radius);
    painter->restore();
}
void QtMaterialStateLayerPainter::paintPath(QPainter* painter, const QPainterPath& path, const QColor& color, qreal opacity)
{
    if (!painter) return;
    painter->save();
    QColor fill = color;
    fill.setAlphaF(opacity);
    painter->setPen(Qt::NoPen);
    painter->setBrush(fill);
    painter->drawPath(path);
    painter->restore();
}
} // namespace QtMaterial
