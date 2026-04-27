#pragma once

#include <QColor>
#include <QMargins>
#include <QSize>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"

namespace QtMaterial {

enum class ChipVariant {
    Assist,
    Filter,
    Input,
    Suggestion
};

struct QTMATERIAL3_SPECS_EXPORT ChipSpec {
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

    QSize touchTarget = QSize(48, 48);
    int containerHeight = 32;
    int minWidth = 48;
    int horizontalPadding = 16;
    int iconSize = 18;
    int iconSpacing = 8;
    int outlineWidth = 1;
};

} // namespace QtMaterial
