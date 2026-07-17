#include "qtmaterial/specs/qtmaterialbuttonspecresolver.h"

#include <QStringList>

#include "qtmaterialcomponenttokenapplier_p.h"

namespace QtMaterial {

ButtonSpec ButtonSpecResolver::baseButtonSpec(
    Density density) noexcept
{
    ButtonSpec spec;
    spec.containerColor = Qt::transparent;
    spec.outlineColor = Qt::transparent;
    spec.disabledOutlineColor = Qt::transparent;
    spec.containerHeight = buttonHeightForDensity(density);
    return spec;
}

int ButtonSpecResolver::buttonHeightForDensity(Density density) noexcept
{
    switch (density) {
    case Density::Compact:
        return 36;
    case Density::Comfortable:
        return 44;
    case Density::Default:
    default:
        return 40;
    }
}

ButtonSpec ButtonSpecResolver::resolve(
    ButtonVariant variant,
    const Theme& theme,
    Density density) const
{
    switch (variant) {
    case ButtonVariant::Text:
        return textButtonSpec(theme, density);
    case ButtonVariant::Filled:
        return filledButtonSpec(theme, density);
    case ButtonVariant::FilledTonal:
        return filledTonalButtonSpec(theme, density);
    case ButtonVariant::Outlined:
        return outlinedButtonSpec(theme, density);
    case ButtonVariant::Elevated:
        return elevatedButtonSpec(theme, density);
    }

    return textButtonSpec(theme, density);
}

ButtonSpec ButtonSpecResolver::textButtonSpec(
    const Theme& theme,
    Density density) const
{
    ButtonSpec spec = baseButtonSpec(density);
    spec.labelColor = theme.colorScheme().color(ColorRole::Primary);
    spec.iconColor = spec.labelColor;
    spec.disabledContainerColor = Qt::transparent;
    spec.disabledLabelColor =
        theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.stateLayerColor = theme.colorScheme().color(ColorRole::Primary);
    spec.focusRingColor = theme.colorScheme().color(ColorRole::Primary);

    applyButtonComponentTokens(
        theme,
        QStringList{
            QStringLiteral("button"),
            QStringLiteral("button.text"),
            QStringLiteral("TextButton")},
        &spec);
    return spec;
}

ButtonSpec ButtonSpecResolver::filledButtonSpec(
    const Theme& theme,
    Density density) const
{
    ButtonSpec spec = baseButtonSpec(density);
    spec.containerColor = theme.colorScheme().color(ColorRole::Primary);
    spec.labelColor = theme.colorScheme().color(ColorRole::OnPrimary);
    spec.iconColor = spec.labelColor;
    spec.disabledContainerColor =
        theme.colorScheme().color(ColorRole::SurfaceContainerHigh);
    spec.disabledLabelColor =
        theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.stateLayerColor = theme.colorScheme().color(ColorRole::OnPrimary);
    spec.focusRingColor = theme.colorScheme().color(ColorRole::Primary);
    spec.elevationRole = ElevationRole::Level1;

    applyButtonComponentTokens(
        theme,
        QStringList{
            QStringLiteral("button"),
            QStringLiteral("button.filled"),
            QStringLiteral("FilledButton")},
        &spec);
    return spec;
}

ButtonSpec ButtonSpecResolver::filledTonalButtonSpec(
    const Theme& theme,
    Density density) const
{
    ButtonSpec spec = baseButtonSpec(density);
    spec.containerColor =
        theme.colorScheme().color(ColorRole::SecondaryContainer);
    spec.labelColor =
        theme.colorScheme().color(ColorRole::OnSecondaryContainer);
    spec.iconColor = spec.labelColor;
    spec.disabledContainerColor =
        theme.colorScheme().color(ColorRole::SurfaceContainerHigh);
    spec.disabledLabelColor =
        theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.stateLayerColor = spec.labelColor;
    spec.focusRingColor = theme.colorScheme().color(ColorRole::Secondary);

    applyButtonComponentTokens(
        theme,
        QStringList{
            QStringLiteral("button"),
            QStringLiteral("button.filledTonal"),
            QStringLiteral("FilledTonalButton")},
        &spec);
    return spec;
}

ButtonSpec ButtonSpecResolver::outlinedButtonSpec(
    const Theme& theme,
    Density density) const
{
    ButtonSpec spec = baseButtonSpec(density);
    spec.labelColor = theme.colorScheme().color(ColorRole::Primary);
    spec.iconColor = spec.labelColor;
    spec.disabledContainerColor = Qt::transparent;
    spec.disabledLabelColor =
        theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.stateLayerColor = theme.colorScheme().color(ColorRole::Primary);
    spec.focusRingColor = theme.colorScheme().color(ColorRole::Primary);
    spec.outlineColor = theme.colorScheme().color(ColorRole::Outline);
    spec.disabledOutlineColor =
        theme.colorScheme().color(ColorRole::OutlineVariant);

    applyButtonComponentTokens(
        theme,
        QStringList{
            QStringLiteral("button"),
            QStringLiteral("button.outlined"),
            QStringLiteral("OutlinedButton")},
        &spec);
    return spec;
}

ButtonSpec ButtonSpecResolver::elevatedButtonSpec(
    const Theme& theme,
    Density density) const
{
    ButtonSpec spec = baseButtonSpec(density);
    spec.containerColor =
        theme.colorScheme().color(ColorRole::SurfaceContainerLow);
    spec.labelColor = theme.colorScheme().color(ColorRole::Primary);
    spec.iconColor = spec.labelColor;
    spec.disabledContainerColor =
        theme.colorScheme().color(ColorRole::SurfaceContainerHigh);
    spec.disabledLabelColor =
        theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.stateLayerColor = spec.labelColor;
    spec.focusRingColor = theme.colorScheme().color(ColorRole::Primary);
    spec.elevationRole = ElevationRole::Level1;

    applyButtonComponentTokens(
        theme,
        QStringList{
            QStringLiteral("button"),
            QStringLiteral("button.elevated"),
            QStringLiteral("ElevatedButton")},
        &spec);
    return spec;
}

} // namespace QtMaterial
