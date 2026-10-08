#pragma once

#include <QtGlobal>

#include "qtmaterial/core/private/qtmaterialthemecontextbinding_p.h"
#include "qtmaterial/specs/qtmaterialsliderspecresolver.h"

namespace QtMaterial {
namespace SliderSpecResolution {

inline SliderSpec sliderSpec(
    const QtMaterialThemeContextBinding* binding,
    Density density)
{
    Q_ASSERT(binding);
    return SliderSpecResolver().sliderSpec(
        binding->theme(),
        density);
}

} // namespace SliderSpecResolution
} // namespace QtMaterial
