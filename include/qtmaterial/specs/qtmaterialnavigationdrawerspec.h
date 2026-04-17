#pragma once
#include <QColor>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT NavigationDrawerSpec {
    QColor containerColor;
    QColor contentColor;
    QColor scrimColor;
    ShapeRole shapeRole = ShapeRole::Large;
    ElevationRole elevationRole = ElevationRole::Level1;
    MotionToken motionToken = MotionToken::Medium2;
    int preferredWidth = 360;
};
} // namespace QtMaterial
