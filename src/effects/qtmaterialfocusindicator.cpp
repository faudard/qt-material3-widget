#include "qtmaterial/effects/qtmaterialfocusindicator.h"

#include <QPainter>
#include <QtGlobal>

namespace QtMaterial {

bool QtMaterialFocusIndicator::shouldShow(
    const QtMaterialInteractionState& state,
    Qt::FocusReason reason,
    const InteractionStateTokens& policy) noexcept
{
    if (!state.isEnabled() || !state.isFocused()) {
        return false;
    }

    if (policy.strongFocusIndicators) {
        return true;
    }

    if (!policy.keyboardFocusVisible) {
        return false;
    }

    switch (reason) {
    case Qt::TabFocusReason:
    case Qt::BacktabFocusReason:
    case Qt::ShortcutFocusReason:
        return true;
    default:
        return false;
    }
}

void QtMaterialFocusIndicator::paintRectFocusRing(
    QPainter* painter,
    const QRectF& rect,
    const QColor& color,
    qreal radius,
    qreal width)
{
    if (!painter || width <= 0.0 || !color.isValid()) {
        return;
    }

    painter->save();
    QPen pen(color, width);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawRoundedRect(
        rect.adjusted(
            width / 2.0,
            width / 2.0,
            -width / 2.0,
            -width / 2.0),
        qMax<qreal>(0.0, radius),
        qMax<qreal>(0.0, radius));
    painter->restore();
}

void QtMaterialFocusIndicator::paintPathFocusRing(
    QPainter* painter,
    const QPainterPath& path,
    const QColor& color,
    qreal width)
{
    if (!painter || path.isEmpty() || width <= 0.0 || !color.isValid()) {
        return;
    }

    painter->save();
    QPen pen(color, width);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path);
    painter->restore();
}

void QtMaterialFocusIndicator::paintRectFocusRing(
    QPainter* painter,
    const QRectF& rect,
    qreal radius,
    const FocusRingTokens& tokens)
{
    QColor color = tokens.color;
    color.setAlphaF(
        qBound<qreal>(
            0.0,
            color.alphaF() * tokens.opacity,
            1.0));

    const qreal offset = qMax(0, tokens.offset);
    const QRectF ringRect =
        rect.adjusted(-offset, -offset, offset, offset);
    paintRectFocusRing(
        painter,
        ringRect,
        color,
        qMax<qreal>(0.0, radius + tokens.radiusAdjustment + offset),
        qMax(0, tokens.width));
}

void QtMaterialFocusIndicator::paintPathFocusRing(
    QPainter* painter,
    const QPainterPath& path,
    const FocusRingTokens& tokens)
{
    QColor color = tokens.color;
    color.setAlphaF(
        qBound<qreal>(
            0.0,
            color.alphaF() * tokens.opacity,
            1.0));
    paintPathFocusRing(
        painter,
        path,
        color,
        qMax(0, tokens.width));
}

} // namespace QtMaterial
