#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/specs/qtmaterialcompactspecresolver.h"

namespace QtMaterial {
namespace {

int compactAdjusted(int value, Density density, int compactDelta, int comfortableDelta)
{
    switch (density) {
    case Density::Compact:
        return value + compactDelta;
    case Density::Comfortable:
        return value + comfortableDelta;
    case Density::Default:
    default:
        return value;
    }
}

} // namespace

ChipSpec SpecFactory::assistChipSpec(
    const Theme& theme,
    Density density) const
{
    return CompactSpecResolver()
        .assistChipSpec(theme, density);
}

ChipSpec SpecFactory::filterChipSpec(
    const Theme& theme,
    Density density) const
{
    return CompactSpecResolver()
        .filterChipSpec(theme, density);
}

ChipSpec SpecFactory::inputChipSpec(
    const Theme& theme,
    Density density) const
{
    return CompactSpecResolver()
        .inputChipSpec(theme, density);
}

ChipSpec SpecFactory::suggestionChipSpec(
    const Theme& theme,
    Density density) const
{
    return CompactSpecResolver()
        .suggestionChipSpec(theme, density);
}

SegmentedButtonSpec SpecFactory::segmentedButtonSpec(const Theme& theme, Density density) const
{
    SegmentedButtonSpec spec;
    spec.containerColor = Qt::transparent;
    spec.selectedContainerColor = theme.colorScheme().color(ColorRole::SecondaryContainer);
    spec.labelColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.selectedLabelColor = theme.colorScheme().color(ColorRole::OnSecondaryContainer);
    spec.outlineColor = theme.colorScheme().color(ColorRole::Outline);
    spec.stateLayerColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.focusRingColor = theme.colorScheme().color(ColorRole::Primary);
    spec.disabledLabelColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.disabledOutlineColor = theme.colorScheme().color(ColorRole::OutlineVariant);
    spec.segmentHeight = compactAdjusted(40, density, -4, 4);
    spec.touchTarget = QSize(48, qMax(48, spec.segmentHeight));
    return spec;
}

MenuSpec SpecFactory::menuSpec(const Theme& theme, Density density) const
{
    MenuSpec spec;
    spec.containerColor = theme.colorScheme().color(ColorRole::SurfaceContainer);
    spec.itemLabelColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.itemIconColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.disabledItemLabelColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.stateLayerColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.focusRingColor = theme.colorScheme().color(ColorRole::Primary);
    spec.dividerColor = theme.colorScheme().color(ColorRole::OutlineVariant);
    spec.shortcutColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.minItemSize.setHeight(compactAdjusted(48, density, -8, 4));
    return spec;
}

AutocompleteSpec SpecFactory::autocompleteSpec(const Theme& theme, Density density) const
{
    AutocompleteSpec spec;
    spec.inputContainerColor = theme.colorScheme().color(ColorRole::SurfaceContainerHighest);
    spec.popupContainerColor = theme.colorScheme().color(ColorRole::SurfaceContainerHigh);
    spec.inputTextColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.suggestionTextColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.placeholderColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.selectedSuggestionContainerColor = theme.colorScheme().color(ColorRole::SecondaryContainer);
    spec.selectedSuggestionTextColor = theme.colorScheme().color(ColorRole::OnSecondaryContainer);
    spec.stateLayerColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.outlineColor = theme.colorScheme().color(ColorRole::Outline);
    spec.focusedOutlineColor = theme.colorScheme().color(ColorRole::Primary);
    spec.focusRingColor = theme.colorScheme().color(ColorRole::Primary);
    spec.disabledTextColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.inputMinHeight = compactAdjusted(56, density, -8, 4);
    return spec;
}

} // namespace QtMaterial
