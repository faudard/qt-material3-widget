#pragma once
#include <QColor>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT DatePickerSpec {
    QColor containerColor;
    QColor headerColor;
    QColor dayTextColor;
    ShapeRole shapeRole = ShapeRole::ExtraLarge;
    ElevationRole elevationRole = ElevationRole::Level3;
    MotionToken motionToken = MotionToken::Medium2;
    int dayCellSize = 40;
};
}
