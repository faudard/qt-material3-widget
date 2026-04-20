#include "qtmaterialsurfacerenderhelper_p.h"

#include <QPainter>

#include "qtmaterial/effects/qtmaterialshadowrenderer.h"

namespace QtMaterial {

QtMaterialSurfaceRenderHelper::SurfaceFrame QtMaterialSurfaceRenderHelper::baseFrame(
    const Theme& theme,
    ShapeRole shapeRole,
    ElevationRole elevationRole,
    const QRectF& rect,
    const QColor& containerColor,
    const QColor& outlineColor,
    bool drawOutline)
{
    SurfaceFrame frame;
    frame.panelRect = rect;
    frame.radius = theme.shapes().radius(shapeRole);
    frame.containerColor = containerColor;
    frame.outlineColor = outlineColor;
    frame.shadowColor = theme.colorScheme().color(ColorRole::Shadow);
    const ElevationStyle elevation = theme.elevations().style(elevationRole);
    frame.shadowBlur = elevation.shadowBlur;
    frame.shadowYOffset = elevation.shadowYOffset;
    frame.drawOutline = drawOutline && outlineColor.isValid() && outlineColor.alpha() > 0;
    frame.drawShadow = elevation.shadowBlur > 0 || elevation.shadowYOffset > 0;
    return frame;
}

QtMaterialSurfaceRenderHelper::SurfaceFrame QtMaterialSurfaceRenderHelper::dialogFrame(const Theme& theme, const DialogSpec& spec, const QRect& bounds)
{
    return baseFrame(theme, spec.shapeRole, spec.elevationRole, bounds.adjusted(2, 2, -2, -2), spec.containerColor, QColor(), false);
}

QtMaterialSurfaceRenderHelper::SurfaceFrame QtMaterialSurfaceRenderHelper::drawerFrame(const Theme& theme, const NavigationDrawerSpec& spec, const QRect& bounds)
{
    return baseFrame(theme, spec.shapeRole, spec.elevationRole, bounds, spec.containerColor, QColor(), false);
}

QtMaterialSurfaceRenderHelper::SurfaceFrame QtMaterialSurfaceRenderHelper::bottomSheetFrame(const Theme& theme, const BottomSheetSpec& spec, const QRect& bounds)
{
    return baseFrame(theme, spec.shapeRole, spec.elevationRole, bounds, spec.containerColor, QColor(), false);
}

QtMaterialSurfaceRenderHelper::SurfaceFrame QtMaterialSurfaceRenderHelper::cardFrame(const Theme& theme, const CardSpec& spec, const QRect& bounds)
{
    return baseFrame(theme, spec.shapeRole, spec.elevationRole, bounds.adjusted(1, 1, -1, -1), spec.containerColor, spec.outlineColor, true);
}

QPainterPath QtMaterialSurfaceRenderHelper::roundedPath(const QRectF& rect, qreal radius)
{
    QPainterPath path;
    path.addRoundedRect(rect, radius, radius);
    return path;
}

void QtMaterialSurfaceRenderHelper::paintFrame(QPainter* painter, const SurfaceFrame& frame)
{
    if (!painter) {
        return;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    if (frame.drawShadow) {
        QtMaterialShadowRenderer::paintRoundedShadow(
            painter,
            frame.panelRect,
            frame.radius,
            frame.shadowColor,
            frame.shadowBlur,
            frame.shadowYOffset);
    }

    painter->setPen(frame.drawOutline ? QPen(frame.outlineColor, 1.0) : Qt::NoPen);
    painter->setBrush(frame.containerColor);
    painter->drawPath(roundedPath(frame.panelRect, frame.radius));
    painter->restore();
}

} // namespace QtMaterial
