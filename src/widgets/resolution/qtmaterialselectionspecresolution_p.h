#pragma once

#include <QtGlobal>

#include "qtmaterial/core/private/qtmaterialthemecontextbinding_p.h"
#include "qtmaterial/specs/qtmaterialselectionspecresolver.h"

namespace QtMaterial {
namespace SelectionSpecResolution {

inline CheckboxSpec checkboxSpec(
    const QtMaterialThemeContextBinding* binding,
    Density density)
{
    Q_ASSERT(binding);
    return SelectionSpecResolver().checkboxSpec(
        binding->theme(),
        density);
}

inline RadioButtonSpec radioButtonSpec(
    const QtMaterialThemeContextBinding* binding,
    Density density)
{
    Q_ASSERT(binding);
    return SelectionSpecResolver().radioButtonSpec(
        binding->theme(),
        density);
}

} // namespace SelectionSpecResolution
} // namespace QtMaterial
