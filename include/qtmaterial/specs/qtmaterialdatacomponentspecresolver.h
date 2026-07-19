#pragma once

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialdatacomponentspecs.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

/**
 * Resolves the theme-dependent specifications for higher-level data
 * components. The default*Spec functions remain the theme-free defaults.
 */
class QTMATERIAL3_SPECS_EXPORT DataComponentSpecResolver final
{
public:
    DatePickerSpec datePickerSpec(const Theme& theme) const;
    TableSpec tableSpec(const Theme& theme) const;
    GridListSpec gridListSpec(const Theme& theme) const;
    CarouselSpec carouselSpec(const Theme& theme) const;
};

} // namespace QtMaterial
