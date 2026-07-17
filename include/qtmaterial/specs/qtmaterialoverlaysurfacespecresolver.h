#pragma once

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialbottomsheetspec.h"
#include "qtmaterial/specs/qtmaterialnavigationdrawerspec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

/**
 * Resolves modal/overlay surface specifications from a Theme snapshot.
 *
 * Widgets consume the concrete values directly after this boundary.
 * SpecFactory delegates here temporarily for source compatibility.
 */
class QTMATERIAL3_SPECS_EXPORT OverlaySurfaceSpecResolver final
{
public:
    NavigationDrawerSpec navigationDrawerSpec(
        const Theme& theme) const;

    BottomSheetSpec bottomSheetSpec(
        const Theme& theme) const;
};

} // namespace QtMaterial
