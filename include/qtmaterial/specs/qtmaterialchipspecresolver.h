#pragma once

#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialchipspec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

/**
 * Resolves immutable specifications for Assist, Filter, Input and Suggestion
 * chips. SpecFactory delegates here temporarily while callers migrate.
 */
class QTMATERIAL3_SPECS_EXPORT ChipSpecResolver final
{
public:
    ChipSpec resolve(
        ChipVariant variant,
        const Theme& theme,
        Density density = Density::Default) const;

    ChipSpec assistChipSpec(
        const Theme& theme,
        Density density = Density::Default) const;
    ChipSpec filterChipSpec(
        const Theme& theme,
        Density density = Density::Default) const;
    ChipSpec inputChipSpec(
        const Theme& theme,
        Density density = Density::Default) const;
    ChipSpec suggestionChipSpec(
        const Theme& theme,
        Density density = Density::Default) const;
};

} // namespace QtMaterial
