#pragma once

#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialbuttonspec.h"
#include "qtmaterial/specs/qtmaterialfabspec.h"
#include "qtmaterial/specs/qtmaterialiconbuttonspec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

enum class FabColorVariant
{
    Primary,
    Secondary,
    Tertiary,
    Surface
};

/**
 * Resolves floating-action and icon-button specifications.
 *
 * The resolver also owns the conversion from FabSpec to the ButtonSpec
 * consumed by the FilledButton rendering pipeline.
 */
class QTMATERIAL3_SPECS_EXPORT ActionButtonSpecResolver final
{
public:
    FabSpec fabSpec(
        const Theme& theme,
        Density density = Density::Default) const;

    FabSpec extendedFabSpec(
        const Theme& theme,
        Density density = Density::Default) const;

    IconButtonSpec iconButtonSpec(
        const Theme& theme,
        Density density = Density::Default) const;

    ButtonSpec fabButtonSpec(
        const Theme& theme,
        Density density = Density::Default,
        FabColorVariant variant =
            FabColorVariant::Primary) const;

    ButtonSpec extendedFabButtonSpec(
        const Theme& theme,
        Density density = Density::Default,
        FabColorVariant variant =
            FabColorVariant::Primary) const;

private:
    static void applyColorVariant(
        const Theme& theme,
        FabColorVariant variant,
        FabSpec* spec);

    static ButtonSpec toButtonSpec(
        const FabSpec& fab,
        bool extended);
};

} // namespace QtMaterial
