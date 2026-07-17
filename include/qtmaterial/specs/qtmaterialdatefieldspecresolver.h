#pragma once

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialdatefieldspec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

/**
 * Resolves DateField-specific affordance tokens.
 *
 * The inherited TextField shell remains resolved by TextFieldSpecResolver;
 * this resolver owns only calendar/clear actions and date placeholder policy.
 */
class QTMATERIAL3_SPECS_EXPORT DateFieldSpecResolver final
{
public:
    DateFieldSpec dateFieldSpec(
        const Theme& theme) const;
};

} // namespace QtMaterial
