#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"

#include <QPainter>
#include <QtGlobal>

namespace QtMaterial {

qreal QtMaterialStateLayerPainter::opacityForState(
    const QtMaterialInteractionState& state,
    const StateLayer& layer,
    const InteractionStateTokens& policy) noexcept
{
    if (!state.isEnabled()) {
        return 0.0;
    }
    if (state.isDragged() && policy.dragFeedbackEnabled) {
        return qBound<qreal>(0.0, layer.dragOpacity, 1.0);
    }
    if (state.isPressed() && policy.pressFeedbackEnabled) {
        return qBound<qreal>(0.0, layer.pressOpacity, 1.0);
    }
    if (state.isFocused()) {
        return qBound<qreal>(0.0, layer.focusOpacity, 1.0);
    }
    if (state.isHovered() && policy.hoverFeedbackEnabled) {
        return qBound<qreal>(0.0, layer.hoverOpacity, 1.0);
    }
    return 0.0;
}

void QtMaterialStateLayerPainter::paintRect(
    QPainter* painter,
    const QRectF& rect,
    const QColor& color,
    qreal opacity,
    qreal radius)
{
    if (!painter || opacity <= 0.0) {
        return;
    }
    painter->save();
    QColor fill = color;
    fill.setAlphaF(qBound<qreal>(0.0, color.alphaF() * opacity, 1.0));
    painter->setPen(Qt::NoPen);
    painter->setBrush(fill);
    painter->drawRoundedRect(rect, radius, radius);
    painter->restore();
}

void QtMaterialStateLayerPainter::paintPath(
    QPainter* painter,
    const QPainterPath& path,
    const QColor& color,
    qreal opacity)
{
    if (!painter || path.isEmpty() || opacity <= 0.0) {
        return;
    }
    painter->save();
    QColor fill = color;
    fill.setAlphaF(qBound<qreal>(0.0, color.alphaF() * opacity, 1.0));
    painter->setPen(Qt::NoPen);
    painter->setBrush(fill);
    painter->drawPath(path);
    painter->restore();
}

void QtMaterialStateLayerPainter::paintRectForState(
    QPainter* painter,
    const QRectF& rect,
    qreal radius,
    const QtMaterialInteractionState& state,
    const StateLayer& layer,
    const InteractionStateTokens& policy)
{
    paintRect(
        painter,
        rect,
        layer.color,
        opacityForState(state, layer, policy),
        radius);
}

void QtMaterialStateLayerPainter::paintPathForState(
    QPainter* painter,
    const QPainterPath& path,
    const QtMaterialInteractionState& state,
    const StateLayer& layer,
    const InteractionStateTokens& policy)
{
    paintPath(
        painter,
        path,
        layer.color,
        opacityForState(state, layer, policy));
}

} // namespace QtMaterial
