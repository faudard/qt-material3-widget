#include "qtmaterial/specs/qtmaterialappbarspecresolver.h"

#include <QtGlobal>

namespace QtMaterial {

AppBarSpec AppBarSpecResolver::baseAppBarSpec(
    const Theme& theme)
{
    AppBarSpec spec;
    spec.containerColor =
        theme.colorScheme().color(ColorRole::Surface);
    spec.titleColor =
        theme.colorScheme().color(ColorRole::OnSurface);
    spec.navigationIconColor =
        theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.actionIconColor =
        theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.outlineColor =
        theme.colorScheme().color(ColorRole::OutlineVariant);
    spec.focusRingColor =
        theme.colorScheme().color(ColorRole::Primary);
    spec.shadowColor =
        theme.colorScheme().color(ColorRole::Shadow);

    spec.elevationRole = ElevationRole::Level0;
    spec.elevatedElevationRole = ElevationRole::Level2;
    spec.titleTypeRole = TypeRole::TitleLarge;
    spec.contentPadding = QMargins(16, 8, 16, 8);
    spec.preferredHeight = 64;
    spec.minimumWidth = 200;
    spec.preferredWidth = 360;
    spec.actionSlot = 48;
    spec.betweenSpacing = 12;
    spec.fabSlot = 0;
    return spec;
}

void AppBarSpecResolver::resolveRuntimeTokens(
    const Theme& theme,
    AppBarSpec* spec)
{
    if (!spec) {
        return;
    }

    spec->hasResolvedTitleFont = false;
    if (theme.typography().contains(spec->titleTypeRole)) {
        spec->titleFont =
            theme.typography().style(spec->titleTypeRole).font;
        spec->hasResolvedTitleFont = true;
    }

    spec->hasResolvedElevatedElevationStyle = false;
    if (theme.elevations().contains(spec->elevatedElevationRole)) {
        spec->elevatedElevationStyle =
            theme.elevations().style(spec->elevatedElevationRole);
        spec->hasResolvedElevatedElevationStyle = true;
    }
}

AppBarSpec AppBarSpecResolver::topAppBarSpec(
    const Theme& theme) const
{
    AppBarSpec spec = baseAppBarSpec(theme);
    spec.elevationRole = ElevationRole::Level0;
    spec.elevatedElevationRole = ElevationRole::Level2;
    spec.titleTypeRole = TypeRole::TitleLarge;
    spec.preferredHeight = 64;
    spec.minimumWidth = 200;
    spec.fabSlot = 0;
    resolveRuntimeTokens(theme, &spec);
    return spec;
}

AppBarSpec AppBarSpecResolver::bottomAppBarSpec(
    const Theme& theme) const
{
    AppBarSpec spec = baseAppBarSpec(theme);
    spec.elevationRole = ElevationRole::Level2;
    spec.elevatedElevationRole = ElevationRole::Level2;
    spec.titleTypeRole = TypeRole::TitleMedium;
    spec.preferredHeight = 80;
    spec.minimumWidth = 240;
    spec.fabSlot = 64;
    resolveRuntimeTokens(theme, &spec);
    return spec;
}

} // namespace QtMaterial
