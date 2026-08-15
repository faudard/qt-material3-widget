#pragma once

#include <QtGlobal>

#include "qtmaterial/core/qtmaterialthemecontextbinding.h"
#include "qtmaterial/specs/qtmaterialdataspecresolver.h"

namespace QtMaterial {
namespace DataSpecResolution {

inline TableSpec tableSpec(
    const QtMaterialThemeContextBinding* binding,
    Density density)
{
    Q_ASSERT(binding);
    return DataSpecResolver().tableSpec(
        binding->theme(),
        density);
}

inline GridListSpec gridListSpec(
    const QtMaterialThemeContextBinding* binding,
    Density density)
{
    Q_ASSERT(binding);
    return DataSpecResolver().gridListSpec(
        binding->theme(),
        density);
}

} // namespace DataSpecResolution
} // namespace QtMaterial
