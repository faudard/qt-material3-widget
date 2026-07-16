#pragma once

#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialbuttonspec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

enum class ButtonVariant
{
    Text,
    Filled,
    FilledTonal,
    Outlined,
    Elevated
};

/**
 * Resolves immutable specifications for the standard text-button family.
 *
 * Widgets consume this resolver directly. SpecFactory delegates to it
 * temporarily to preserve the current public API while the architecture is
 * migrated family by family.
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

private:
    static int buttonHeightForDensity(Density density) noexcept;
};

} // namespace QtMaterial
