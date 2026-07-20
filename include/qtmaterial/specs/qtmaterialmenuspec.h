#pragma once

#include <QColor>
#include <QFont>
#include <QMargins>
#include <QSize>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialelevationscale.h"
#include "qtmaterial/theme/qtmaterialmotiontokens.h"

namespace QtMaterial {

struct QTMATERIAL3_SPECS_EXPORT MenuSpec
{
    QColor containerColor;
    QColor itemLabelColor;
    QColor itemIconColor;
    QColor disabledItemLabelColor;
    QColor disabledItemIconColor;
    QColor stateLayerColor;
    QColor focusRingColor;
    QColor dividerColor;
    QColor shortcutColor;
    QColor disabledShortcutColor;
    QColor checkedIndicatorColor;
    QColor shadowColor = QColor(0, 0, 0, 96);

    QFont labelFont;
    QFont shortcutFont;
    ElevationStyle elevationStyle;
    MotionStyle enterMotionStyle;
    MotionStyle exitMotionStyle;

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
    qreal cornerRadius = 4.0;
    int separatorHeight = 8;
    int verticalPadding = 8;
    int checkColumnWidth = 24;
    int shortcutSpacing = 16;
    int dividerInset = 12;
    int focusRingWidth = 2;

    qreal hoverStateLayerOpacity = 0.08;
    qreal focusStateLayerOpacity = 0.12;
    qreal pressStateLayerOpacity = 0.12;
};

} // namespace QtMaterial
