#pragma once
#include <QColor>
#include <QSize>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT FabSpec {
    QColor containerColor;
    QColor iconColor;
    QColor disabledContainerColor;
    QColor disabledIconColor;
    QColor stateLayerColor;
    ShapeRole shapeRole = ShapeRole::Full;
    ElevationRole elevationRole = ElevationRole::Level3;
    MotionToken motionToken = MotionToken::Medium2;
    QSize touchTarget = QSize(56, 56);
    int containerDiameter = 56;
    int iconSize = 24;
};
} // namespace QtMaterial
