#pragma once

#include "qtmaterial/foundation/qtmaterialdensity.h"

namespace QtMaterial {
namespace SpecsPrivate {

inline int adjustedMetric(
    int value,
    Density density,
    int compactDelta,
    int comfortableDelta) noexcept
{
    switch (density) {
    case Density::Compact:
        return value + compactDelta;
    case Density::Comfortable:
        return value + comfortableDelta;
    case Density::Default:
    default:
        return value;
    }
}

} // namespace SpecsPrivate
} // namespace QtMaterial
