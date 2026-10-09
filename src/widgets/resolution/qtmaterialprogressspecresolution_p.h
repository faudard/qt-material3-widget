#pragma once

#include <QtGlobal>

#include "qtmaterial/core/private/qtmaterialthemecontextbinding_p.h"
#include "qtmaterial/specs/qtmaterialprogressspecresolver.h"

namespace QtMaterial {
namespace ProgressSpecResolution {

inline ProgressIndicatorSpec linearProgressSpec(
    const QtMaterialThemeContextBinding* binding)
{
    Q_ASSERT(binding);
    return ProgressSpecResolver().resolve(binding->theme());
}

inline bool reducedMotion(
    const QtMaterialThemeContextBinding* binding)
{
    Q_ASSERT(binding);
    return binding->theme().accessibility().reducedMotion;
}

} // namespace ProgressSpecResolution
} // namespace QtMaterial
