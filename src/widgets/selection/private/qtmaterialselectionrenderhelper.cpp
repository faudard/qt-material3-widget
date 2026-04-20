#include "qtmaterialselectionrenderhelper_p.h"

#include <QFontMetrics>

namespace QtMaterial::SelectionRenderHelper {

qreal stateLayerOpacity(const Theme& theme, const QtMaterialInteractionState& state)
{
    if (!state.isEnabled()) {
        return 0.0;
    }
    const StateLayer& layer = theme.stateLayer();
    if (state.isPressed()) {
        return layer.pressOpacity;
    }
    if (state.isFocused()) {
        return layer.focusOpacity;
    }
    if (state.isHovered()) {
        return layer.hoverOpacity;
    }
    return 0.0;
}

QFont labelFont(const Theme& theme, TypeRole role, const QFont& fallback)
{
    if (theme.typography().contains(role)) {
        return theme.typography().style(role).font;
    }
    return fallback;
}

QRectF centeredStateLayerRect(const QRect& bounds, int stateLayerSize)
{
    return QRectF(bounds.center().x() - stateLayerSize / 2.0,
                  bounds.center().y() - stateLayerSize / 2.0,
                  stateLayerSize,
                  stateLayerSize);
}

void paintCircularStateLayer(QPainter* painter, const QRectF& rect, const QColor& color, qreal opacity)
{
    if (!painter || opacity <= 0.0) {
        return;
    }
    painter->save();
    QColor brush = color;
    brush.setAlphaF(opacity);
    painter->setPen(Qt::NoPen);
    painter->setBrush(brush);
    painter->drawEllipse(rect);
    painter->restore();
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
