#include "qtmaterial/specs/qtmaterialactionbuttonspecresolver.h"

#include <QStringList>

#include "qtmaterialcomponenttokenapplier_p.h"

namespace QtMaterial {

FabSpec ActionButtonSpecResolver::fabSpec(
    const Theme& theme,
    Density density) const
{
    Q_UNUSED(density)

    FabSpec spec;
    spec.containerColor =
        theme.colorScheme().color(ColorRole::PrimaryContainer);
    spec.iconColor =
        theme.colorScheme().color(ColorRole::OnPrimaryContainer);
    spec.disabledContainerColor =
        theme.colorScheme().color(
            ColorRole::SurfaceContainerHigh);
    spec.disabledIconColor =
        theme.colorScheme().color(
            ColorRole::OnSurfaceVariant);
    spec.stateLayerColor = spec.iconColor;
    spec.focusRingColor =
        theme.colorScheme().color(ColorRole::Primary);
    spec.shadowColor =
        theme.colorScheme().color(ColorRole::Shadow);

    applyFabComponentTokens(
        theme,
        QStringList{
            QStringLiteral("button"),
            QStringLiteral("fab"),
            QStringLiteral("Fab")},
        &spec);

    return spec;
}

FabSpec ActionButtonSpecResolver::extendedFabSpec(
    const Theme& theme,
    Density density) const
{
    FabSpec spec = fabSpec(theme, density);
    spec.touchTarget = QSize(80, 56);

    applyFabComponentTokens(
        theme,
        QStringList{
            QStringLiteral("button"),
            QStringLiteral("fab"),
            QStringLiteral("fab.extended"),
            QStringLiteral("ExtendedFab")},
        &spec);

    return spec;
}

IconButtonSpec ActionButtonSpecResolver::iconButtonSpec(
    const Theme& theme,
    Density density) const
{
    Q_UNUSED(density)

    IconButtonSpec spec;
    spec.containerColor = Qt::transparent;
    spec.iconColor =
        theme.colorScheme().color(
            ColorRole::OnSurfaceVariant);
    spec.selectedContainerColor =
        theme.colorScheme().color(
            ColorRole::PrimaryContainer);
    spec.selectedIconColor =
        theme.colorScheme().color(
            ColorRole::OnPrimaryContainer);
    spec.disabledIconColor =
        theme.colorScheme().color(
            ColorRole::OnSurfaceVariant);
    spec.stateLayerColor = spec.iconColor;
    spec.focusRingColor =
        theme.colorScheme().color(ColorRole::Primary);

    applyIconButtonComponentTokens(
        theme,
        QStringList{
            QStringLiteral("button"),
            QStringLiteral("iconButton"),
            QStringLiteral("IconButton")},
        &spec);

    return spec;
}

void ActionButtonSpecResolver::applyColorVariant(
    const Theme& theme,
    FabColorVariant variant,
    FabSpec* spec)
{
    if (!spec) {
        return;
    }

    const ColorScheme& colors = theme.colorScheme();

    switch (variant) {
    case FabColorVariant::Primary:
        return;

    case FabColorVariant::Secondary:
        spec->containerColor =
            colors.color(ColorRole::SecondaryContainer);
        spec->iconColor =
            colors.color(ColorRole::OnSecondaryContainer);
        spec->stateLayerColor = spec->iconColor;
        spec->focusRingColor =
            colors.color(ColorRole::Secondary);
        return;

    case FabColorVariant::Tertiary:
        spec->containerColor =
            colors.color(ColorRole::TertiaryContainer);
        spec->iconColor =
            colors.color(ColorRole::OnTertiaryContainer);
        spec->stateLayerColor = spec->iconColor;
        spec->focusRingColor =
            colors.color(ColorRole::Tertiary);
        return;

    case FabColorVariant::Surface:
        spec->containerColor =
            colors.color(ColorRole::SurfaceContainerHigh);
        spec->iconColor =
            colors.color(ColorRole::Primary);
        spec->stateLayerColor = spec->iconColor;
        spec->focusRingColor =
            colors.color(ColorRole::Primary);
        return;
    }
}

ButtonSpec ActionButtonSpecResolver::toButtonSpec(
    const FabSpec& fab,
    bool extended)
{
    ButtonSpec spec;
    spec.containerColor = fab.containerColor;
    spec.labelColor = fab.iconColor;
    spec.iconColor = fab.iconColor;
    spec.disabledContainerColor =
        fab.disabledContainerColor;
    spec.disabledLabelColor =
        fab.disabledIconColor;
    spec.stateLayerColor = fab.stateLayerColor;
    spec.focusRingColor = fab.focusRingColor;
    spec.outlineColor = Qt::transparent;
    spec.disabledOutlineColor = Qt::transparent;

    spec.shapeRole = fab.shapeRole;
    spec.labelTypeRole = fab.labelTypeRole;
    spec.elevationRole = fab.elevationRole;
    spec.motionToken = fab.motionToken;

    spec.touchTarget = fab.touchTarget;
    spec.containerHeight = fab.containerDiameter;
    spec.horizontalPadding = extended ? 20 : 0;
    spec.iconSize = fab.iconSize;
    spec.iconSpacing = extended ? 12 : 0;

    spec.labelFont = fab.labelFont;
    spec.motionStyle = fab.motionStyle;
    spec.shadowColor = fab.shadowColor;
    spec.cornerRadius = fab.cornerRadius;

    spec.hoverStateLayerOpacity =
        fab.hoverStateLayerOpacity;
    spec.focusStateLayerOpacity =
        fab.focusStateLayerOpacity;
    spec.pressStateLayerOpacity =
        fab.pressStateLayerOpacity;
    spec.dragStateLayerOpacity =
        fab.dragStateLayerOpacity;

    spec.elevationStyle = fab.elevationStyle;
    spec.restingElevationProgress =
        fab.restingElevationProgress;
    spec.hoverElevationProgress =
        fab.hoverElevationProgress;
    spec.focusElevationProgress =
        fab.focusElevationProgress;
    spec.pressElevationProgress =
        fab.pressElevationProgress;
    spec.disabledElevationProgress =
        fab.disabledElevationProgress;

    spec.hasResolvedLabelFont =
        fab.hasResolvedLabelFont;
    spec.hasResolvedMotionStyle =
        fab.hasResolvedMotionStyle;
    spec.hasResolvedElevationStyle =
        fab.hasResolvedElevationStyle;

    return spec;
}

ButtonSpec ActionButtonSpecResolver::fabButtonSpec(
    const Theme& theme,
    Density density,
    FabColorVariant variant) const
{
    FabSpec spec = fabSpec(theme, density);
    applyColorVariant(theme, variant, &spec);
    return toButtonSpec(spec, false);
}

ButtonSpec ActionButtonSpecResolver::extendedFabButtonSpec(
    const Theme& theme,
    Density density,
    FabColorVariant variant) const
{
    FabSpec spec = extendedFabSpec(theme, density);
    applyColorVariant(theme, variant, &spec);
    return toButtonSpec(spec, true);
}

} // namespace QtMaterial
