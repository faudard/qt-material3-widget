#include "qtmaterial/effects/qtmaterialshadowrenderer.h"
#include <QPainter>
namespace QtMaterial {
void QtMaterialShadowRenderer::paintRoundedShadow(QPainter* painter, const QRectF& rect, qreal radius, const QColor& shadowColor, int blurRadius, int yOffset)
{
    if (!painter) return;
    painter->save();
    QColor fill = shadowColor;
    fill.setAlpha(qMin(255, 32 + blurRadius * 4));
    painter->setPen(Qt::NoPen);
    painter->setBrush(fill);
    painter->drawRoundedRect(rect.adjusted(0, yOffset, 0, yOffset), radius, radius);
    painter->restore();
}
} // namespace QtMaterial
