#pragma once
#include <QColor>
#include <QSize>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT CheckboxSpec {
    QColor selectedContainerColor;
    QColor selectedIconColor;
    QColor unselectedOutlineColor;
    QColor disabledSelectedContainerColor;
    QColor disabledUnselectedOutlineColor;
    QColor labelColor;
    QColor disabledLabelColor;
    QColor stateLayerColor;
    QColor focusRingColor;
    TypeRole labelTypeRole = TypeRole::BodyLarge;
    MotionToken motionToken = MotionToken::Short3;
    QSize touchTarget = QSize(48, 48);
    int indicatorSize = 18;
    int spacing = 12;
    int outlineWidth = 2;
    int cornerRadius = 4;
    int checkmarkStrokeWidth = 2;
    int stateLayerSize = 40;
};
} // namespace QtMaterial
