#pragma once

#include <QColor>
#include <QFont>
#include <QSize>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"

namespace QtMaterial {

struct QTMATERIAL3_SPECS_EXPORT ListItemSpec
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
    QColor disabledIconColor;

    QFont headlineFont;
    QFont supportingFont;

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
    int iconSize = 20;
    int contentSpacing = 12;
    int supportingTopSpacing = 2;
    int dividerInsetStart = 16;
    int dividerInsetEnd = 16;
    int focusRingWidth = 2;

    qreal cornerRadius = -1.0;
    qreal hoverOpacity = -1.0;
    qreal focusOpacity = -1.0;
    qreal pressedOpacity = -1.0;
    qreal disabledOpacity = 0.38;

    bool showDivider = false;
    bool reserveLeadingSlot = false;
    bool reserveTrailingSlot = false;
};

} // namespace QtMaterial
