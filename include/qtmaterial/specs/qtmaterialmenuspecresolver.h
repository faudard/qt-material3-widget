#pragma once

#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialmenuspec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

class QTMATERIAL3_SPECS_EXPORT MenuSpecResolver final
{
public:
    MenuSpec menuSpec(
        const Theme& theme,
        Density density = Density::Default) const;
};

} // namespace QtMaterial
