#pragma once
#include <QColor>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT DateFieldSpec {
    QColor leadingIconColor;
    QColor trailingIconColor;
    QColor placeholderColor;
    MotionToken motionToken = MotionToken::Short4;
};
} // namespace QtMaterial
