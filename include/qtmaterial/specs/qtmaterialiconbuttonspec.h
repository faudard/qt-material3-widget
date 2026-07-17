#pragma once
#include <QColor>
#include <QSize>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include <QtGlobal>
#include "qtmaterial/theme/qtmaterialmotiontokens.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT IconButtonSpec {
    QColor containerColor;
    QColor iconColor;
    QColor selectedContainerColor;
    QColor selectedIconColor;
    QColor disabledIconColor;
    QColor stateLayerColor;
    QColor focusRingColor;
    MotionStyle motionStyle;

    qreal cornerRadius = -1.0;
    qreal hoverStateLayerOpacity = 0.08;
    qreal focusStateLayerOpacity = 0.12;
    qreal pressStateLayerOpacity = 0.12;
    qreal focusRingWidth = 2.0;

    bool hasResolvedMotionStyle = false;
    ShapeRole shapeRole = ShapeRole::Full;
    ElevationRole elevationRole = ElevationRole::Level0;
    MotionToken motionToken = MotionToken::Short4;
    QSize touchTarget = QSize(48, 48);
    int containerSize = 40;
    int iconSize = 24;
};
} // namespace QtMaterial
