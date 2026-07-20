#include "qtmaterial/specs/qtmaterialmenuspecresolver.h"

#include "private/qtmaterialdensitymetrics_p.h"
#include "qtmaterialcomponenttokenapplier_p.h"

namespace QtMaterial {

MenuSpec MenuSpecResolver::menuSpec(
    const Theme& theme,
    Density density) const
{
    MenuSpec spec;
    spec.containerColor =
        theme.colorScheme().color(ColorRole::SurfaceContainer);
    spec.itemLabelColor =
        theme.colorScheme().color(ColorRole::OnSurface);
    spec.itemIconColor =
        theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.disabledItemLabelColor =
        theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.disabledItemIconColor =
        theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.stateLayerColor =
        theme.colorScheme().color(ColorRole::OnSurface);
    spec.focusRingColor =
        theme.colorScheme().color(ColorRole::Primary);
    spec.dividerColor =
        theme.colorScheme().color(ColorRole::OutlineVariant);
    spec.shortcutColor =
        theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.disabledShortcutColor =
        theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.checkedIndicatorColor =
        theme.colorScheme().color(ColorRole::Primary);

    spec.minItemSize.setHeight(
        SpecsPrivate::adjustedMetric(48, density, -8, 4));

    applyMenuComponentTokens(
        theme,
        QStringList{
            QStringLiteral("menu"),
            QStringLiteral("Menu")},
        &spec);
    return spec;
}

} // namespace QtMaterial
