#include "qtmaterial/specs/qtmaterialnavigationrailspecresolver.h"

#include "qtmaterialcomponenttokenapplier_p.h"

namespace QtMaterial {

NavigationRailSpec
NavigationRailSpecResolver::navigationRailSpec(
    const Theme& theme) const
{
    NavigationRailSpec spec;
    spec.containerColor =
        theme.colorScheme().color(ColorRole::Surface);
    spec.indicatorColor =
        theme.colorScheme().color(
            ColorRole::SecondaryContainer);
    spec.selectedIconColor =
        theme.colorScheme().color(
            ColorRole::OnSecondaryContainer);
    spec.selectedLabelColor =
        theme.colorScheme().color(ColorRole::OnSurface);
    spec.unselectedIconColor =
        theme.colorScheme().color(
            ColorRole::OnSurfaceVariant);
    spec.unselectedLabelColor =
        theme.colorScheme().color(
            ColorRole::OnSurfaceVariant);
    spec.disabledIconColor =
        spec.unselectedIconColor;
    spec.disabledLabelColor =
        spec.unselectedLabelColor;
    spec.stateLayerColor =
        theme.colorScheme().color(ColorRole::OnSurface);
    spec.focusRingColor =
        theme.colorScheme().color(ColorRole::Primary);
    spec.dividerColor =
        theme.colorScheme().color(
            ColorRole::OutlineVariant);

    applyNavigationRailComponentTokens(
        theme,
        QStringList{
            QStringLiteral("navigation"),
            QStringLiteral("navigationRail"),
            QStringLiteral("NavigationRail")},
        &spec);
    return spec;
}

} // namespace QtMaterial
