#pragma once

#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialchipspec.h"

namespace QtMaterial {

class Theme;

class QTMATERIAL3_SPECS_EXPORT CompactSpecResolver
{
public:
    CompactSpecResolver();
    ~CompactSpecResolver();

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

private:
    ChipSpec baseChipSpec(
        const Theme& theme,
        Density density) const;

    void applyVariant(
        ChipVariant variant,
        const Theme& theme,
        ChipSpec* spec) const;

    void applyComponentOverrides(
        ChipVariant variant,
        const Theme& theme,
        ChipSpec* spec) const;

    void resolveRuntimeValues(
        const Theme& theme,
        ChipSpec* spec) const;

    static void normalize(ChipSpec* spec);
};

} // namespace QtMaterial
