#include "qtmaterial/specs/qtmaterialspecfactory.h"

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

ChipSpec SpecFactory::assistChipSpec(const Theme& theme, Density density) const
{
    ChipSpec spec;
    spec.variant = ChipVariant::Assist;
    spec.containerColor = theme.colorScheme().color(ColorRole::SurfaceContainerLow);
    spec.selectedContainerColor = theme.colorScheme().color(ColorRole::SecondaryContainer);
    spec.labelColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.selectedLabelColor = theme.colorScheme().color(ColorRole::OnSecondaryContainer);
    spec.iconColor = theme.colorScheme().color(ColorRole::Primary);
    spec.selectedIconColor = spec.selectedLabelColor;
    spec.outlineColor = theme.colorScheme().color(ColorRole::Outline);
    spec.disabledContainerColor = theme.colorScheme().color(ColorRole::SurfaceContainerHigh);
    spec.disabledLabelColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.disabledOutlineColor = theme.colorScheme().color(ColorRole::OutlineVariant);
    spec.stateLayerColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.focusRingColor = theme.colorScheme().color(ColorRole::Primary);
    spec.containerHeight = compactAdjusted(32, density, -4, 4);
    spec.touchTarget = QSize(48, qMax(48, spec.containerHeight));
    return spec;
}

ChipSpec SpecFactory::filterChipSpec(const Theme& theme, Density density) const
{
    ChipSpec spec = assistChipSpec(theme, density);
    spec.variant = ChipVariant::Filter;
    spec.containerColor = Qt::transparent;
    spec.selectedContainerColor = theme.colorScheme().color(ColorRole::SecondaryContainer);
    spec.labelColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.selectedLabelColor = theme.colorScheme().color(ColorRole::OnSecondaryContainer);
    spec.stateLayerColor = spec.labelColor;
    return spec;
}

ChipSpec SpecFactory::inputChipSpec(const Theme& theme, Density density) const
{
    ChipSpec spec = assistChipSpec(theme, density);
    spec.variant = ChipVariant::Input;
    spec.containerColor = Qt::transparent;
    spec.selectedContainerColor = theme.colorScheme().color(ColorRole::SurfaceContainerHighest);
    spec.iconColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    return spec;
}

ChipSpec SpecFactory::suggestionChipSpec(const Theme& theme, Density density) const
{
    ChipSpec spec = assistChipSpec(theme, density);
    spec.variant = ChipVariant::Suggestion;
    spec.containerColor = Qt::transparent;
    spec.selectedContainerColor = theme.colorScheme().color(ColorRole::PrimaryContainer);
    spec.selectedLabelColor = theme.colorScheme().color(ColorRole::OnPrimaryContainer);
    spec.selectedIconColor = spec.selectedLabelColor;
    return spec;
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

NavigationRailSpec SpecFactory::navigationRailSpec(const Theme& theme) const
{
    NavigationRailSpec spec;
    spec.containerColor = theme.colorScheme().color(ColorRole::Surface);
    spec.indicatorColor = theme.colorScheme().color(ColorRole::SecondaryContainer);
    spec.selectedIconColor = theme.colorScheme().color(ColorRole::OnSecondaryContainer);
    spec.selectedLabelColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.unselectedIconColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.unselectedLabelColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.stateLayerColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.focusRingColor = theme.colorScheme().color(ColorRole::Primary);
    spec.dividerColor = theme.colorScheme().color(ColorRole::OutlineVariant);
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
