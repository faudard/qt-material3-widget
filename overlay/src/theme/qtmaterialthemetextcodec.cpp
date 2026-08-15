#include "qtmaterial/theme/qtmaterialthemetextcodec.h"
#include "qtmaterial/theme/qtmaterialtokenids.h"

#include <algorithm>

namespace QtMaterial {
namespace {
struct ComponentNamePair { ComponentId id; const char* canonical; };
static const ComponentNamePair kCanonicalComponents[] = {
    { ComponentId::Button, "button" },
    { ComponentId::ButtonText, "button.text" },
    { ComponentId::ButtonFilled, "button.filled" },
    { ComponentId::ButtonFilledTonal, "button.filledTonal" },
    { ComponentId::ButtonOutlined, "button.outlined" },
    { ComponentId::ButtonElevated, "button.elevated" },
    { ComponentId::IconButton, "iconButton" },
    { ComponentId::IconButtonStandard, "iconButton.standard" },
    { ComponentId::IconButtonFilled, "iconButton.filled" },
    { ComponentId::IconButtonFilledTonal, "iconButton.filledTonal" },
    { ComponentId::IconButtonOutlined, "iconButton.outlined" },
    { ComponentId::FloatingActionButton, "fab" },
    { ComponentId::SmallFloatingActionButton, "fab.small" },
    { ComponentId::LargeFloatingActionButton, "fab.large" },
    { ComponentId::ExtendedFloatingActionButton, "fab.extended" },
    { ComponentId::Checkbox, "checkbox" },
    { ComponentId::RadioButton, "radioButton" },
    { ComponentId::Switch, "switch" },
    { ComponentId::Dialog, "dialog" },
    { ComponentId::NavigationDrawer, "navigationDrawer" },
    { ComponentId::BottomSheet, "bottomSheet" },
    { ComponentId::Banner, "banner" },
    { ComponentId::Card, "card" },
    { ComponentId::TopAppBar, "topAppBar" },
    { ComponentId::BottomAppBar, "bottomAppBar" },
    { ComponentId::Input, "input" },
    { ComponentId::TextField, "textField" },
    { ComponentId::TextFieldOutlined, "textField.outlined" },
    { ComponentId::TextFieldFilled, "textField.filled" },
    { ComponentId::AutoComplete, "autocomplete" },
    { ComponentId::AutoCompletePopup, "autocompletePopup" },
    { ComponentId::DateField, "dateField" },
    { ComponentId::List, "list" },
    { ComponentId::ListItem, "listItem" },
    { ComponentId::Divider, "divider" },
    { ComponentId::Tabs, "tabs" },
    { ComponentId::Snackbar, "snackbar" },
    { ComponentId::ProgressIndicator, "progressIndicator" },
    { ComponentId::NavigationRail, "navigationRail" },
    { ComponentId::Surface, "surface" },
    { ComponentId::Data, "data" },
    { ComponentId::Table, "table" },
    { ComponentId::GridList, "gridList" },
    { ComponentId::Carousel, "carousel" },
    { ComponentId::DatePicker, "datePicker" },
    { ComponentId::Navigation, "navigation" },
    { ComponentId::Selection, "selection" },
    { ComponentId::Chip, "chip" },
    { ComponentId::AssistChip, "chip.assist" },
    { ComponentId::FilterChip, "chip.filter" },
    { ComponentId::InputChip, "chip.input" },
    { ComponentId::SuggestionChip, "chip.suggestion" },
    { ComponentId::Menu, "menu" },
    { ComponentId::SegmentedButton, "segmentedButton" },
    { ComponentId::Custom, "custom" },
};
struct ComponentAliasPair { const char* alias; ComponentId id; };
static const ComponentAliasPair kComponentAliases[] = {
    { "button", ComponentId::Button },
    { "Button", ComponentId::Button },
    { "button.text", ComponentId::ButtonText },
    { "TextButton", ComponentId::ButtonText },
    { "button.filled", ComponentId::ButtonFilled },
    { "FilledButton", ComponentId::ButtonFilled },
    { "button.filledTonal", ComponentId::ButtonFilledTonal },
    { "FilledTonalButton", ComponentId::ButtonFilledTonal },
    { "button.outlined", ComponentId::ButtonOutlined },
    { "OutlinedButton", ComponentId::ButtonOutlined },
    { "button.elevated", ComponentId::ButtonElevated },
    { "ElevatedButton", ComponentId::ButtonElevated },
    { "iconButton", ComponentId::IconButton },
    { "IconButton", ComponentId::IconButton },
    { "iconButton.standard", ComponentId::IconButtonStandard },
    { "StandardIconButton", ComponentId::IconButtonStandard },
    { "iconButton.filled", ComponentId::IconButtonFilled },
    { "FilledIconButton", ComponentId::IconButtonFilled },
    { "iconButton.filledTonal", ComponentId::IconButtonFilledTonal },
    { "FilledTonalIconButton", ComponentId::IconButtonFilledTonal },
    { "iconButton.outlined", ComponentId::IconButtonOutlined },
    { "OutlinedIconButton", ComponentId::IconButtonOutlined },
    { "fab", ComponentId::FloatingActionButton },
    { "Fab", ComponentId::FloatingActionButton },
    { "floatingActionButton", ComponentId::FloatingActionButton },
    { "FloatingActionButton", ComponentId::FloatingActionButton },
    { "fab.small", ComponentId::SmallFloatingActionButton },
    { "SmallFab", ComponentId::SmallFloatingActionButton },
    { "fab.large", ComponentId::LargeFloatingActionButton },
    { "LargeFab", ComponentId::LargeFloatingActionButton },
    { "fab.extended", ComponentId::ExtendedFloatingActionButton },
    { "ExtendedFab", ComponentId::ExtendedFloatingActionButton },
    { "ExtendedFloatingActionButton", ComponentId::ExtendedFloatingActionButton },
    { "checkbox", ComponentId::Checkbox },
    { "Checkbox", ComponentId::Checkbox },
    { "radio", ComponentId::RadioButton },
    { "radioButton", ComponentId::RadioButton },
    { "RadioButton", ComponentId::RadioButton },
    { "switch", ComponentId::Switch },
    { "Switch", ComponentId::Switch },
    { "dialog", ComponentId::Dialog },
    { "Dialog", ComponentId::Dialog },
    { "navigationDrawer", ComponentId::NavigationDrawer },
    { "NavigationDrawer", ComponentId::NavigationDrawer },
    { "bottomSheet", ComponentId::BottomSheet },
    { "BottomSheet", ComponentId::BottomSheet },
    { "banner", ComponentId::Banner },
    { "Banner", ComponentId::Banner },
    { "card", ComponentId::Card },
    { "Card", ComponentId::Card },
    { "topAppBar", ComponentId::TopAppBar },
    { "TopAppBar", ComponentId::TopAppBar },
    { "bottomAppBar", ComponentId::BottomAppBar },
    { "BottomAppBar", ComponentId::BottomAppBar },
    { "input", ComponentId::Input },
    { "textField", ComponentId::TextField },
    { "TextField", ComponentId::TextField },
    { "textField.outlined", ComponentId::TextFieldOutlined },
    { "OutlinedTextField", ComponentId::TextFieldOutlined },
    { "textField.filled", ComponentId::TextFieldFilled },
    { "FilledTextField", ComponentId::TextFieldFilled },
    { "autocomplete", ComponentId::AutoComplete },
    { "Autocomplete", ComponentId::AutoComplete },
    { "AutoComplete", ComponentId::AutoComplete },
    { "autocompletePopup", ComponentId::AutoCompletePopup },
    { "AutocompletePopup", ComponentId::AutoCompletePopup },
    { "dateField", ComponentId::DateField },
    { "DateField", ComponentId::DateField },
    { "list", ComponentId::List },
    { "List", ComponentId::List },
    { "listItem", ComponentId::ListItem },
    { "ListItem", ComponentId::ListItem },
    { "divider", ComponentId::Divider },
    { "Divider", ComponentId::Divider },
    { "tabs", ComponentId::Tabs },
    { "Tabs", ComponentId::Tabs },
    { "snackbar", ComponentId::Snackbar },
    { "Snackbar", ComponentId::Snackbar },
    { "progressIndicator", ComponentId::ProgressIndicator },
    { "ProgressIndicator", ComponentId::ProgressIndicator },
    { "navigationRail", ComponentId::NavigationRail },
    { "NavigationRail", ComponentId::NavigationRail },
    { "surface", ComponentId::Surface },
    { "Surface", ComponentId::Surface },
    { "data", ComponentId::Data },
    { "table", ComponentId::Table },
    { "Table", ComponentId::Table },
    { "gridList", ComponentId::GridList },
    { "GridList", ComponentId::GridList },
    { "carousel", ComponentId::Carousel },
    { "Carousel", ComponentId::Carousel },
    { "datePicker", ComponentId::DatePicker },
    { "DatePicker", ComponentId::DatePicker },
    { "navigation", ComponentId::Navigation },
    { "chip", ComponentId::Chip },
    { "Chip", ComponentId::Chip },
    { "chip.assist", ComponentId::AssistChip },
    { "AssistChip", ComponentId::AssistChip },
    { "chip.filter", ComponentId::FilterChip },
    { "FilterChip", ComponentId::FilterChip },
    { "chip.input", ComponentId::InputChip },
    { "InputChip", ComponentId::InputChip },
    { "chip.suggestion", ComponentId::SuggestionChip },
    { "SuggestionChip", ComponentId::SuggestionChip },
    { "menu", ComponentId::Menu },
    { "Menu", ComponentId::Menu },
    { "segmentedButton", ComponentId::SegmentedButton },
    { "SegmentedButton", ComponentId::SegmentedButton },
};
struct TokenNamePair { TokenId id; const char* canonical; };
static const TokenNamePair kTokenNames[] = {
    { tokenId(ColorRole::Primary), "Primary" },
    { tokenId(ColorRole::OnPrimary), "OnPrimary" },
    { tokenId(ColorRole::PrimaryContainer), "PrimaryContainer" },
    { tokenId(ColorRole::OnPrimaryContainer), "OnPrimaryContainer" },
    { tokenId(ColorRole::PrimaryFixed), "PrimaryFixed" },
    { tokenId(ColorRole::PrimaryFixedDim), "PrimaryFixedDim" },
    { tokenId(ColorRole::OnPrimaryFixed), "OnPrimaryFixed" },
    { tokenId(ColorRole::OnPrimaryFixedVariant), "OnPrimaryFixedVariant" },
    { tokenId(ColorRole::Secondary), "Secondary" },
    { tokenId(ColorRole::OnSecondary), "OnSecondary" },
    { tokenId(ColorRole::SecondaryContainer), "SecondaryContainer" },
    { tokenId(ColorRole::OnSecondaryContainer), "OnSecondaryContainer" },
    { tokenId(ColorRole::SecondaryFixed), "SecondaryFixed" },
    { tokenId(ColorRole::SecondaryFixedDim), "SecondaryFixedDim" },
    { tokenId(ColorRole::OnSecondaryFixed), "OnSecondaryFixed" },
    { tokenId(ColorRole::OnSecondaryFixedVariant), "OnSecondaryFixedVariant" },
    { tokenId(ColorRole::Tertiary), "Tertiary" },
    { tokenId(ColorRole::OnTertiary), "OnTertiary" },
    { tokenId(ColorRole::TertiaryContainer), "TertiaryContainer" },
    { tokenId(ColorRole::OnTertiaryContainer), "OnTertiaryContainer" },
    { tokenId(ColorRole::TertiaryFixed), "TertiaryFixed" },
    { tokenId(ColorRole::TertiaryFixedDim), "TertiaryFixedDim" },
    { tokenId(ColorRole::OnTertiaryFixed), "OnTertiaryFixed" },
    { tokenId(ColorRole::OnTertiaryFixedVariant), "OnTertiaryFixedVariant" },
    { tokenId(ColorRole::Error), "Error" },
    { tokenId(ColorRole::OnError), "OnError" },
    { tokenId(ColorRole::ErrorContainer), "ErrorContainer" },
    { tokenId(ColorRole::OnErrorContainer), "OnErrorContainer" },
    { tokenId(ColorRole::Background), "Background" },
    { tokenId(ColorRole::OnBackground), "OnBackground" },
    { tokenId(ColorRole::Surface), "Surface" },
    { tokenId(ColorRole::OnSurface), "OnSurface" },
    { tokenId(ColorRole::SurfaceDim), "SurfaceDim" },
    { tokenId(ColorRole::SurfaceBright), "SurfaceBright" },
    { tokenId(ColorRole::SurfaceContainerLowest), "SurfaceContainerLowest" },
    { tokenId(ColorRole::SurfaceContainerLow), "SurfaceContainerLow" },
    { tokenId(ColorRole::SurfaceContainer), "SurfaceContainer" },
    { tokenId(ColorRole::SurfaceContainerHigh), "SurfaceContainerHigh" },
    { tokenId(ColorRole::SurfaceContainerHighest), "SurfaceContainerHighest" },
    { tokenId(ColorRole::SurfaceVariant), "SurfaceVariant" },
    { tokenId(ColorRole::OnSurfaceVariant), "OnSurfaceVariant" },
    { tokenId(ColorRole::SurfaceTint), "SurfaceTint" },
    { tokenId(ColorRole::Outline), "Outline" },
    { tokenId(ColorRole::OutlineVariant), "OutlineVariant" },
    { tokenId(ColorRole::InverseSurface), "InverseSurface" },
    { tokenId(ColorRole::InverseOnSurface), "InverseOnSurface" },
    { tokenId(ColorRole::InversePrimary), "InversePrimary" },
    { tokenId(ColorRole::Shadow), "Shadow" },
    { tokenId(ColorRole::Scrim), "Scrim" },
    { tokenId(TypeRole::DisplayLarge), "DisplayLarge" },
    { tokenId(TypeRole::DisplayMedium), "DisplayMedium" },
    { tokenId(TypeRole::DisplaySmall), "DisplaySmall" },
    { tokenId(TypeRole::HeadlineLarge), "HeadlineLarge" },
    { tokenId(TypeRole::HeadlineMedium), "HeadlineMedium" },
    { tokenId(TypeRole::HeadlineSmall), "HeadlineSmall" },
    { tokenId(TypeRole::TitleLarge), "TitleLarge" },
    { tokenId(TypeRole::TitleMedium), "TitleMedium" },
    { tokenId(TypeRole::TitleSmall), "TitleSmall" },
    { tokenId(TypeRole::BodyLarge), "BodyLarge" },
    { tokenId(TypeRole::BodyMedium), "BodyMedium" },
    { tokenId(TypeRole::BodySmall), "BodySmall" },
    { tokenId(TypeRole::LabelLarge), "LabelLarge" },
    { tokenId(TypeRole::LabelMedium), "LabelMedium" },
    { tokenId(TypeRole::LabelSmall), "LabelSmall" },
    { tokenId(ShapeRole::None), "None" },
    { tokenId(ShapeRole::ExtraSmall), "ExtraSmall" },
    { tokenId(ShapeRole::Small), "Small" },
    { tokenId(ShapeRole::Medium), "Medium" },
    { tokenId(ShapeRole::Large), "Large" },
    { tokenId(ShapeRole::ExtraLarge), "ExtraLarge" },
    { tokenId(ShapeRole::Full), "Full" },
    { tokenId(ElevationRole::Level0), "Level0" },
    { tokenId(ElevationRole::Level1), "Level1" },
    { tokenId(ElevationRole::Level2), "Level2" },
    { tokenId(ElevationRole::Level3), "Level3" },
    { tokenId(ElevationRole::Level4), "Level4" },
    { tokenId(ElevationRole::Level5), "Level5" },
    { tokenId(MotionToken::Short1), "Short1" },
    { tokenId(MotionToken::Short2), "Short2" },
    { tokenId(MotionToken::Short3), "Short3" },
    { tokenId(MotionToken::Short4), "Short4" },
    { tokenId(MotionToken::Medium1), "Medium1" },
    { tokenId(MotionToken::Medium2), "Medium2" },
    { tokenId(MotionToken::Medium3), "Medium3" },
    { tokenId(MotionToken::Medium4), "Medium4" },
    { tokenId(MotionToken::Long1), "Long1" },
    { tokenId(MotionToken::Long2), "Long2" },
    { tokenId(MotionToken::Long3), "Long3" },
    { tokenId(MotionToken::Long4), "Long4" },
    { tokenId(StateLayerRole::Hover), "Hover" },
    { tokenId(StateLayerRole::Focus), "Focus" },
    { tokenId(StateLayerRole::Press), "Press" },
    { tokenId(StateLayerRole::Drag), "Drag" },
    { tokenId(DensityRole::Compact), "Compact" },
    { tokenId(DensityRole::Default), "Default" },
    { tokenId(DensityRole::Comfortable), "Comfortable" },
    { tokenId(IconSizeRole::ExtraSmall), "ExtraSmall" },
    { tokenId(IconSizeRole::Small), "Small" },
    { tokenId(IconSizeRole::Medium), "Medium" },
    { tokenId(IconSizeRole::Large), "Large" },
    { tokenId(IconSizeRole::ExtraLarge), "ExtraLarge" },
    { tokenId(AccessibilityRole::HighContrast), "highContrast" },
    { tokenId(AccessibilityRole::ReducedMotion), "reducedMotion" },
    { tokenId(AccessibilityRole::MinimumTextContrastRatio), "minimumTextContrastRatio" },
    { tokenId(AccessibilityRole::MinimumUiContrastRatio), "minimumUiContrastRatio" },
    { tokenId(AccessibilityRole::FocusRingWidth), "focusRing.width" },
    { tokenId(AccessibilityRole::FocusRingOffset), "focusRing.offset" },
    { tokenId(AccessibilityRole::FocusRingRadiusAdjustment), "focusRing.radiusAdjustment" },
    { tokenId(AccessibilityRole::FocusRingColor), "focusRing.color" },
    { tokenId(AccessibilityRole::FocusRingOpacity), "focusRing.opacity" },
    { tokenId(InteractionRole::KeyboardFocusVisible), "keyboardFocusVisible" },
    { tokenId(InteractionRole::StrongFocusIndicators), "strongFocusIndicators" },
    { tokenId(InteractionRole::HoverFeedbackEnabled), "hoverFeedbackEnabled" },
    { tokenId(InteractionRole::PressFeedbackEnabled), "pressFeedbackEnabled" },
    { tokenId(InteractionRole::DragFeedbackEnabled), "dragFeedbackEnabled" },
};
} // namespace

QString ThemeTextCodec::componentIdToString(ComponentId id)
{
    for (const auto& pair : kCanonicalComponents)
        if (pair.id == id) return QString::fromLatin1(pair.canonical);
    return QString();
}

bool ThemeTextCodec::componentIdFromString(const QString& text, ComponentId* outId)
{
    const QString normalized = text.trimmed();
    for (const auto& pair : kComponentAliases) {
        if (normalized.compare(QLatin1String(pair.alias), Qt::CaseInsensitive) == 0) {
            if (outId) *outId = pair.id;
            return true;
        }
    }
    return false;
}

QString ThemeTextCodec::tokenIdToString(TokenId id)
{
    for (const auto& pair : kTokenNames)
        if (pair.id == id) return QString::fromLatin1(pair.canonical);
    return QString();
}

bool ThemeTextCodec::tokenIdFromString(
    TokenCategory category, const QString& text, TokenId* outId)
{
    const QString normalized = text.trimmed();
    for (const auto& pair : kTokenNames) {
        if (pair.id.category() == category
            && normalized.compare(QLatin1String(pair.canonical), Qt::CaseInsensitive) == 0) {
            if (outId) *outId = pair.id;
            return true;
        }
    }
    return false;
}

QStringList ThemeTextCodec::extensionComponentNames(
    const ComponentTokenOverrides& overrides)
{
    QStringList names = overrides.m_extensionOverrides.keys();
    std::sort(names.begin(), names.end());
    return names;
}

ComponentTokenOverride ThemeTextCodec::extensionOverrideFor(
    const ComponentTokenOverrides& overrides, const QString& extensionName)
{
    return overrides.m_extensionOverrides.value(
        extensionName.trimmed(), ComponentTokenOverride{});
}

void ThemeTextCodec::setExtensionOverride(
    ComponentTokenOverrides* overrides, const QString& extensionName,
    const ComponentTokenOverride& overrideTokens)
{
    if (!overrides) return;
    const QString key = extensionName.trimmed();
    if (key.isEmpty()) return;
    if (overrideTokens.isEmpty()) overrides->m_extensionOverrides.remove(key);
    else overrides->m_extensionOverrides.insert(key, overrideTokens);
}

} // namespace QtMaterial
