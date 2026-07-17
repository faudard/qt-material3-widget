#include "qtmaterial/specs/qtmaterialtextfieldspecresolver.h"

#include <QStringList>

#include "qtmaterialcomponenttokenapplier_p.h"

namespace QtMaterial {

TextFieldSpec TextFieldSpecResolver::baseTextFieldSpec(
    const Theme& theme,
    Density density)
{
    TextFieldSpec spec;
    spec.containerColor = Qt::transparent;
    spec.activeIndicatorColor =
        theme.colorScheme().color(ColorRole::Primary);
    spec.outlineColor =
        theme.colorScheme().color(ColorRole::Outline);
    spec.focusedOutlineColor =
        theme.colorScheme().color(ColorRole::Primary);
    spec.disabledOutlineColor =
        theme.colorScheme().color(
            ColorRole::OutlineVariant);
    spec.inputTextColor =
        theme.colorScheme().color(ColorRole::OnSurface);
    spec.disabledInputTextColor =
        theme.colorScheme().color(
            ColorRole::OnSurfaceVariant);
    spec.labelColor =
        theme.colorScheme().color(
            ColorRole::OnSurfaceVariant);
    spec.disabledLabelColor = spec.labelColor;
    spec.supportingTextColor = spec.labelColor;
    spec.disabledSupportingTextColor = spec.labelColor;
    spec.errorTextColor =
        theme.colorScheme().color(ColorRole::Error);
    spec.cursorColor =
        theme.colorScheme().color(ColorRole::Primary);
    spec.errorColor =
        theme.colorScheme().color(ColorRole::Error);
    spec.stateLayerColor =
        theme.colorScheme().color(ColorRole::OnSurface);
    spec.disabledErrorTextColor =
        theme.colorScheme().color(
            ColorRole::OnSurfaceVariant);
    spec.focusRingColor =
        theme.colorScheme().color(ColorRole::Primary);

    switch (density) {
    case Density::Compact:
        spec.minHeight = 52;
        spec.horizontalPadding = 12;
        spec.verticalPadding = 6;
        break;

    case Density::Comfortable:
        spec.minHeight = 60;
        spec.horizontalPadding = 18;
        spec.verticalPadding = 10;
        break;

    case Density::Default:
    default:
        break;
    }

    applyTextFieldComponentTokens(
        theme,
        QStringList{
            QStringLiteral("input"),
            QStringLiteral("textField")},
        &spec);

    return spec;
}

TextFieldSpec TextFieldSpecResolver::outlinedTextFieldSpec(
    const Theme& theme,
    Density density) const
{
    TextFieldSpec spec =
        baseTextFieldSpec(theme, density);
    spec.containerColor = Qt::transparent;

    applyTextFieldComponentTokens(
        theme,
        QStringList{
            QStringLiteral("textField.outlined"),
            QStringLiteral("OutlinedTextField")},
        &spec);

    return spec;
}

TextFieldSpec TextFieldSpecResolver::filledTextFieldSpec(
    const Theme& theme,
    Density density) const
{
    TextFieldSpec spec =
        baseTextFieldSpec(theme, density);
    spec.containerColor =
        theme.colorScheme().color(
            ColorRole::SurfaceContainerHighest);

    applyTextFieldComponentTokens(
        theme,
        QStringList{
            QStringLiteral("textField.filled"),
            QStringLiteral("FilledTextField")},
        &spec);

    return spec;
}

} // namespace QtMaterial
