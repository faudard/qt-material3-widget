#pragma once

#include <QColor>
#include <QSize>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include <QFont>
#include <QtGlobal>
#include "qtmaterial/theme/qtmaterialmotiontokens.h"
#include "qtmaterial/theme/qtmaterialtypographyscale.h"

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

    QFont labelFont;
    MotionStyle motionStyle;
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
