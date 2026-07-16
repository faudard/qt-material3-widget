#pragma once

#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialcheckboxspec.h"
#include "qtmaterial/specs/qtmaterialradiobuttonspec.h"
#include "qtmaterial/specs/qtmaterialswitchspec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

/**
 * Resolves immutable specifications for the standard selection controls.
 *
 * Widgets consume this resolver directly. SpecFactory delegates to it
 * temporarily to preserve source compatibility during the architecture
 * refactor.
 */
class QTMATERIAL3_SPECS_EXPORT SelectionSpecResolver final
{
public:
    CheckboxSpec checkboxSpec(
        const Theme& theme,
        Density density = Density::Default) const;

    RadioButtonSpec radioButtonSpec(
        const Theme& theme,
        Density density = Density::Default) const;

    SwitchSpec switchSpec(
        const Theme& theme,
        Density density = Density::Default) const;

private:
    static int spacingForDensity(
        Density density,
        int defaultSpacing) noexcept;
};

} // namespace QtMaterial
