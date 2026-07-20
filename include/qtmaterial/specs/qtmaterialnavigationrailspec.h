#pragma once

#include <QColor>
#include <QFont>
#include <QSize>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialelevationscale.h"
#include "qtmaterial/theme/qtmaterialmotiontokens.h"
#include "qtmaterial/theme/qtmaterialtypographyscale.h"

namespace QtMaterial {

struct QTMATERIAL3_SPECS_EXPORT NavigationRailSpec
{
    QColor containerColor;
    QColor indicatorColor;
    QColor selectedIconColor;
    QColor selectedLabelColor;
    QColor unselectedIconColor;
    QColor unselectedLabelColor;
    QColor disabledIconColor;
    QColor disabledLabelColor;
    QColor stateLayerColor;
    QColor focusRingColor;
    QColor dividerColor;

    QFont labelFont;
    MotionStyle motionStyle;

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

    qreal indicatorRadius = -1.0;
    qreal hoverStateLayerOpacity = 0.08;
    qreal focusStateLayerOpacity = 0.12;
    qreal pressStateLayerOpacity = 0.12;
    qreal disabledOpacity = 0.38;
    qreal focusRingWidth = 2.0;

    int indicatorTopOffset = 4;
    int iconTopOffset = 8;
    int labelTopOffset = 36;
    int labelHeight = 22;
    int dividerWidth = 1;
    bool showDivider = false;

    bool hasResolvedLabelFont = false;
    bool hasResolvedMotionStyle = false;
};

} // namespace QtMaterial
