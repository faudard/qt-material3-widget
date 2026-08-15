#pragma once

#include <QtGlobal>

#include "qtmaterial/specs/qtmaterialsurfacespecresolver.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"

namespace QtMaterial {
namespace SurfaceSpecResolution {

inline CardSpec cardSpec(ThemeContext* context)
{
    Q_ASSERT(context);
    return SurfaceSpecResolver().cardSpec(context->theme());
}

inline SnackbarSpec snackbarSpec(ThemeContext* context)
{
    Q_ASSERT(context);
    return SurfaceSpecResolver().snackbarSpec(context->theme());
}

} // namespace SurfaceSpecResolution
} // namespace QtMaterial
