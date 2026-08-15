#include "qtmaterial/theme/qtmaterialthemetextcodec.h"

#include <algorithm>

namespace QtMaterial {
namespace {
struct ComponentNamePair { ComponentId id; const char* canonical; };
static const ComponentNamePair kCanonical[] = {
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
struct AliasPair { const char* alias; ComponentId id; };
static const AliasPair kAliases[] = {
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
    { "selection", ComponentId::Selection },
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
} // namespace

QString ThemeTextCodec::componentIdToString(ComponentId id)
{
    for (const auto& pair : kCanonical) {
        if (pair.id == id) return QString::fromLatin1(pair.canonical);
    }
    return QString();
}

bool ThemeTextCodec::componentIdFromString(const QString& text, ComponentId* outId)
{
    const QString normalized = text.trimmed();
    for (const auto& pair : kAliases) {
        if (normalized.compare(QLatin1String(pair.alias), Qt::CaseInsensitive) == 0) {
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
    ComponentTokenOverrides* overrides,
    const QString& extensionName,
    const ComponentTokenOverride& overrideTokens)
{
    if (!overrides) return;
    const QString key = extensionName.trimmed();
    if (key.isEmpty()) return;
    if (overrideTokens.isEmpty()) {
        overrides->m_extensionOverrides.remove(key);
    } else {
        overrides->m_extensionOverrides.insert(key, overrideTokens);
    }
}

} // namespace QtMaterial
