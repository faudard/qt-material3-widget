#pragma once

#include <QtGlobal>

#include "qtmaterial/core/private/qtmaterialthemecontextbinding_p.h"
#include "qtmaterial/specs/qtmaterialbuttonspecresolver.h"

namespace QtMaterial {
namespace ButtonSpecResolution {

inline ButtonSpec buttonSpec(
    const QtMaterialThemeContextBinding* binding,
    ButtonVariant variant,
    Density density)
{
    Q_ASSERT(binding);
    ButtonSpecResolver resolver;
    ButtonSpec spec = resolver.resolve(
        variant,
        binding->theme(),
        density);
    resolver.resolveRuntimeValues(
        binding->theme(),
        &spec);
    return spec;
}

} // namespace ButtonSpecResolution
} // namespace QtMaterial
