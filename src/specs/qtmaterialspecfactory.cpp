#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/specs/qtmaterialselectionspecresolver.h"
#include "qtmaterial/specs/qtmaterialbuttonspecresolver.h"
#include "qtmaterialcomponenttokenapplier_p.h"
#include "qtmaterial/specs/qtmaterialsurfacespecresolver.h"
#include "qtmaterial/specs/qtmaterialoverlaysurfacespecresolver.h"
#include "qtmaterial/specs/qtmaterialappbarspecresolver.h"

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


FabSpec SpecFactory::fabSpec(const Theme& theme, Density) const
{
    FabSpec spec;
    spec.containerColor = theme.colorScheme().color(ColorRole::PrimaryContainer);
    spec.iconColor = theme.colorScheme().color(ColorRole::OnPrimaryContainer);
    spec.disabledContainerColor = theme.colorScheme().color(ColorRole::SurfaceContainerHigh);
    spec.disabledIconColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.stateLayerColor = spec.iconColor;
    applyFabComponentTokens(theme, QStringList{QStringLiteral("fab"), QStringLiteral("Fab")}, &spec);
  return spec;
}

FabSpec SpecFactory::extendedFabSpec(const Theme& theme, Density density) const
{
    FabSpec spec = fabSpec(theme, density);
    spec.touchTarget = QSize(80, 56);
    applyFabComponentTokens(theme, QStringList{QStringLiteral("fab"), QStringLiteral("fab.extended"), QStringLiteral("ExtendedFab")}, &spec);
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
    applyIconButtonComponentTokens(theme, QStringList{QStringLiteral("iconButton"), QStringLiteral("IconButton")}, &spec);
  return spec;
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
    DialogSpec spec;
    spec.containerColor = theme.colorScheme().color(ColorRole::SurfaceContainerHigh);
    spec.headlineColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.bodyColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.scrimColor = theme.colorScheme().color(ColorRole::Scrim);
    applyDialogComponentTokens(theme, QStringList{QStringLiteral("surface"), QStringLiteral("dialog"), QStringLiteral("Dialog")}, &spec);
  return spec;
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

TextFieldSpec SpecFactory::outlinedTextFieldSpec(const Theme& theme, Density density) const
{
    TextFieldSpec spec;
    spec.containerColor = Qt::transparent;
    spec.activeIndicatorColor = theme.colorScheme().color(ColorRole::Primary);
    spec.outlineColor = theme.colorScheme().color(ColorRole::Outline);
    spec.focusedOutlineColor = theme.colorScheme().color(ColorRole::Primary);
    spec.disabledOutlineColor = theme.colorScheme().color(ColorRole::OutlineVariant);
    spec.inputTextColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.disabledInputTextColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.labelColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.disabledLabelColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.supportingTextColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.disabledSupportingTextColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.cursorColor = theme.colorScheme().color(ColorRole::Primary);
    spec.errorColor = theme.colorScheme().color(ColorRole::Error);
    spec.stateLayerColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.focusRingColor = theme.colorScheme().color(ColorRole::Primary);
    switch (density) {
    case Density::Compact:
        spec.minHeight = 52;
        spec.horizontalPadding = 12;
        spec.verticalPadding = 6;
        break;
    case Density::Comfortable:
        spec.minHeight = 60;
        spec.horizontalPadding = 18;
        spec.verticalPadding = 10;
        break;
    case Density::Default:
    default:
        break;
    }
    applyTextFieldComponentTokens(theme, QStringList{QStringLiteral("input"), QStringLiteral("textField"), QStringLiteral("textField.outlined"), QStringLiteral("OutlinedTextField")}, &spec);
  return spec;
}

TextFieldSpec SpecFactory::filledTextFieldSpec(const Theme& theme, Density density) const
{
    TextFieldSpec spec = outlinedTextFieldSpec(theme, density);
    spec.containerColor = theme.colorScheme().color(ColorRole::SurfaceContainerHighest);
    applyTextFieldComponentTokens(theme, QStringList{QStringLiteral("input"), QStringLiteral("textField"), QStringLiteral("textField.filled"), QStringLiteral("FilledTextField")}, &spec);
  return spec;
}

AutocompletePopupSpec SpecFactory::autocompletePopupSpec(const Theme& theme) const
{
    AutocompletePopupSpec spec;
    spec.containerColor = theme.colorScheme().color(ColorRole::SurfaceContainerHigh);
    spec.textColor = theme.colorScheme().color(ColorRole::OnSurface);
    // spec.hoverColor = theme.colorScheme().color(ColorRole::SurfaceVariant);
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
    // spec.supportingColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    // spec.leadingColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    // spec.trailingColor = theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.stateLayerColor = theme.colorScheme().color(ColorRole::OnSurface);
    return spec;
}

SnackbarSpec SpecFactory::snackbarSpec(
    const Theme& theme) const
{
    return SurfaceSpecResolver().snackbarSpec(theme);
}

DividerSpec SpecFactory::dividerSpec(const Theme& theme) const
{
    DividerSpec spec;
    spec.color = theme.colorScheme().color(ColorRole::OutlineVariant);
    spec.insetColor = spec.color;
    spec.shapeRole = ShapeRole::None;
    spec.orientation = Qt::Horizontal;
    spec.margins = QMargins(0, 0, 0, 0);
    spec.thickness = 1;
    spec.leadingInset = 0;
    spec.trailingInset = 0;
    return spec;
}
} // namespace QtMaterial
