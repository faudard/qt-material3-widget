#pragma once

// Slice 13 additive declarations for qtmaterialspecfactory.h.
// Merge these methods into QtMaterial::SpecFactory if the project keeps one central factory.

#include "qtmaterial/specs/qtmaterialdatacomponentspecs.h"

namespace QtMaterial {

class Theme;
class DensityTokens;

class SpecFactorySlice13Additions {
public:
    static DatePickerSpec datePickerSpec(const Theme& theme);
    static TableSpec tableSpec(const Theme& theme);
    static GridListSpec gridListSpec(const Theme& theme);
    static CarouselSpec carouselSpec(const Theme& theme);
};

} // namespace QtMaterial
