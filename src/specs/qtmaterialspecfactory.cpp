#include "qtmaterial/specs/qtmaterialspecfactory.h"
namespace QtMaterial {
SpecFactory::SpecFactory() = default;
SpecFactory::~SpecFactory() = default;
int SpecFactory::buttonHeightForDensity(Density density) const
{
    switch (density) {
    case Density::Compact: return 36;
    case Density::Comfortable: return 44;
    case Density::Default:
    default: return 40;
    }
}
ButtonSpec SpecFactory::textButtonSpec(const Theme& theme, Density density) const
{
    ButtonSpec spec;
    spec.containerColor = Qt::transparent;
    spec.labelColor = theme.colorScheme().color(ColorRole::Primary);
    spec.iconColor = spec.labelColor;
    spec.disabledContainerColor = Qt::transparent;
    spec.disabledLabelColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.stateLayerColor = theme.colorScheme().color(ColorRole::Primary);
    spec.containerHeight = buttonHeightForDensity(density);
    return spec;
}
ButtonSpec SpecFactory::filledButtonSpec(const Theme& theme, Density density) const
{
    ButtonSpec spec = textButtonSpec(theme, density);
    spec.containerColor = theme.colorScheme().color(ColorRole::Primary);
    spec.labelColor = theme.colorScheme().color(ColorRole::OnPrimary);
    spec.iconColor = spec.labelColor;
    spec.disabledContainerColor = theme.colorScheme().color(ColorRole::SurfaceContainerHigh);
    spec.disabledLabelColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.stateLayerColor = theme.colorScheme().color(ColorRole::OnPrimary);
    spec.elevationRole = ElevationRole::Level1;
    return spec;
}
ButtonSpec SpecFactory::filledTonalButtonSpec(const Theme& theme, Density density) const
{
    ButtonSpec spec = textButtonSpec(theme, density);
    spec.containerColor = theme.colorScheme().color(ColorRole::SecondaryContainer);
    spec.labelColor = theme.colorScheme().color(ColorRole::OnSecondaryContainer);
    spec.iconColor = spec.labelColor;
    spec.stateLayerColor = spec.labelColor;
    return spec;
}
ButtonSpec SpecFactory::outlinedButtonSpec(const Theme& theme, Density density) const
{
    ButtonSpec spec = textButtonSpec(theme, density);
    spec.containerColor = Qt::transparent;
    return spec;
}
ButtonSpec SpecFactory::elevatedButtonSpec(const Theme& theme, Density density) const
{
    ButtonSpec spec = textButtonSpec(theme, density);
    spec.containerColor = theme.colorScheme().color(ColorRole::SurfaceContainerLow);
    spec.labelColor = theme.colorScheme().color(ColorRole::Primary);
    spec.iconColor = spec.labelColor;
    spec.elevationRole = ElevationRole::Level1;
    return spec;
}
FabSpec SpecFactory::fabSpec(const Theme& theme, Density) const
{
    FabSpec spec;
    spec.containerColor = theme.colorScheme().color(ColorRole::PrimaryContainer);
    spec.iconColor = theme.colorScheme().color(ColorRole::OnPrimaryContainer);
    spec.disabledContainerColor = theme.colorScheme().color(ColorRole::SurfaceContainerHigh);
    spec.disabledIconColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.stateLayerColor = spec.iconColor;
    return spec;
}
FabSpec SpecFactory::extendedFabSpec(const Theme& theme, Density density) const
{
    FabSpec spec = fabSpec(theme, density);
    spec.touchTarget = QSize(80, 56);
    return spec;
}
IconButtonSpec SpecFactory::iconButtonSpec(const Theme& theme, Density) const
{
    IconButtonSpec spec;
    spec.containerColor = Qt::transparent;
    spec.iconColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.selectedContainerColor = theme.colorScheme().color(ColorRole::PrimaryContainer);
    spec.selectedIconColor = theme.colorScheme().color(ColorRole::OnPrimaryContainer);
    spec.disabledIconColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.stateLayerColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    return spec;
}
CheckboxSpec SpecFactory::checkboxSpec(const Theme& theme, Density) const
{
    CheckboxSpec spec;
    spec.selectedContainerColor = theme.colorScheme().color(ColorRole::Primary);
    spec.selectedIconColor = theme.colorScheme().color(ColorRole::OnPrimary);
    spec.unselectedOutlineColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.disabledSelectedContainerColor = theme.colorScheme().color(ColorRole::SurfaceContainerHigh);
    spec.disabledUnselectedOutlineColor = theme.colorScheme().color(ColorRole::OutlineVariant);
    spec.labelColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.stateLayerColor = theme.colorScheme().color(ColorRole::Primary);
    spec.focusRingColor = theme.colorScheme().color(ColorRole::Primary);
    return spec;
}
RadioButtonSpec SpecFactory::radioButtonSpec(const Theme& theme, Density) const
{
    RadioButtonSpec spec;
    spec.selectedColor = theme.colorScheme().color(ColorRole::Primary);
    spec.unselectedOutlineColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.disabledColor = theme.colorScheme().color(ColorRole::OutlineVariant);
    spec.labelColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.stateLayerColor = theme.colorScheme().color(ColorRole::Primary);
    spec.focusRingColor = theme.colorScheme().color(ColorRole::Primary);
    return spec;
}
SwitchSpec SpecFactory::switchSpec(const Theme& theme, Density) const
{
    SwitchSpec spec;
    spec.selectedTrackColor = theme.colorScheme().color(ColorRole::Primary);
    spec.selectedHandleColor = theme.colorScheme().color(ColorRole::OnPrimary);
    spec.unselectedTrackColor = theme.colorScheme().color(ColorRole::SurfaceVariant);
    spec.unselectedHandleColor = theme.colorScheme().color(ColorRole::Outline);
    spec.iconColor = theme.colorScheme().color(ColorRole::OnPrimary);
    spec.stateLayerColor = theme.colorScheme().color(ColorRole::Primary);
    return spec;
}
DialogSpec SpecFactory::dialogSpec(const Theme& theme) const
{
    DialogSpec spec;
    spec.containerColor = theme.colorScheme().color(ColorRole::SurfaceContainerHigh);
    spec.headlineColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.bodyColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.scrimColor = theme.colorScheme().color(ColorRole::Scrim);
    return spec;
}
NavigationDrawerSpec SpecFactory::navigationDrawerSpec(const Theme& theme) const
{
    NavigationDrawerSpec spec;
    spec.containerColor = theme.colorScheme().color(ColorRole::SurfaceContainerLow);
    spec.contentColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.scrimColor = theme.colorScheme().color(ColorRole::Scrim);
    return spec;
}
BottomSheetSpec SpecFactory::bottomSheetSpec(const Theme& theme) const
{
    BottomSheetSpec spec;
    spec.containerColor = theme.colorScheme().color(ColorRole::SurfaceContainerHigh);
    spec.contentColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.scrimColor = theme.colorScheme().color(ColorRole::Scrim);
    return spec;
}
BannerSpec SpecFactory::bannerSpec(const Theme& theme) const
{
    BannerSpec spec;
    spec.containerColor = theme.colorScheme().color(ColorRole::SurfaceContainerLow);
    spec.headlineColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.supportingColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.actionColor = theme.colorScheme().color(ColorRole::Primary);
    return spec;
}
CardSpec SpecFactory::cardSpec(const Theme& theme) const
{
    CardSpec spec;
    spec.containerColor = theme.colorScheme().color(ColorRole::SurfaceContainerLow);
    spec.outlineColor = theme.colorScheme().color(ColorRole::OutlineVariant);
    spec.contentColor = theme.colorScheme().color(ColorRole::OnSurface);
    return spec;
}
AppBarSpec SpecFactory::topAppBarSpec(const Theme& theme) const
{
    AppBarSpec spec;
    spec.containerColor = theme.colorScheme().color(ColorRole::Surface);
    spec.titleColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.navigationIconColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.actionIconColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    return spec;
}
AppBarSpec SpecFactory::bottomAppBarSpec(const Theme& theme) const
{
    AppBarSpec spec = topAppBarSpec(theme);
    spec.elevationRole = ElevationRole::Level2;
    return spec;
}
TextFieldSpec SpecFactory::outlinedTextFieldSpec(const Theme& theme, Density) const
{
    TextFieldSpec spec;
    spec.containerColor = Qt::transparent;
    spec.activeIndicatorColor = theme.colorScheme().color(ColorRole::Primary);
    spec.outlineColor = theme.colorScheme().color(ColorRole::Outline);
    spec.focusedOutlineColor = theme.colorScheme().color(ColorRole::Primary);
    spec.inputTextColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.labelColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.supportingTextColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.cursorColor = theme.colorScheme().color(ColorRole::Primary);
    spec.errorColor = theme.colorScheme().color(ColorRole::Error);
    spec.stateLayerColor = theme.colorScheme().color(ColorRole::OnSurface);
    return spec;
}
TextFieldSpec SpecFactory::filledTextFieldSpec(const Theme& theme, Density density) const
{
    TextFieldSpec spec = outlinedTextFieldSpec(theme, density);
    spec.containerColor = theme.colorScheme().color(ColorRole::SurfaceContainerHighest);
    return spec;
}
AutocompletePopupSpec SpecFactory::autocompletePopupSpec(const Theme& theme) const
{
    AutocompletePopupSpec spec;
    spec.containerColor = theme.colorScheme().color(ColorRole::SurfaceContainerHigh);
    spec.textColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.selectedContainerColor = theme.colorScheme().color(ColorRole::SecondaryContainer);
    spec.selectedTextColor = theme.colorScheme().color(ColorRole::OnSecondaryContainer);
    return spec;
}
DateFieldSpec SpecFactory::dateFieldSpec(const Theme& theme) const
{
    DateFieldSpec spec;
    spec.leadingIconColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.trailingIconColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.placeholderColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    return spec;
}
ListItemSpec SpecFactory::listItemSpec(const Theme& theme) const
{
    ListItemSpec spec;
    spec.containerColor = Qt::transparent;
    spec.headlineColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.supportingColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.leadingColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.trailingColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.stateLayerColor = theme.colorScheme().color(ColorRole::OnSurface);
    return spec;
}
DividerSpec SpecFactory::dividerSpec(const Theme& theme) const
{
    DividerSpec spec;
    spec.color = theme.colorScheme().color(ColorRole::OutlineVariant);
    return spec;
}
} // namespace QtMaterial
