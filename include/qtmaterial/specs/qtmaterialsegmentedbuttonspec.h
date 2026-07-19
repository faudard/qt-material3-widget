#pragma once

#include <QColor>
#include <QMargins>
#include <QSize>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialmotiontokens.h"
#include <QFont>

namespace QtMaterial {

struct QTMATERIAL3_SPECS_EXPORT SegmentedButtonSpec {
    QColor containerColor;
    QColor selectedContainerColor;
    QColor labelColor;
    QColor selectedLabelColor;
    QColor outlineColor;
    QColor stateLayerColor;
    QColor focusRingColor;
    QColor disabledLabelColor;
    QColor disabledOutlineColor;

    ShapeRole shapeRole = ShapeRole::Full;
    TypeRole labelTypeRole = TypeRole::LabelLarge;
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
    int segmentHeight = 40;
    int minSegmentWidth = 72;
    int horizontalPadding = 16;
    int iconSize = 18;
    int iconSpacing = 8;
    int outlineWidth = 1;
};

} // namespace QtMaterial
