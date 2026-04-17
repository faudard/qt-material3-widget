#pragma once
#include <QColor>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT DialogSpec {
    QColor containerColor;
    QColor headlineColor;
    QColor bodyColor;
    QColor scrimColor;
    ShapeRole shapeRole = ShapeRole::ExtraLarge;
    ElevationRole elevationRole = ElevationRole::Level3;
    MotionToken enterMotion = MotionToken::Medium2;
    MotionToken exitMotion = MotionToken::Short4;
    int maxWidth = 560;
    int padding = 24;
    int headlineBottomSpacing = 16;
    int actionsTopSpacing = 24;
};
} // namespace QtMaterial
