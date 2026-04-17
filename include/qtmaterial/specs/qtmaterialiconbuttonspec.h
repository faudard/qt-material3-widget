#pragma once
#include <QColor>
#include <QSize>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT IconButtonSpec {
    QColor containerColor;
    QColor iconColor;
    QColor selectedContainerColor;
    QColor selectedIconColor;
    QColor disabledIconColor;
    QColor stateLayerColor;
    ShapeRole shapeRole = ShapeRole::Full;
    ElevationRole elevationRole = ElevationRole::Level0;
    MotionToken motionToken = MotionToken::Short4;
    QSize touchTarget = QSize(48, 48);
    int containerSize = 40;
    int iconSize = 24;
};
} // namespace QtMaterial
