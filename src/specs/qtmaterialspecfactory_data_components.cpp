#include "qtmaterial/specs/qtmaterialdatacomponentspecresolver.h"

namespace QtMaterial {

// Transitional compatibility entry points. New code should instantiate
// DataComponentSpecResolver directly.
DatePickerSpec makeDatePickerSpec(const Theme& theme)
{
    return DataComponentSpecResolver().datePickerSpec(theme);
}

TableSpec makeTableSpec(const Theme& theme)
{
    return DataComponentSpecResolver().tableSpec(theme);
}

GridListSpec makeGridListSpec(const Theme& theme)
{
    return DataComponentSpecResolver().gridListSpec(theme);
}

CarouselSpec makeCarouselSpec(const Theme& theme)
{
    return DataComponentSpecResolver().carouselSpec(theme);
}

} // namespace QtMaterial
