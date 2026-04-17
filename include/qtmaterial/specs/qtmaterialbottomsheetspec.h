#pragma once
#include <QColor>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT BottomSheetSpec {
    QColor containerColor;
    QColor contentColor;
    QColor scrimColor;
    ShapeRole shapeRole = ShapeRole::ExtraLarge;
    ElevationRole elevationRole = ElevationRole::Level3;
    MotionToken motionToken = MotionToken::Medium2;
    int topPadding = 12;
};
} // namespace QtMaterial
