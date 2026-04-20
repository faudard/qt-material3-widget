#include "qtmaterialtextfieldshellhelper_p.h"

#include <QFontMetrics>
#include <QPainter>

#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"

namespace QtMaterial {

QtMaterialTextFieldShellHelper::Layout QtMaterialTextFieldShellHelper::layoutFor(
    const QRect& bounds,
    const TextFieldSpec& spec,
    const Theme& theme,
    Variant variant)
{
    Layout layout;
    const int containerBottomInset = spec.supportingHeight + spec.supportingTopSpacing;

    layout.containerRect = bounds.adjusted(1, spec.topLabelHeight, -1, -containerBottomInset);
    layout.labelRect = QRect(bounds.left() + spec.horizontalPadding,
                             0,
                             qMax(0, bounds.width() - (2 * spec.horizontalPadding)),
                             spec.topLabelHeight);
    layout.editorRect = layout.containerRect.adjusted(spec.horizontalPadding,
                                                      spec.verticalPadding,
                                                      -spec.horizontalPadding,
                                                      -spec.verticalPadding);
    layout.supportingRect = QRect(bounds.left() + spec.horizontalPadding,
                                  bounds.bottom() - spec.supportingHeight + 1,
                                  qMax(0, bounds.width() - (2 * spec.horizontalPadding)),
                                  spec.supportingHeight - 2);
    layout.focusRect = layout.containerRect.adjusted(-2, -2, 2, 2);
    layout.radius = theme.shapes().radius(spec.shapeRole);

    if (variant == Variant::Filled) {
        layout.editorRect.adjust(0, 4, 0, 0);
    }

    return layout;
}

QtMaterialTextFieldShellHelper::ElidedText QtMaterialTextFieldShellHelper::elidedTextFor(
    const Layout& layout,
    const TextFieldSpec&,
    const QString& label,
    const QString& supportingText,
    const QString& errorText,
    const QFont& font)
{
    const QFontMetrics metrics(font);
    ElidedText text;
    text.labelText = metrics.elidedText(label, Qt::ElideRight, qMax(0, layout.labelRect.width()));
    text.supportingText = metrics.elidedText(supportingText, Qt::ElideRight, qMax(0, layout.supportingRect.width()));
    text.errorText = metrics.elidedText(errorText, Qt::ElideRight, qMax(0, layout.supportingRect.width()));
    return text;
}

QColor QtMaterialTextFieldShellHelper::effectiveLabelColor(const TextFieldSpec& spec, bool focused, bool hasError, bool enabled)
{
    if (!enabled) {
        return spec.disabledLabelColor;
    }
    if (hasError) {
        return spec.errorColor;
    }
    return focused ? spec.focusedOutlineColor : spec.labelColor;
}

QColor QtMaterialTextFieldShellHelper::effectiveOutlineColor(const TextFieldSpec& spec, bool focused, bool hasError, bool enabled)
{
    if (!enabled) {
        return spec.disabledOutlineColor;
    }
    if (hasError) {
        return spec.errorColor;
    }
    return focused ? spec.focusedOutlineColor : spec.outlineColor;
}

QColor QtMaterialTextFieldShellHelper::effectiveSupportingColor(const TextFieldSpec& spec, bool hasError, bool enabled)
{
    if (!enabled) {
        return spec.disabledSupportingTextColor;
    }
    return hasError ? spec.errorColor : spec.supportingTextColor;
}

void QtMaterialTextFieldShellHelper::paintShell(
    QPainter* painter,
    const Layout& layout,
    const Theme& theme,
    const TextFieldSpec& spec,
    const QtMaterialInteractionState& state,
    Variant variant,
    const ElidedText& text,
    bool hasError,
    const QFont& font)
{
    if (!painter) {
        return;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    const bool enabled = state.isEnabled();
    const bool focused = enabled && state.isFocused();
    const QColor outline = effectiveOutlineColor(spec, focused, hasError, enabled);
    const QColor labelColor = effectiveLabelColor(spec, focused, hasError, enabled);
    const QColor supportingColor = effectiveSupportingColor(spec, hasError, enabled);

    if (variant == Variant::Filled) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(spec.containerColor);
        painter->drawRoundedRect(layout.containerRect, layout.radius, layout.radius);

        if (enabled && (state.isHovered() || focused || state.isPressed())) {
            qreal opacity = state.isPressed() ? theme.stateLayer().pressOpacity
                                              : (focused ? theme.stateLayer().focusOpacity : theme.stateLayer().hoverOpacity);
            QtMaterialStateLayerPainter::paintRect(painter, layout.containerRect, spec.stateLayerColor, opacity, layout.radius);
        }

        painter->setPen(QPen(outline, focused ? spec.focusedOutlineWidth : spec.outlineWidth));
        painter->drawLine(layout.containerRect.bottomLeft(), layout.containerRect.bottomRight());
    } else {
        painter->setPen(QPen(outline, focused ? spec.focusedOutlineWidth : spec.outlineWidth));
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(layout.containerRect, layout.radius, layout.radius);

        if (enabled && (state.isHovered() || focused || state.isPressed())) {
            qreal opacity = state.isPressed() ? theme.stateLayer().pressOpacity
                                              : (focused ? theme.stateLayer().focusOpacity : theme.stateLayer().hoverOpacity);
            QtMaterialStateLayerPainter::paintRect(painter, layout.containerRect, spec.stateLayerColor, opacity * 0.35, layout.radius);
        }
    }

    painter->setFont(font);
    painter->setPen(labelColor);
    painter->drawText(layout.labelRect, Qt::AlignLeft | Qt::AlignVCenter, text.labelText);

    painter->setPen(supportingColor);
    painter->drawText(layout.supportingRect,
                      Qt::AlignLeft | Qt::AlignVCenter,
                      hasError ? text.errorText : text.supportingText);

    if (focused) {
        QtMaterialFocusIndicator::paintRectFocusRing(
            painter,
            layout.focusRect,
            spec.focusRingColor,
            layout.radius + 2.0,
            spec.focusRingWidth);
    }

    painter->restore();
}

} // namespace QtMaterial
