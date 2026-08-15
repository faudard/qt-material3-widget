#include "qtmaterialsurfacerenderhelper_p.h"

#include <QPainter>

#include "qtmaterial/effects/qtmaterialshadowrenderer.h"

namespace QtMaterial {

QtMaterialSurfaceRenderHelper::SurfaceFrame
QtMaterialSurfaceRenderHelper::baseFrame(
    qreal radius,
    const ElevationStyle& elevationStyle,
    bool hasResolvedElevationStyle,
    const QColor& shadowColor,
    const QRectF& rect,
    const QColor& containerColor,
    const QColor& outlineColor,
    bool drawOutline)
{
    SurfaceFrame frame;
    frame.panelRect = rect;
    frame.radius = radius < 0.0
        ? qMin(rect.width(), rect.height()) / 2.0
        : qMax(0.0, radius);
    frame.containerColor = containerColor;
    frame.outlineColor = outlineColor;
    frame.shadowColor = shadowColor;

    if (hasResolvedElevationStyle) {
        frame.shadowBlur = elevationStyle.shadowBlur;
        frame.shadowYOffset = elevationStyle.shadowYOffset;
    }

    frame.drawOutline =
        drawOutline && outlineColor.isValid() && outlineColor.alpha() > 0;
    frame.drawShadow =
        hasResolvedElevationStyle
        && (frame.shadowBlur > 0 || frame.shadowYOffset > 0)
        && shadowColor.isValid()
        && shadowColor.alpha() > 0;
    return frame;
}

QtMaterialSurfaceRenderHelper::SurfaceFrame
QtMaterialSurfaceRenderHelper::dialogFrame(const DialogSpec& spec, const QRect& bounds)
{
    return baseFrame(
        spec.cornerRadius,
        spec.elevationStyle,
        spec.hasResolvedElevationStyle,
        spec.shadowColor,
        bounds.adjusted(2, 2, -2, -2),
        spec.containerColor,
        QColor(),
        false);
}

QtMaterialSurfaceRenderHelper::SurfaceFrame
QtMaterialSurfaceRenderHelper::drawerFrame(
    const NavigationDrawerSpec& spec,
    const QRect& bounds)
{
    return baseFrame(
        spec.cornerRadius,
        spec.elevationStyle,
        spec.hasResolvedElevationStyle,
        spec.shadowColor,
        bounds,
        spec.containerColor,
        QColor(),
        false);
}

QtMaterialSurfaceRenderHelper::SurfaceFrame
QtMaterialSurfaceRenderHelper::bottomSheetFrame(
    const BottomSheetSpec& spec,
    const QRect& bounds)
{
    return baseFrame(
        spec.cornerRadius,
        spec.elevationStyle,
        spec.hasResolvedElevationStyle,
        spec.shadowColor,
        bounds,
        spec.containerColor,
        QColor(),
        false);
}

QtMaterialSurfaceRenderHelper::SurfaceFrame
QtMaterialSurfaceRenderHelper::cardFrame(const CardSpec& spec, const QRect& bounds)
{
    return baseFrame(
        spec.cornerRadius,
        spec.elevationStyle,
        spec.hasResolvedElevationStyle,
        spec.shadowColor,
        bounds.adjusted(1, 1, -1, -1),
        spec.containerColor,
        spec.outlineColor,
        true);
}

QPainterPath QtMaterialSurfaceRenderHelper::roundedPath(
    const QRectF& rect,
    qreal radius)
{
    QPainterPath path;
    path.addRoundedRect(rect, radius, radius);
    return path;
}

void QtMaterialSurfaceRenderHelper::paintFrame(
    QPainter* painter,
    const SurfaceFrame& frame)
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
    painter->setPen(
        frame.drawOutline ? QPen(frame.outlineColor, 1.0) : Qt::NoPen);
    painter->setBrush(frame.containerColor);
    painter->drawPath(roundedPath(frame.panelRect, frame.radius));
    painter->restore();
}

} // namespace QtMaterial
