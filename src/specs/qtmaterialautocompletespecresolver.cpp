#include "qtmaterial/specs/qtmaterialautocompletespecresolver.h"

#include "private/qtmaterialdensitymetrics_p.h"

namespace QtMaterial {

AutocompleteSpec AutocompleteSpecResolver::autocompleteSpec(
    const Theme& theme,
    Density density) const
{
    AutocompleteSpec spec;
    spec.inputContainerColor =
        theme.colorScheme().color(ColorRole::SurfaceContainerHighest);
    spec.popupContainerColor =
        theme.colorScheme().color(ColorRole::SurfaceContainerHigh);
    spec.inputTextColor =
        theme.colorScheme().color(ColorRole::OnSurface);
    spec.suggestionTextColor =
        theme.colorScheme().color(ColorRole::OnSurface);
    spec.placeholderColor =
        theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.selectedSuggestionContainerColor =
        theme.colorScheme().color(ColorRole::SecondaryContainer);
    spec.selectedSuggestionTextColor =
        theme.colorScheme().color(ColorRole::OnSecondaryContainer);
    spec.stateLayerColor =
        theme.colorScheme().color(ColorRole::OnSurface);
    spec.outlineColor =
        theme.colorScheme().color(ColorRole::Outline);
    spec.focusedOutlineColor =
        theme.colorScheme().color(ColorRole::Primary);
    spec.focusRingColor =
        theme.colorScheme().color(ColorRole::Primary);
    spec.disabledTextColor =
        theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.inputMinHeight =
        SpecsPrivate::adjustedMetric(56, density, -8, 4);
    return spec;
}

} // namespace QtMaterial
