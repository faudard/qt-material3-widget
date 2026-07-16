#include "qtmaterial/effects/qtmaterialelevationrenderer.h"

#include <QPainter>
#include <QPainterPathStroker>
#include <QtGlobal>
#include <QtMath>

namespace QtMaterial {
namespace {

void paintShadowLayer(
    QPainter* painter,
    const QPainterPath& sourcePath,
    const QColor& shadowColor,
    qreal blurRadius,
    qreal yOffset,
    qreal opacity)
{
    if (!painter || sourcePath.isEmpty() || opacity <= 0.0) {
        return;
    }

    const int layerCount =
        qBound(1, qCeil(qMax<qreal>(1.0, blurRadius)), 12);
    const qreal safeBlur = qMax<qreal>(0.0, blurRadius);

    for (int layer = layerCount; layer >= 1; --layer) {
        const qreal fraction =
            static_cast<qreal>(layer) / static_cast<qreal>(layerCount);
        const qreal spread = safeBlur * 0.5 * fraction;

        QPainterPath layerPath = sourcePath;
        if (spread > 0.0) {
            QPainterPathStroker stroker;
            stroker.setCapStyle(Qt::RoundCap);
            stroker.setJoinStyle(Qt::RoundJoin);
            stroker.setWidth(spread * 2.0);
            layerPath = sourcePath.united(stroker.createStroke(sourcePath));
        }
        layerPath.translate(0.0, yOffset);

        QColor layerColor = shadowColor;
        const qreal normalizedAlpha =
            qBound<qreal>(0.0, shadowColor.alphaF(), 1.0);
        const qreal layerWeight =
            (1.0 - fraction * 0.72) / static_cast<qreal>(layerCount);
        layerColor.setAlphaF(
            qBound<qreal>(
                0.0,
                normalizedAlpha * opacity * layerWeight,
                1.0));

        painter->setBrush(layerColor);
        painter->drawPath(layerPath);
    }
}

} // namespace

ElevationStyle QtMaterialElevationRenderer::interpolate(
    const ElevationStyle& from,
    const ElevationStyle& to,
    qreal progress) noexcept
{
    const qreal t = qBound<qreal>(0.0, progress, 1.0);

    ElevationStyle style;
    style.shadowBlur =
        qRound(from.shadowBlur
               + (to.shadowBlur - from.shadowBlur) * t);
    style.shadowYOffset =
        qRound(from.shadowYOffset
               + (to.shadowYOffset - from.shadowYOffset) * t);
    style.tonalOverlayOpacity =
        from.tonalOverlayOpacity
        + (to.tonalOverlayOpacity - from.tonalOverlayOpacity) * t;
    return style;
}

void QtMaterialElevationRenderer::paintPathElevation(
    QPainter* painter,
    const QPainterPath& path,
    const QColor& shadowColor,
    const ElevationStyle& style)
{
    if (!painter
        || path.isEmpty()
        || !shadowColor.isValid()
        || (style.shadowBlur <= 0 && style.shadowYOffset <= 0)) {
        return;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);

    paintShadowLayer(
        painter,
        path,
        shadowColor,
        qMax(1, style.shadowBlur),
        qMax<qreal>(0.0, style.shadowYOffset * 0.35),
        0.16);

    paintShadowLayer(
        painter,
        path,
        shadowColor,
        qMax<qreal>(1.0, style.shadowBlur * 0.62),
        qMax(0, style.shadowYOffset),
        0.24);

    painter->restore();
}

void QtMaterialElevationRenderer::paintInterpolatedPathElevation(
    QPainter* painter,
    const QPainterPath& path,
    const QColor& shadowColor,
    const ElevationStyle& from,
    const ElevationStyle& to,
    qreal progress)
{
    paintPathElevation(
        painter,
        path,
        shadowColor,
        interpolate(from, to, progress));
}

} // namespace QtMaterial
