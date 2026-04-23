#include "qtmaterialtextfieldshellhelper_p.h"

#include <QFontMetrics>
#include <QObject>
#include <QPainter>
#include <QtGlobal>

#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"

namespace QtMaterial {
namespace {

static int boundedTextSlotWidth(const QFontMetrics& metrics,
                                const QString& text,
                                int minimumWidth,
                                int textPadding)
{
    if (text.isEmpty()) {
        return 0;
    }

    return qMax(minimumWidth,
                metrics.horizontalAdvance(text) + (2 * textPadding));
}

static int endActionSlotWidth(const QtMaterialTextFieldShellHelper::Accessories& accessories,
                              const QFontMetrics& metrics)
{
    switch (accessories.endActionMode) {
    case QtMaterialTextFieldShellHelper::EndActionMode::ClearText:
        return accessories.iconExtent + 6;

    case QtMaterialTextFieldShellHelper::EndActionMode::TogglePasswordVisibility:
        return metrics.horizontalAdvance(
                   accessories.passwordVisible ? QObject::tr("Hide")
                                               : QObject::tr("Show"))
               + 16;

    case QtMaterialTextFieldShellHelper::EndActionMode::CustomTrailingAction:
        if (accessories.hasCustomEndActionIcon && !accessories.customEndActionText.isEmpty()) {
            return qMax(accessories.iconExtent,
                        metrics.horizontalAdvance(accessories.customEndActionText) + 16);
        }
        if (accessories.hasCustomEndActionIcon) {
            return accessories.iconExtent + 6;
        }
        return metrics.horizontalAdvance(accessories.customEndActionText) + 16;

    case QtMaterialTextFieldShellHelper::EndActionMode::None:
    default:
        return 0;
    }
}

static QRect placeLeading(bool rtl,
                          int& cursor,
                          int y,
                          int width,
                          int height,
                          int spacing)
{
    if (!rtl) {
        const QRect rect(cursor, y, width, height);
        cursor += width + spacing;
        return rect;
    }

    const QRect rect(cursor - width, y, width, height);
    cursor -= width + spacing;
    return rect;
}

static QRect placeTrailing(bool rtl,
                           int& cursor,
                           int y,
                           int width,
                           int height,
                           int spacing)
{
    if (!rtl) {
        cursor -= width;
        const QRect rect(cursor, y, width, height);
        cursor -= spacing;
        return rect;
    }

    const QRect rect(cursor, y, width, height);
    cursor += width + spacing;
    return rect;
}

} // namespace

QtMaterialTextFieldShellHelper::Layout QtMaterialTextFieldShellHelper::layoutFor(
    const QRect& bounds,
    const TextFieldSpec& spec,
    const Theme& theme,
    Variant variant,
    const Accessories& accessories,
    const QFont& font)
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

    const QFontMetrics metrics(font);
    const bool rtl = (accessories.layoutDirection == Qt::RightToLeft);
    const int centerY = layout.editorRect.center().y();

    int leadingCursor = rtl ? (layout.editorRect.right() + 1)
                            : layout.editorRect.left();
    int trailingCursor = rtl ? layout.editorRect.left()
                             : (layout.editorRect.right() + 1);

    if (accessories.hasLeadingIcon) {
        layout.leadingIconRect = placeLeading(rtl,
                                              leadingCursor,
                                              centerY - (accessories.iconExtent / 2),
                                              accessories.iconExtent,
                                              accessories.iconExtent,
                                              accessories.accessorySpacing);
    }

    const int prefixWidth = boundedTextSlotWidth(metrics,
                                                 accessories.prefixText,
                                                 accessories.minimumAccessoryTextWidth,
                                                 accessories.accessoryTextPadding);
    if (prefixWidth > 0) {
        layout.prefixRect = placeLeading(rtl,
                                         leadingCursor,
                                         layout.editorRect.top(),
                                         prefixWidth,
                                         layout.editorRect.height(),
                                         accessories.accessorySpacing);
    }

    const int suffixWidth = boundedTextSlotWidth(metrics,
                                                 accessories.suffixText,
                                                 accessories.minimumAccessoryTextWidth,
                                                 accessories.accessoryTextPadding);
    if (suffixWidth > 0) {
        layout.suffixRect = placeTrailing(rtl,
                                          trailingCursor,
                                          layout.editorRect.top(),
                                          suffixWidth,
                                          layout.editorRect.height(),
                                          accessories.accessorySpacing);
    }

    int endActionWidth = endActionSlotWidth(accessories, metrics);
    if (accessories.endActionMode == EndActionMode::None && accessories.hasTrailingIcon) {
        endActionWidth = qMax(endActionWidth, accessories.iconExtent);
    }

    if (accessories.endActionMode != EndActionMode::None) {
        layout.endActionRect = placeTrailing(rtl,
                                             trailingCursor,
                                             centerY - (layout.editorRect.height() / 2),
                                             endActionWidth,
                                             layout.editorRect.height(),
                                             accessories.accessorySpacing);
    } else if (accessories.hasTrailingIcon) {
        layout.trailingIconRect = placeTrailing(rtl,
                                                trailingCursor,
                                                centerY - (accessories.iconExtent / 2),
                                                accessories.iconExtent,
                                                accessories.iconExtent,
                                                accessories.accessorySpacing);
    }

    if (!rtl) {
        layout.editorRect = QRect(leadingCursor,
                                  layout.editorRect.top(),
                                  qMax(0, trailingCursor - leadingCursor),
                                  layout.editorRect.height());
    } else {
        layout.editorRect = QRect(trailingCursor,
                                  layout.editorRect.top(),
                                  qMax(0, leadingCursor - trailingCursor),
                                  layout.editorRect.height());
    }

    return layout;
}

QtMaterialTextFieldShellHelper::ElidedText QtMaterialTextFieldShellHelper::elidedTextFor(
    const Layout& layout,
    const TextFieldSpec&,
    const Accessories& accessories,
    const QString& label,
    const QString& supportingText,
    const QString& errorText,
    const QFont& font)
{
    const QFontMetrics metrics(font);

    ElidedText text;
    text.labelText = metrics.elidedText(label,
                                        Qt::ElideRight,
                                        qMax(0, layout.labelRect.width()));
    text.supportingText = metrics.elidedText(supportingText,
                                             Qt::ElideRight,
                                             qMax(0, layout.supportingRect.width()));
    text.errorText = metrics.elidedText(errorText,
                                        Qt::ElideRight,
                                        qMax(0, layout.supportingRect.width()));
    text.prefixText = metrics.elidedText(accessories.prefixText,
                                         Qt::ElideRight,
                                         qMax(0, layout.prefixRect.width()));
    text.suffixText = metrics.elidedText(accessories.suffixText,
                                         Qt::ElideRight,
                                         qMax(0, layout.suffixRect.width()));

    switch (accessories.endActionMode) {
    case EndActionMode::TogglePasswordVisibility:
        text.endActionText = metrics.elidedText(
            accessories.passwordVisible ? QObject::tr("Hide") : QObject::tr("Show"),
            Qt::ElideRight,
            qMax(0, layout.endActionRect.width()));
        break;

    case EndActionMode::CustomTrailingAction:
        text.endActionText = metrics.elidedText(
            accessories.customEndActionText,
            Qt::ElideRight,
            qMax(0, layout.endActionRect.width()));
        break;

    case EndActionMode::ClearText:
    case EndActionMode::None:
    default:
        break;
    }

    return text;
}

QColor QtMaterialTextFieldShellHelper::effectiveLabelColor(const TextFieldSpec& spec,
                                                           bool focused,
                                                           bool hasError,
                                                           bool enabled)
{
    if (!enabled) {
        return spec.disabledLabelColor;
    }
    if (hasError) {
        return spec.errorColor;
    }
    return focused ? spec.focusedOutlineColor : spec.labelColor;
}

QColor QtMaterialTextFieldShellHelper::effectiveOutlineColor(const TextFieldSpec& spec,
                                                             bool focused,
                                                             bool hasError,
                                                             bool enabled)
{
    if (!enabled) {
        return spec.disabledOutlineColor;
    }
    if (hasError) {
        return spec.errorColor;
    }
    return focused ? spec.focusedOutlineColor : spec.outlineColor;
}

QColor QtMaterialTextFieldShellHelper::effectiveSupportingColor(const TextFieldSpec& spec,
                                                                bool hasError,
                                                                bool enabled)
{
    if (!enabled) {
        return spec.disabledSupportingTextColor;
    }
    return hasError ? spec.errorColor : spec.supportingTextColor;
}

void QtMaterialTextFieldShellHelper::paintShell(QPainter* painter,
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
            const qreal opacity = state.isPressed()
            ? theme.stateLayer().pressOpacity
            : (focused ? theme.stateLayer().focusOpacity
                       : theme.stateLayer().hoverOpacity);

            QtMaterialStateLayerPainter::paintRect(painter,
                                                   layout.containerRect,
                                                   spec.stateLayerColor,
                                                   opacity,
                                                   layout.radius);
        }

        painter->setPen(QPen(outline,
                             focused ? spec.focusedOutlineWidth
                                     : spec.outlineWidth));
        painter->drawLine(layout.containerRect.bottomLeft(),
                          layout.containerRect.bottomRight());
    } else {
        painter->setPen(QPen(outline,
                             focused ? spec.focusedOutlineWidth
                                     : spec.outlineWidth));
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(layout.containerRect, layout.radius, layout.radius);

        if (enabled && (state.isHovered() || focused || state.isPressed())) {
            const qreal opacity = state.isPressed()
            ? theme.stateLayer().pressOpacity
            : (focused ? theme.stateLayer().focusOpacity
                       : theme.stateLayer().hoverOpacity);

            QtMaterialStateLayerPainter::paintRect(painter,
                                                   layout.containerRect,
                                                   spec.stateLayerColor,
                                                   opacity * 0.35,
                                                   layout.radius);
        }
    }

    painter->setFont(font);

    painter->setPen(labelColor);
    painter->drawText(layout.labelRect,
                      Qt::AlignLeft | Qt::AlignVCenter,
                      text.labelText);

    painter->setPen(supportingColor);
    painter->drawText(layout.supportingRect,
                      Qt::AlignLeft | Qt::AlignVCenter,
                      hasError ? text.errorText : text.supportingText);

    if (focused) {
        QtMaterialFocusIndicator::paintRectFocusRing(painter,
                                                     layout.focusRect,
                                                     spec.focusRingColor,
                                                     layout.radius + 2.0,
                                                     spec.focusRingWidth);
    }

    painter->restore();
}

} // namespace QtMaterial