#pragma once

#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialdatacomponentspecs.h"
#include "qtmaterial/specs/qtmaterialdividerspec.h"
#include "qtmaterial/specs/qtmateriallistitemspec.h"
#include "qtmaterial/specs/qtmateriallistspec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

/**
 * Resolves data-component specs against a Theme snapshot.
 *
 * Widgets consume the returned values without consulting Theme, QPalette or
 * component overrides during layout and painting.
 */
class QTMATERIAL3_SPECS_EXPORT DataSpecResolver final
{
public:
    ListSpec listSpec(
        const Theme& theme,
        Density density = Density::Default) const;

    ListItemSpec listItemSpec(
        const Theme& theme,
        Density density = Density::Default) const;

    DividerSpec dividerSpec(
        const Theme& theme) const;

    GridListSpec gridListSpec(
        const Theme& theme,
        Density density = Density::Default) const;
};

} // namespace QtMaterial
