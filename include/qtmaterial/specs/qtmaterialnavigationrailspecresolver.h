#pragma once

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialnavigationrailspec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

class QTMATERIAL3_SPECS_EXPORT
NavigationRailSpecResolver final
{
public:
    NavigationRailSpec navigationRailSpec(
        const Theme& theme) const;
};

} // namespace QtMaterial
