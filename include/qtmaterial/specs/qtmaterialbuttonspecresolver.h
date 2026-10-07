#pragma once

#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialbuttonspec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

/**
 * Resolves immutable specifications for the standard text-button family.
 *
 * Widgets consume this resolver directly. This resolver is the canonical boundary for this component family.
 */
class QTMATERIAL3_SPECS_EXPORT ButtonSpecResolver final
{
public:
    ButtonSpec resolve(
        ButtonVariant variant,
        const Theme& theme,
        Density density = Density::Default) const;

    ButtonSpec textButtonSpec(
        const Theme& theme,
        Density density = Density::Default) const;

    ButtonSpec filledButtonSpec(
        const Theme& theme,
        Density density = Density::Default) const;

    ButtonSpec filledTonalButtonSpec(
        const Theme& theme,
        Density density = Density::Default) const;

    ButtonSpec outlinedButtonSpec(
        const Theme& theme,
        Density density = Density::Default) const;

    ButtonSpec elevatedButtonSpec(
        const Theme& theme,
        Density density = Density::Default) const;

    // Re-resolve concrete theme values after callers change semantic roles/tokens.
    // This keeps widget implementations on the spec-resolver boundary.
    void resolveRuntimeValues(
        const Theme& theme,
        ButtonSpec* spec) const;

private:
    static ButtonSpec baseButtonSpec(Density density) noexcept;
    static int buttonHeightForDensity(Density density) noexcept;
};

} // namespace QtMaterial
