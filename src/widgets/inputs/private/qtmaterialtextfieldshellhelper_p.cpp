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
    const int supportingHeight = 20;
    const int topLabelHeight = 20;
    const int containerBottomInset = supportingHeight + spec.supportingTopSpacing;

    layout.containerRect = bounds.adjusted(1, topLabelHeight, -1, -containerBottomInset);
    layout.labelRect = QRect(bounds.left() + spec.horizontalPadding,
                             0,
                             qMax(0, bounds.width() - (2 * spec.horizontalPadding)),
                             topLabelHeight);
    layout.editorRect = layout.containerRect.adjusted(spec.horizontalPadding,
                                                      spec.verticalPadding,
                                                      -spec.horizontalPadding,
                                                      -spec.verticalPadding);
    layout.supportingRect = QRect(bounds.left() + spec.horizontalPadding,
                                  bounds.bottom() - supportingHeight + 1,
                                  qMax(0, bounds.width() - (2 * spec.horizontalPadding)),
                                  supportingHeight - 2);
    layout.focusRect = layout.containerRect.adjusted(-2, -2, 2, 2);
    layout.radius = theme.shapes().radius(spec.shapeRole);

    if (variant == Variant::Filled) {
        layout.editorRect.adjust(0, 4, 0, 0);
    }

    return layout;
}

QColor QtMaterialTextFieldShellHelper::effectiveLabelColor(const TextFieldSpec& spec, bool focused, bool hasError)
{
    if (hasError) {
        return spec.errorColor;
    }
    return focused ? spec.focusedOutlineColor : spec.labelColor;
}

QColor QtMaterialTextFieldShellHelper::effectiveOutlineColor(const TextFieldSpec& spec, bool focused, bool hasError)
{
    if (hasError) {
        return spec.errorColor;
    }
    return focused ? spec.focusedOutlineColor : spec.outlineColor;
}

void QtMaterialTextFieldShellHelper::paintShell(
    QPainter* painter,
    const Layout& layout,
    const Theme& theme,
    const TextFieldSpec& spec,
    const QtMaterialInteractionState& state,
    Variant variant,
    const QString& label,
    const QString& supportingText,
    const QString& errorText,
    bool hasError,
    const QFont& font)
{
    if (!painter) {
        return;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    const bool focused = state.isFocused();
    const QColor outline = effectiveOutlineColor(spec, focused, hasError);
    const QColor labelColor = effectiveLabelColor(spec, focused, hasError);

    if (variant == Variant::Filled) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(spec.containerColor);
        painter->drawRoundedRect(layout.containerRect, layout.radius, layout.radius);

        if (state.isHovered() || focused || state.isPressed()) {
            qreal opacity = state.isPressed() ? theme.stateLayer().pressOpacity : (focused ? theme.stateLayer().focusOpacity : theme.stateLayer().hoverOpacity);
            QtMaterialStateLayerPainter::paintRect(painter, layout.containerRect, spec.stateLayerColor, opacity, layout.radius);
        }

        painter->setPen(QPen(outline, focused ? 2.0 : 1.0));
        painter->drawLine(layout.containerRect.bottomLeft(), layout.containerRect.bottomRight());
    } else {
        painter->setPen(QPen(outline, focused ? 2.0 : 1.0));
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(layout.containerRect, layout.radius, layout.radius);

        if (state.isHovered() || focused || state.isPressed()) {
            qreal opacity = state.isPressed() ? theme.stateLayer().pressOpacity : (focused ? theme.stateLayer().focusOpacity : theme.stateLayer().hoverOpacity);
            QtMaterialStateLayerPainter::paintRect(painter, layout.containerRect, spec.stateLayerColor, opacity * 0.35, layout.radius);
        }
    }

    painter->setFont(font);
    painter->setPen(labelColor);
    painter->drawText(layout.labelRect, Qt::AlignLeft | Qt::AlignVCenter, label);

    painter->setPen(hasError ? spec.errorColor : spec.supportingTextColor);
    painter->drawText(layout.supportingRect, Qt::AlignLeft | Qt::AlignVCenter, hasError ? errorText : supportingText);

    if (focused) {
        QtMaterialFocusIndicator::paintRectFocusRing(painter, layout.focusRect, outline, layout.radius + 2.0, 1.25);
    }

    painter->restore();
}

} // namespace QtMaterial
