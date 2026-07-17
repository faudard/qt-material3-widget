#pragma once

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialautocompletepopupspec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

class QTMATERIAL3_SPECS_EXPORT
AutocompletePopupSpecResolver final
{
public:
    AutocompletePopupSpec autocompletePopupSpec(
        const Theme& theme) const;
};

} // namespace QtMaterial
