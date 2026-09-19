#pragma once

#include <QtGlobal>

#include "qtmaterial/core/qtmaterialthemecontextbinding.h"
#include "qtmaterial/specs/qtmaterialautocompletepopupspecresolver.h"
#include "qtmaterial/specs/qtmaterialdataspecresolver.h"

namespace QtMaterial {
namespace InputSpecResolution {

inline AutocompletePopupSpec autocompletePopupSpec(
    const Theme& theme)
{
    return AutocompletePopupSpecResolver().autocompletePopupSpec(
        theme);
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
