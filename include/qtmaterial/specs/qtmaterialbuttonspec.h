#pragma once
#include <QColor>
#include <QSize>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT ButtonSpec {
    QColor containerColor;
    QColor labelColor;
    QColor iconColor;
    QColor disabledContainerColor;
    QColor disabledLabelColor;
    QColor stateLayerColor;
    QColor focusRingColor;
    QColor outlineColor;
    QColor disabledOutlineColor;
    ShapeRole shapeRole = ShapeRole::Full;
    TypeRole labelTypeRole = TypeRole::LabelLarge;
    ElevationRole elevationRole = ElevationRole::Level0;
    MotionToken motionToken = MotionToken::Short4;
    QSize touchTarget = QSize(48, 48);
    int containerHeight = 40;
    int horizontalPadding = 24;
    int iconSize = 18;
    int iconSpacing = 8;
};
} // namespace QtMaterial
