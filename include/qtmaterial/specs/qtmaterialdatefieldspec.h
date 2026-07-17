#pragma once
#include <QColor>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialmotiontokens.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT DateFieldSpec {
    QColor leadingIconColor;
    QColor trailingIconColor;
    QColor placeholderColor;
    QColor clearIconColor;
    QColor disabledIconColor;
    MotionStyle motionStyle;
    int actionIconSize = 20;
    int actionButtonExtent = 40;
    bool calendarVisibleWhenEmpty = true;
    bool preferClearAction = true;
    bool hasResolvedMotionStyle = false;
    MotionToken motionToken = MotionToken::Short4;
};
} // namespace QtMaterial
