#pragma once

#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialsegmentedbuttonspec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

class QTMATERIAL3_SPECS_EXPORT SegmentedButtonSpecResolver final
{
public:
    SegmentedButtonSpec segmentedButtonSpec(
        const Theme& theme,
        Density density = Density::Default) const;
};

} // namespace QtMaterial
