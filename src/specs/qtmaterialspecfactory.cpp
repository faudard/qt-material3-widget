#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/specs/qtmaterialdataspecresolver.h"
#include "qtmaterial/specs/qtmaterialselectionspecresolver.h"
#include "qtmaterial/specs/qtmaterialbuttonspecresolver.h"
#include "qtmaterialcomponenttokenapplier_p.h"
#include "qtmaterial/specs/qtmaterialsurfacespecresolver.h"
#include "qtmaterial/specs/qtmaterialoverlaysurfacespecresolver.h"
#include "qtmaterial/specs/qtmaterialappbarspecresolver.h"
#include "qtmaterial/specs/qtmaterialdialogspecresolver.h"
#include "qtmaterial/specs/qtmaterialactionbuttonspecresolver.h"
#include "qtmaterial/specs/qtmaterialtextfieldspecresolver.h"
#include "qtmaterial/specs/qtmaterialautocompletepopupspecresolver.h"
#include "qtmaterial/specs/qtmaterialdatefieldspecresolver.h"
#include "qtmaterial/specs/qtmaterialnavigationrailspecresolver.h"

namespace QtMaterial {
SpecFactory::SpecFactory() = default;
SpecFactory::~SpecFactory() = default;


ButtonSpec SpecFactory::textButtonSpec(
    const Theme& theme,
    Density density) const
{
    return ButtonSpecResolver().textButtonSpec(theme, density);
}


ButtonSpec SpecFactory::filledButtonSpec(
    const Theme& theme,
    Density density) const
{
    return ButtonSpecResolver().filledButtonSpec(theme, density);
}


ButtonSpec SpecFactory::filledTonalButtonSpec(
    const Theme& theme,
    Density density) const
{
    return ButtonSpecResolver().filledTonalButtonSpec(theme, density);
}


ButtonSpec SpecFactory::outlinedButtonSpec(
    const Theme& theme,
    Density density) const
{
    return ButtonSpecResolver().outlinedButtonSpec(theme, density);
}


ButtonSpec SpecFactory::elevatedButtonSpec(
    const Theme& theme,
    Density density) const
{
    return ButtonSpecResolver().elevatedButtonSpec(theme, density);
}


FabSpec SpecFactory::fabSpec(
    const Theme& theme,
    Density density) const
{
    return ActionButtonSpecResolver().fabSpec(
        theme,
        density);
}

FabSpec SpecFactory::extendedFabSpec(
    const Theme& theme,
    Density density) const
{
    return ActionButtonSpecResolver().extendedFabSpec(
        theme,
        density);
}

IconButtonSpec SpecFactory::iconButtonSpec(
    const Theme& theme,
    Density density) const
{
    return ActionButtonSpecResolver().iconButtonSpec(
        theme,
        density);
}

CheckboxSpec SpecFactory::checkboxSpec(
    const Theme& theme,
    Density density) const
{
    return SelectionSpecResolver().checkboxSpec(theme, density);
}

RadioButtonSpec SpecFactory::radioButtonSpec(
    const Theme& theme,
    Density density) const
{
    return SelectionSpecResolver().radioButtonSpec(theme, density);
}

SwitchSpec SpecFactory::switchSpec(
    const Theme& theme,
    Density density) const
{
    return SelectionSpecResolver().switchSpec(theme, density);
}

DialogSpec SpecFactory::dialogSpec(const Theme& theme) const
{
    return DialogSpecResolver().dialogSpec(theme);
}

NavigationRailSpec
SpecFactory::navigationRailSpec(
    const Theme& theme) const
{
    return NavigationRailSpecResolver()
        .navigationRailSpec(theme);
}

NavigationDrawerSpec
SpecFactory::navigationDrawerSpec(
    const Theme& theme) const
{
    return OverlaySurfaceSpecResolver()
        .navigationDrawerSpec(theme);
}

BottomSheetSpec SpecFactory::bottomSheetSpec(
    const Theme& theme) const
{
    return OverlaySurfaceSpecResolver()
        .bottomSheetSpec(theme);
}

BannerSpec SpecFactory::bannerSpec(
    const Theme& theme) const
{
    return SurfaceSpecResolver().bannerSpec(theme);
}

CardSpec SpecFactory::cardSpec(
    const Theme& theme) const
{
    return SurfaceSpecResolver().cardSpec(theme);
}

AppBarSpec SpecFactory::topAppBarSpec(
    const Theme& theme) const
{
    return AppBarSpecResolver().topAppBarSpec(theme);
}

AppBarSpec SpecFactory::bottomAppBarSpec(
    const Theme& theme) const
{
    return AppBarSpecResolver().bottomAppBarSpec(theme);
}

TextFieldSpec
SpecFactory::outlinedTextFieldSpec(
    const Theme& theme,
    Density density) const
{
    return TextFieldSpecResolver()
        .outlinedTextFieldSpec(theme, density);
}

TextFieldSpec
SpecFactory::filledTextFieldSpec(
    const Theme& theme,
    Density density) const
{
    return TextFieldSpecResolver()
        .filledTextFieldSpec(theme, density);
}

AutocompletePopupSpec
SpecFactory::autocompletePopupSpec(
    const Theme& theme) const
{
    return AutocompletePopupSpecResolver()
        .autocompletePopupSpec(theme);
}

DateFieldSpec SpecFactory::dateFieldSpec(
    const Theme& theme) const
{
    return DateFieldSpecResolver()
        .dateFieldSpec(theme);
}

ListItemSpec SpecFactory::listItemSpec(
    const Theme& theme) const
{
    return DataSpecResolver().listItemSpec(theme);
}

SnackbarSpec SpecFactory::snackbarSpec(
    const Theme& theme) const
{
    return SurfaceSpecResolver().snackbarSpec(theme);
}

DividerSpec SpecFactory::dividerSpec(
    const Theme& theme) const
{
    return DataSpecResolver().dividerSpec(theme);
}
} // namespace QtMaterial
