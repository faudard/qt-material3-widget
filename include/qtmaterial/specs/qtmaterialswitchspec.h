#pragma once
#include <QColor>
#include <QSize>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT SwitchSpec {
    QColor selectedTrackColor;
    QColor selectedHandleColor;
    QColor unselectedTrackColor;
    QColor unselectedHandleColor;
    QColor iconColor;
    QColor stateLayerColor;
    MotionToken motionToken = MotionToken::Short3;
    QSize touchTarget = QSize(52, 32);
    int trackWidth = 52;
    int trackHeight = 32;
    int handleDiameter = 16;
    int selectedHandleDiameter = 24;
};
} // namespace QtMaterial
