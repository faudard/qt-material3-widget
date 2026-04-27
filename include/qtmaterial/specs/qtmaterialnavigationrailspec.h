#pragma once

#include <QColor>
#include <QSize>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"

namespace QtMaterial {

struct QTMATERIAL3_SPECS_EXPORT NavigationRailSpec {
    QColor containerColor;
    QColor indicatorColor;
    QColor selectedIconColor;
    QColor selectedLabelColor;
    QColor unselectedIconColor;
    QColor unselectedLabelColor;
    QColor stateLayerColor;
    QColor focusRingColor;
    QColor dividerColor;

    ShapeRole indicatorShapeRole = ShapeRole::Full;
    TypeRole labelTypeRole = TypeRole::LabelMedium;
    ElevationRole elevationRole = ElevationRole::Level0;
    MotionToken motionToken = MotionToken::Short4;

    int railWidth = 80;
    int itemHeight = 64;
    int itemSpacing = 12;
    int topPadding = 12;
    int bottomPadding = 12;
    QSize indicatorSize = QSize(56, 32);
    int iconSize = 24;
};

} // namespace QtMaterial
