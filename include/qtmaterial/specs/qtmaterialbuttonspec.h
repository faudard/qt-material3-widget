#pragma once
#include <QColor>
#include <QSize>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include <QFont>
#include "qtmaterial/theme/qtmaterialmotiontokens.h"
#include "qtmaterial/theme/qtmaterialelevationscale.h"
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
    qreal outlineWidth = 0.0;
    qreal focusRingWidth = 2.0;
    ShapeRole shapeRole = ShapeRole::Full;
    TypeRole labelTypeRole = TypeRole::LabelLarge;
    ElevationRole elevationRole = ElevationRole::Level0;
    ElevationRole hoverElevationRole = ElevationRole::Level0;
    MotionToken motionToken = MotionToken::Short4;
    // Concrete runtime values resolved from the theme/overrides.
    QFont labelFont;
    MotionStyle motionStyle;
    QColor shadowColor;
    ElevationStyle elevationStyle;
    ElevationStyle hoverElevationStyle;
    qreal restingElevationProgress = 0.0;
    qreal hoverElevationProgress = 0.0;
    qreal focusElevationProgress = 0.0;
    qreal pressElevationProgress = 0.0;
    qreal disabledElevationProgress = 0.0;
    bool hasResolvedElevationStyle = false;
    bool hasResolvedHoverElevationStyle = false;
    qreal cornerRadius = -1.0;
    qreal hoverStateLayerOpacity = 0.08;
    qreal focusStateLayerOpacity = 0.10;
    qreal pressStateLayerOpacity = 0.10;
    qreal dragStateLayerOpacity = 0.16;
    bool hasResolvedLabelFont = false;
    bool hasResolvedMotionStyle = false;
    QSize touchTarget = QSize(48, 48);
    int containerHeight = 40;
    int horizontalPadding = 24;
    int iconSize = 18;
    int iconSpacing = 8;
};
} // namespace QtMaterial
