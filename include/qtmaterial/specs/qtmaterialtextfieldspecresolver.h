#pragma once

#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialtextfieldspec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

/**
 * Resolves Material text-field variants from a Theme snapshot.
 *
 * Common tokens and variant tokens are applied separately so an Outlined
 * override cannot leak into Filled TextField resolution.
 */
class QTMATERIAL3_SPECS_EXPORT TextFieldSpecResolver final
{
public:
    TextFieldSpec outlinedTextFieldSpec(
        const Theme& theme,
        Density density = Density::Default) const;

    TextFieldSpec filledTextFieldSpec(
        const Theme& theme,
        Density density = Density::Default) const;

private:
    static TextFieldSpec baseTextFieldSpec(
        const Theme& theme,
        Density density);
};

} // namespace QtMaterial
