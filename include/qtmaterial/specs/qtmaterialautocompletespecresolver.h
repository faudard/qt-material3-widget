#pragma once

#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialautocompletespec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

class QTMATERIAL3_SPECS_EXPORT AutocompleteSpecResolver final
{
public:
    AutocompleteSpec autocompleteSpec(
        const Theme& theme,
        Density density = Density::Default) const;
};

} // namespace QtMaterial
