#pragma once

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialbannerspec.h"
#include "qtmaterial/specs/qtmaterialcardspec.h"
#include "qtmaterial/specs/qtmaterialsnackbarspec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

class QTMATERIAL3_SPECS_EXPORT SurfaceSpecResolver final
{
public:
    BannerSpec bannerSpec(const Theme& theme) const;
    CardSpec cardSpec(const Theme& theme) const;
    SnackbarSpec snackbarSpec(const Theme& theme) const;
};

} // namespace QtMaterial
