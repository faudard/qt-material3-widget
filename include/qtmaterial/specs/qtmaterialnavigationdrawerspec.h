#pragma once
#include <QColor>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialelevationscale.h"
#include "qtmaterial/theme/qtmaterialmotiontokens.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT NavigationDrawerSpec {
    QColor containerColor;
    QColor contentColor;
    QColor scrimColor;
    QColor shadowColor;

    qreal scrimOpacity = 0.32;
    qreal cornerRadius = 16.0;
    ElevationStyle elevationStyle;
    MotionStyle motionStyle;

    bool hasResolvedElevationStyle = false;
    bool hasResolvedMotionStyle = false;
    ShapeRole shapeRole = ShapeRole::Large;
    ElevationRole elevationRole = ElevationRole::Level1;
    MotionToken motionToken = MotionToken::Medium2;
    int preferredWidth = 360;
};
} // namespace QtMaterial
