#pragma once

#include <QColor>
#include <QMargins>
#include <QSize>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"

namespace QtMaterial {

struct QTMATERIAL3_SPECS_EXPORT MenuSpec {
    QColor containerColor;
    QColor itemLabelColor;
    QColor itemIconColor;
    QColor disabledItemLabelColor;
    QColor stateLayerColor;
    QColor focusRingColor;
    QColor dividerColor;
    QColor shortcutColor;

    ShapeRole shapeRole = ShapeRole::ExtraSmall;
    TypeRole labelTypeRole = TypeRole::LabelLarge;
    TypeRole shortcutTypeRole = TypeRole::LabelMedium;
    ElevationRole elevationRole = ElevationRole::Level2;
    MotionToken enterMotion = MotionToken::Short4;
    MotionToken exitMotion = MotionToken::Short2;

    QSize minItemSize = QSize(112, 48);
    QSize maxPopupSize = QSize(320, 480);
    QMargins itemPadding = QMargins(12, 0, 12, 0);
    int iconSize = 24;
    int iconSpacing = 12;
    int cornerRadius = 4;
};

} // namespace QtMaterial
