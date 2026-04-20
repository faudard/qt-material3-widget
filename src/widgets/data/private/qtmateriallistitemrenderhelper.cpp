#include "qtmateriallistitemrenderhelper_p.h"

#include <QFontMetrics>
#include <QPainter>

namespace QtMaterial {

QFont ListItemRenderHelper::headlineFont(const ListItemSpec&, const Theme&)
{
    QFont font;
    font.setPointSize(11);
    return font;
}

QFont ListItemRenderHelper::supportingFont(const ListItemSpec&, const Theme&)
{
    QFont font;
    font.setPointSize(10);
    return font;
}

QColor ListItemRenderHelper::containerColorForState(const ListItemSpec& spec,
                                                    bool enabled,
                                                    bool hovered,
                                                    bool selected,
                                                    bool pressed)
{
    if (!enabled) {
        return spec.containerColor;
    }
    if (pressed && spec.pressedContainerColor.isValid()) {
        return spec.pressedContainerColor;
    }
    if (selected && spec.selectedContainerColor.isValid()) {
        return spec.selectedContainerColor;
    }
    if (hovered && spec.hoveredContainerColor.isValid()) {
        return spec.hoveredContainerColor;
    }
    return spec.containerColor;
}

qreal ListItemRenderHelper::stateLayerOpacity(const StateLayer& stateLayer,
                                              bool hovered,
                                              bool focused,
                                              bool pressed)
{
    if (pressed) return stateLayer.pressOpacity;
    if (focused) return stateLayer.focusOpacity;
    if (hovered) return stateLayer.hoverOpacity;
    return 0.0;
}

void ListItemRenderHelper::paintTexts(QPainter* painter,
                                      const QRect& headlineRect,
                                      const QRect& supportingRect,
                                      const QString& headline,
                                      const QString& supporting,
                                      const QFont& headlineFont,
                                      const QFont& supportingFont,
                                      const QColor& headlineColor,
                                      const QColor& supportingColor)
{
    painter->save();
    painter->setPen(headlineColor);
    painter->setFont(headlineFont);
    painter->drawText(headlineRect, Qt::AlignLeft | Qt::AlignVCenter, headline);

    if (!supporting.isEmpty()) {
        painter->setPen(supportingColor);
        painter->setFont(supportingFont);
        painter->drawText(supportingRect, Qt::AlignLeft | Qt::AlignVCenter, supporting);
    }
    painter->restore();
}

void ListItemRenderHelper::paintDivider(QPainter* painter,
                                        const QRect& bounds,
                                        const ListItemSpec& spec)
{
    painter->save();
    painter->setPen(spec.dividerColor);
    const int y = bounds.bottom();
    painter->drawLine(bounds.left() + spec.dividerInsetStart,
                      y,
                      bounds.right() - spec.dividerInsetEnd,
                      y);
    painter->restore();
}

QString ListItemRenderHelper::elidedHeadline(const QString& text, const QFontMetrics& fm, int width)
{
    return fm.elidedText(text, Qt::ElideRight, width);
}

QString ListItemRenderHelper::elidedSupporting(const QString& text, const QFontMetrics& fm, int width)
{
    return fm.elidedText(text, Qt::ElideRight, width);
}

} // namespace QtMaterial
