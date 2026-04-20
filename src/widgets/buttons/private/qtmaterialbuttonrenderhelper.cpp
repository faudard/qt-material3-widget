#include "qtmaterialbuttonrenderhelper_p.h"

#include <QAbstractButton>
#include <QFontMetrics>
#include <QIcon>
#include <QtMath>

namespace QtMaterial::ButtonRenderHelper {

QRectF containerRect(const QRectF& widgetRect, const ButtonSpec& spec)
{
    const qreal targetHeight = widgetRect.height();
    const qreal containerHeight = qMin<qreal>(spec.containerHeight, targetHeight);
    const qreal top = widgetRect.top() + (targetHeight - containerHeight) / 2.0;
    return QRectF(widgetRect.left(), top, widgetRect.width(), containerHeight);
}

qreal cornerRadius(const Theme& theme, const ButtonSpec& spec, const QRectF& bounds)
{
    if (spec.shapeRole == ShapeRole::Full) {
        return bounds.height() / 2.0;
    }

    const int themeRadius = theme.shapes().radius(spec.shapeRole);
    if (themeRadius > 0) {
        return qMin<qreal>(themeRadius, bounds.height() / 2.0);
    }

    return bounds.height() / 2.0;
}

QPainterPath containerPath(const Theme& theme, const ButtonSpec& spec, const QRectF& bounds)
{
    QPainterPath path;
    const qreal radius = cornerRadius(theme, spec, bounds);
    path.addRoundedRect(bounds, radius, radius);
    return path;
}

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

ContentLayout layoutContent(
    const QAbstractButton* button,
    const ButtonSpec& spec,
    const QRect& containerRect,
    const QFont& font,
    const QString& text)
{
    ContentLayout layout;
    const int left = containerRect.left() + spec.horizontalPadding;
    const int right = containerRect.right() - spec.horizontalPadding;
    const int availableWidth = qMax(0, right - left + 1);

    const bool hasIcon = !button->icon().isNull();
    layout.hasIcon = hasIcon;

    QFontMetrics metrics(font);
    int textWidthBudget = availableWidth;
    int iconBlockWidth = 0;

    if (hasIcon) {
        iconBlockWidth = spec.iconSize;
        if (!text.isEmpty()) {
            iconBlockWidth += spec.iconSpacing;
        }
        textWidthBudget = qMax(0, availableWidth - iconBlockWidth);
    }

    layout.elidedText = metrics.elidedText(text, Qt::ElideRight, textWidthBudget);
    const int textWidth = metrics.horizontalAdvance(layout.elidedText);
    const int textHeight = metrics.height();
    const int contentWidth = textWidth + iconBlockWidth;
    const int contentLeft = containerRect.left() + qMax(0, (containerRect.width() - contentWidth) / 2);
    const int contentCenterY = containerRect.center().y();

    int x = contentLeft;
    if (hasIcon) {
        layout.iconRect = QRect(x, contentCenterY - spec.iconSize / 2, spec.iconSize, spec.iconSize);
        x += spec.iconSize;
        if (!text.isEmpty()) {
            x += spec.iconSpacing;
        }
    }

    layout.textRect = QRect(x, contentCenterY - textHeight / 2, qMax(textWidthBudget, textWidth), textHeight);
    return layout;
}

void paintContent(
    QPainter* painter,
    const QAbstractButton* button,
    const ButtonSpec& spec,
    const QRect& containerRect,
    const QColor& textColor,
    const QFont& font,
    const QString& text)
{
    const ContentLayout layout = layoutContent(button, spec, containerRect, font, text);

    painter->save();
    painter->setFont(font);
    painter->setPen(textColor);

    if (layout.hasIcon) {
        const QPixmap pixmap = button->icon().pixmap(QSize(spec.iconSize, spec.iconSize), button->isEnabled() ? QIcon::Normal : QIcon::Disabled);
        painter->drawPixmap(layout.iconRect, pixmap);
    }

    if (!layout.elidedText.isEmpty()) {
        painter->drawText(layout.textRect, Qt::AlignVCenter | Qt::AlignLeft, layout.elidedText);
    }

    painter->restore();
}

} // namespace QtMaterial::ButtonRenderHelper
