#include "qtmaterialbuttonrenderhelper_p.h"
#include <QtGlobal>

#include <QAbstractButton>
#include <QFontMetrics>
#include <QIcon>
#include <QPainter>
#include <QPixmapCache>
#include <QtMath>

namespace QtMaterial::ButtonRenderHelper {

namespace {

QString tintedIconCacheKey(const QPixmap& base, const QColor& color)
{
    if (base.isNull()) {
        return {};
    }

    return QStringLiteral("QtMaterial::ButtonTint:%1:%2:%3")
        .arg(qulonglong(base.cacheKey()))
        .arg(quint32(color.rgba()), 0, 16)
        .arg(qRound(base.devicePixelRatio() * 1000.0));
}


QPixmap tintPixmap(const QPixmap& source, const QColor& color)
{
    if (source.isNull()) {
        return source;
    }

    QPixmap tinted(source.size());
    tinted.setDevicePixelRatio(source.devicePixelRatio());
    tinted.fill(Qt::transparent);

    QPainter painter(&tinted);
    painter.drawPixmap(0, 0, source);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(tinted.rect(), color);
    return tinted;
}
} // namespace

QRectF containerRect(const QRectF& widgetRect, const ButtonSpec& spec)
{
    const qreal targetHeight = widgetRect.height();
    const qreal containerHeight = qMin<qreal>(spec.containerHeight, targetHeight);
    const qreal top = widgetRect.top() + (targetHeight - containerHeight) / 2.0;
    return QRectF(widgetRect.left(), top, widgetRect.width(), containerHeight);
}


QFont resolvedLabelFont(const QFont& fallback, const ButtonSpec& spec)
{
    return spec.hasResolvedLabelFont ? spec.labelFont : fallback;
}


qreal cornerRadius(const ButtonSpec& spec, const QRectF& bounds)
{
    if (spec.cornerRadius < 0.0) {
        return bounds.height() / 2.0;
    }
    return qMin(spec.cornerRadius, bounds.height() / 2.0);
}

QPainterPath containerPath(
    const ButtonSpec& spec,
    const QRectF& bounds)
{
    QPainterPath path;
    const qreal radius = cornerRadius(spec, bounds);
    path.addRoundedRect(bounds, radius, radius);
    return path;
}

qreal stateLayerOpacity(
    const ButtonSpec& spec,
    const QtMaterialInteractionState& state)
{
    if (!state.isEnabled()) {
        return 0.0;
    }
    if (state.isPressed()) {
        return spec.pressStateLayerOpacity;
    }
    if (state.isFocused()) {
        return spec.focusStateLayerOpacity;
    }
    if (state.isHovered()) {
        return spec.hoverStateLayerOpacity;
    }
    return 0.0;
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
    const bool hasIcon = button && !button->icon().isNull();
    const bool rtl = button && button->layoutDirection() == Qt::RightToLeft;
    layout.hasIcon = hasIcon;

    QFontMetrics metrics(font);

    int iconBlockWidth = 0;
    if (hasIcon) {
        iconBlockWidth = spec.iconSize;
        if (!text.isEmpty()) {
            iconBlockWidth += spec.iconSpacing;
        }
    }

    const int textWidthBudget = qMax(0, availableWidth - iconBlockWidth);
    layout.elidedText = metrics.elidedText(text, Qt::ElideRight, textWidthBudget);

    const int textWidth = metrics.horizontalAdvance(layout.elidedText);
    const int textHeight = metrics.height();
    const bool hasVisibleText = !layout.elidedText.isEmpty();
    const int spacing = (hasIcon && hasVisibleText) ? spec.iconSpacing : 0;
    const int contentWidth = textWidth + (hasIcon ? spec.iconSize : 0) + spacing;
    const int contentLeft = containerRect.left() + qMax(0, (containerRect.width() - contentWidth) / 2);
    const int contentCenterY = containerRect.center().y();

    int x = contentLeft;
    if (hasIcon && rtl) {
        layout.textRect = QRect(x, contentCenterY - textHeight / 2, textWidth, textHeight);
        x += textWidth + spacing;
        layout.iconRect = QRect(x, contentCenterY - spec.iconSize / 2, spec.iconSize, spec.iconSize);
        return layout;
    }

    if (hasIcon) {
        layout.iconRect = QRect(x, contentCenterY - spec.iconSize / 2, spec.iconSize, spec.iconSize);
        x += spec.iconSize + spacing;
    }

    layout.textRect = QRect(x, contentCenterY - textHeight / 2, textWidth, textHeight);
    return layout;
}

QPixmap tintedIconPixmap(
    const QAbstractButton* button,
    const ButtonSpec& spec,
    const QColor& iconColor)
{
    if (!button || button->icon().isNull() || spec.iconSize <= 0) {
        return {};
    }

    const QPixmap base =
        button->icon().pixmap(QSize(spec.iconSize, spec.iconSize), QIcon::Normal, QIcon::Off);

    if (base.isNull()) {
        return {};
    }

    const QString cacheKey = tintedIconCacheKey(base, iconColor);

    QPixmap cached;
    if (!cacheKey.isEmpty() && QPixmapCache::find(cacheKey, &cached)) {
        return cached;
    }

    QPixmap tinted = tintPixmap(base, iconColor);

    if (!cacheKey.isEmpty() && !tinted.isNull()) {
        QPixmapCache::insert(cacheKey, tinted);
    }

    return tinted;
}

void paintContent(
    QPainter* painter,
    const QAbstractButton* button,
    const ButtonSpec& spec,
    const QRect& containerRect,
    const QColor& textColor,
    const QColor& iconColor,
    const QFont& font,
    const QString& text)
{
    const ContentLayout layout = layoutContent(button, spec, containerRect, font, text);
    paintContentLayout(painter, button, spec, layout, textColor, iconColor, font);
}

void paintContentLayout(
    QPainter* painter,
    const QAbstractButton* button,
    const ButtonSpec& spec,
    const ContentLayout& layout,
    const QColor& textColor,
    const QColor& iconColor,
    const QFont& font)
{
    painter->save();
    painter->setFont(font);
    painter->setPen(textColor);

    if (layout.hasIcon) {
        const QPixmap pixmap = tintedIconPixmap(button, spec, iconColor);
        painter->drawPixmap(layout.iconRect, pixmap);
    }

    if (!layout.elidedText.isEmpty()) {
        painter->drawText(layout.textRect, Qt::AlignVCenter | Qt::AlignLeft, layout.elidedText);
    }

    painter->restore();
}

} // namespace QtMaterial::ButtonRenderHelper
