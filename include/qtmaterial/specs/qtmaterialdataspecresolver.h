#pragma once

#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialdividerspec.h"
#include "qtmaterial/specs/qtmateriallistitemspec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

/**
 * Resolves the first data-component specs against a Theme snapshot.
 *
 * Widgets consume the returned values without consulting Theme, QPalette or
 * component overrides during layout and painting.
 */
class QTMATERIAL3_SPECS_EXPORT DataSpecResolver final
{
public:
    ListItemSpec listItemSpec(
        const Theme& theme,
        Density density = Density::Default) const;

    DividerSpec dividerSpec(
        const Theme& theme) const;
};

} // namespace QtMaterial
