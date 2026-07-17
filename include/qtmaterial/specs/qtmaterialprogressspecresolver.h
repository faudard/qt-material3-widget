#pragma once

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialprogressindicatorspec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

/**
 * Resolves a ProgressIndicatorSpec against a Theme snapshot.
 *
 * Invalid requested colors inherit their corresponding theme roles. Concrete
 * requested values remain untouched, allowing a widget to combine local
 * overrides with an inherited ThemeContext.
 */
class QTMATERIAL3_SPECS_EXPORT ProgressSpecResolver final
{
public:
    ProgressIndicatorSpec resolve(
        const Theme& theme,
        const ProgressIndicatorSpec& requested =
            ProgressIndicatorSpec()) const;
};

} // namespace QtMaterial
