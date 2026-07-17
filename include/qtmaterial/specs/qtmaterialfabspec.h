#pragma once
#include <QColor>
#include <QSize>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include <QFont>
#include <QtGlobal>
#include "qtmaterial/theme/qtmaterialelevationscale.h"
#include "qtmaterial/theme/qtmaterialmotiontokens.h"
#include "qtmaterial/theme/qtmaterialtypographyscale.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT FabSpec {
    QColor containerColor;
    QColor iconColor;
    QColor disabledContainerColor;
    QColor disabledIconColor;
    QColor stateLayerColor;
    QColor focusRingColor;
    QColor shadowColor;

    TypeRole labelTypeRole = TypeRole::LabelLarge;
    QFont labelFont;
    MotionStyle motionStyle;
    ElevationStyle elevationStyle;

    qreal cornerRadius = -1.0;
    qreal hoverStateLayerOpacity = 0.08;
    qreal focusStateLayerOpacity = 0.12;
    qreal pressStateLayerOpacity = 0.12;
    qreal dragStateLayerOpacity = 0.16;

    qreal restingElevationProgress = 0.65;
    qreal hoverElevationProgress = 1.0;
    qreal focusElevationProgress = 1.0;
    qreal pressElevationProgress = 0.80;
    qreal disabledElevationProgress = 0.0;

    bool hasResolvedLabelFont = false;
    bool hasResolvedMotionStyle = false;
    bool hasResolvedElevationStyle = false;
    ShapeRole shapeRole = ShapeRole::Full;
    ElevationRole elevationRole = ElevationRole::Level3;
    MotionToken motionToken = MotionToken::Medium2;
    QSize touchTarget = QSize(56, 56);
    int containerDiameter = 56;
    int iconSize = 24;
};
} // namespace QtMaterial
