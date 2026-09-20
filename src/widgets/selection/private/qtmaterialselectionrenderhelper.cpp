#include "qtmaterialselectionrenderhelper_p.h"

#include <QFontMetrics>
#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"

namespace QtMaterial::SelectionRenderHelper {


qreal stateLayerOpacity(
    const SelectionRuntimeSpec& spec,
    const QtMaterialInteractionState& state,
    const InteractionStateTokens& policy)
{
    StateLayer layer;
    layer.hoverOpacity = spec.hoverStateLayerOpacity;
    layer.focusOpacity = spec.focusStateLayerOpacity;
    layer.pressOpacity = spec.pressStateLayerOpacity;
    layer.dragOpacity = spec.dragStateLayerOpacity;
    return QtMaterialStateLayerPainter::opacityForState(
        state,
        layer,
        policy);
}

QFont resolvedLabelFont(
    const QFont& fallback,
    const SelectionRuntimeSpec& spec)
{
    return spec.hasResolvedLabelFont ? spec.labelFont : fallback;
}

void configureMotion(
    const SelectionRuntimeSpec& spec,
    QtMaterialTransitionController* transition,
    QtMaterialRippleController* ripple,
    bool reducedMotion)
{
    if (spec.hasResolvedMotionStyle) {
        if (transition) {
            transition->applyMotionStyle(spec.motionStyle);
            transition->setReducedMotion(reducedMotion);
        }
        if (ripple && spec.motionStyle.durationMs > 0) {
            ripple->setDuration(spec.motionStyle.durationMs);
        }
    }

    if (ripple) {
        ripple->setBaseOpacity(spec.pressStateLayerOpacity);
        ripple->setReducedMotion(reducedMotion);
    }
}



QRectF centeredStateLayerRect(const QRect& bounds, int stateLayerSize)
{
    return QRectF(bounds.center().x() - stateLayerSize / 2.0,
                  bounds.center().y() - stateLayerSize / 2.0,
                  stateLayerSize,
                  stateLayerSize);
}

void paintCircularStateLayer(
    QPainter* painter,
    const QRectF& rect,
    const QColor& color,
    qreal opacity)
{
    QPainterPath path;
    path.addEllipse(rect);
    QtMaterialStateLayerPainter::paintPath(
        painter,
        path,
        color,
        opacity);
}

void paintLabel(QPainter* painter, const QRect& rect, Qt::Alignment alignment, const QString& text, const QColor& color, const QFont& font)
{
    if (!painter || text.isEmpty()) {
        return;
    }
    painter->save();
    painter->setFont(font);
    painter->setPen(color);
    const QFontMetrics metrics(font);
    const QString elided = metrics.elidedText(text, Qt::ElideRight, rect.width());
    painter->drawText(rect, alignment, elided);
    painter->restore();
}

QRectF switchTrackRect(const QRect& widgetRect, const SwitchSpec& spec)
{
    return QRectF((widgetRect.width() - spec.trackWidth) / 2.0,
                  (widgetRect.height() - spec.trackHeight) / 2.0,
                  spec.trackWidth,
                  spec.trackHeight);
}

QRectF switchHandleRect(const QRectF& trackRect, const SwitchSpec& spec, qreal progress)
{
    const qreal diameter = spec.handleDiameter + (spec.selectedHandleDiameter - spec.handleDiameter) * progress;
    const qreal inset = 4.0;
    const qreal startX = trackRect.left() + inset;
    const qreal endX = trackRect.right() - diameter - inset;
    const qreal x = startX + (endX - startX) * progress;
    const qreal y = trackRect.center().y() - diameter / 2.0;
    return QRectF(x, y, diameter, diameter);
}

QPainterPath roundedTrackPath(const QRectF& trackRect)
{
    QPainterPath path;
    path.addRoundedRect(trackRect, trackRect.height() / 2.0, trackRect.height() / 2.0);
    return path;
}

} // namespace QtMaterial::SelectionRenderHelper
