#pragma once
#include <QColor>
#include <QSize>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT RadioButtonSpec {
    QColor selectedColor;
    QColor unselectedOutlineColor;
    QColor disabledColor;
    QColor labelColor;
    QColor stateLayerColor;
    QColor focusRingColor;
    TypeRole labelTypeRole = TypeRole::BodyLarge;
    MotionToken motionToken = MotionToken::Short3;
    QSize touchTarget = QSize(48, 48);
    int indicatorSize = 20;
    int dotSize = 10;
    int spacing = 12;
    int outlineWidth = 2;
};
} // namespace QtMaterial
