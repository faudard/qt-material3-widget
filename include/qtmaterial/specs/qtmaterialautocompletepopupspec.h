#pragma once

#include <QColor>
#include <QMargins>
#include <QSize>

#include "qtmaterial/theme/qtmaterialcolortoken.h"

namespace QtMaterial {

struct AutocompletePopupSpec
{
    QColor containerColor;
    QColor textColor;
    QColor selectedTextColor;
    QColor supportingTextColor;
    QColor selectedContainerColor;
    QColor stateLayerColor;
    QColor dividerColor;
    QColor focusRingColor;
    QColor shadowColor;

    ShapeRole shapeRole = ShapeRole::Large;
    ElevationRole elevationRole = ElevationRole::Level2;
    TypeRole itemTypeRole = TypeRole::BodyLarge;
    TypeRole supportingTypeRole = TypeRole::BodySmall;
    MotionToken motionToken = MotionToken::Short4;

    QSize minPopupSize = QSize(160, 0);
    QSize maxPopupSize = QSize(480, 320);
    QSize itemIconSize = QSize(20, 20);
    QSize itemMinSize = QSize(0, 48);

    QMargins contentMargins = QMargins(12, 8, 12, 8);
    int itemSpacing = 8;
    int supportingTopSpacing = 2;
    int visibleItemCount = 6;
    int cornerRadius = 16;
    int focusRingWidth = 2;
    bool showDividers = false;
};

} // namespace QtMaterial
