#pragma once

#include <QColor>
#include <QSize>

#include "qtmaterial/theme/qtmaterialcolortoken.h"

namespace QtMaterial {

struct ListItemSpec
{
    QColor containerColor;
    QColor hoveredContainerColor;
    QColor selectedContainerColor;
    QColor pressedContainerColor;

    QColor headlineColor;
    QColor supportingTextColor;
    QColor leadingIconColor;
    QColor trailingIconColor;
    QColor dividerColor;
    QColor stateLayerColor;
    QColor focusRingColor;
    QColor disabledHeadlineColor;
    QColor disabledSupportingTextColor;

    TypeRole headlineTypeRole = TypeRole::BodyLarge;
    TypeRole supportingTypeRole = TypeRole::BodyMedium;
    ShapeRole shapeRole = ShapeRole::None;
    MotionToken motionToken = MotionToken::Short4;

    QSize touchTarget = QSize(48, 56);
    int minHeight = 56;
    int compactMinHeight = 48;
    int largeMinHeight = 72;

    int horizontalPadding = 16;
    int verticalPadding = 8;
    int leadingSlotWidth = 40;
    int trailingSlotWidth = 24;
    int contentSpacing = 16;
    int supportingTopSpacing = 2;
    int dividerInsetStart = 72;
    int dividerInsetEnd = 0;

    bool showDivider = false;
    bool reserveLeadingSlot = true;
    bool reserveTrailingSlot = true;
};

} // namespace QtMaterial
