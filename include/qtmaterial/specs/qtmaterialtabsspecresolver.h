#pragma once

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialtabsspec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

/**
 * Resolves a TabsSpec against a Theme snapshot.
 *
 * Explicit authored colors and non-negative opacities remain authoritative.
 * Invalid colors and negative opacities inherit Material theme tokens.
 */
class QTMATERIAL3_SPECS_EXPORT TabsSpecResolver final
{
public:
    TabsSpec resolve(
        const Theme& theme,
        const TabsSpec& authored = TabsSpec()) const;
};

} // namespace QtMaterial
