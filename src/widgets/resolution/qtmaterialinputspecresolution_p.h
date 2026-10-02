#pragma once

#include <QtGlobal>

#include "qtmaterial/core/private/qtmaterialthemecontextbinding_p.h"
#include "qtmaterial/specs/qtmaterialautocompletespecresolver.h"
#include "qtmaterial/specs/qtmaterialautocompletepopupspecresolver.h"
#include "qtmaterial/specs/qtmaterialdataspecresolver.h"

namespace QtMaterial {
namespace InputSpecResolution {

inline AutocompleteSpec autocompleteSpec(
    const QtMaterialThemeContextBinding* binding,
    Density density = Density::Default)
{
    Q_ASSERT(binding);
    return AutocompleteSpecResolver().autocompleteSpec(
        binding->theme(),
        density);
}

inline AutocompletePopupSpec autocompletePopupSpec(
    const Theme& theme)
{
    return AutocompletePopupSpecResolver().autocompletePopupSpec(
        theme);
}

inline AutocompletePopupSpec autocompletePopupSpec(
    const ThemeContext* context)
{
    Q_ASSERT(context);
    return autocompletePopupSpec(context->theme());
}

inline AutocompletePopupSpec autocompletePopupSpec(
    const QtMaterialThemeContextBinding* binding)
{
    Q_ASSERT(binding);
    return autocompletePopupSpec(binding->theme());
}

inline DatePickerSpec datePickerSpec(
    const QtMaterialThemeContextBinding* binding,
    Density density = Density::Default)
{
    Q_ASSERT(binding);
    return DataSpecResolver().datePickerSpec(
        binding->theme(),
        density);
}

} // namespace InputSpecResolution
} // namespace QtMaterial
