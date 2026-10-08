#pragma once

#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialsliderspec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

/**
 * Canonical Material slider-spec resolver.
 *
 * The resolver owns theme/density interpretation. QWidget-side code consumes
 * SliderSpec only and must not query Theme directly.
 */
class QTMATERIAL3_SPECS_EXPORT SliderSpecResolver final
{
public:
    SliderSpec sliderSpec(
        const Theme& theme,
        Density density = Density::Default) const;
};

} // namespace QtMaterial
