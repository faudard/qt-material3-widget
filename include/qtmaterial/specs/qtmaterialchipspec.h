#pragma once

#include <QColor>
#include <QFont>
#include <QSize>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialelevationscale.h"
#include "qtmaterial/theme/qtmaterialmotiontokens.h"
#include "qtmaterial/theme/qtmaterialshapescale.h"
#include "qtmaterial/theme/qtmaterialtypographyscale.h"

namespace QtMaterial {

enum class ChipVariant {
    Assist,
    Filter,
    Input,
    Suggestion
};

struct QTMATERIAL3_SPECS_EXPORT ChipSpec
{
    ChipVariant variant = ChipVariant::Assist;

    QColor containerColor;
    QColor selectedContainerColor;
    QColor labelColor;
    QColor selectedLabelColor;
    QColor iconColor;
    QColor selectedIconColor;
    QColor outlineColor;
    QColor disabledContainerColor;
    QColor disabledLabelColor;
    QColor disabledOutlineColor;
    QColor stateLayerColor;
    QColor focusRingColor;

    ShapeRole shapeRole = ShapeRole::Small;
    TypeRole labelTypeRole = TypeRole::LabelLarge;
    ElevationRole elevationRole = ElevationRole::Level0;
    MotionToken motionToken = MotionToken::Short4;

    // Concrete values resolved by the resolver boundary.
    QFont labelFont;
    MotionStyle motionStyle;
    qreal cornerRadius = -1.0;
    qreal hoverStateLayerOpacity = 0.08;
    qreal focusStateLayerOpacity = 0.12;
    qreal pressStateLayerOpacity = 0.12;
    qreal dragStateLayerOpacity = 0.16;
    bool hasResolvedLabelFont = false;
    bool hasResolvedMotionStyle = false;

    QSize touchTarget = QSize(48, 48);
    int containerHeight = 32;
    int minWidth = 48;
    int horizontalPadding = 16;
    int iconSize = 18;
    int iconSpacing = 8;
    int outlineWidth = 1;

};

} // namespace QtMaterial
