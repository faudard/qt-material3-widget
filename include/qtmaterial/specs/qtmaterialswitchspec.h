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
    QColor disabledSelectedTrackColor;
    QColor disabledSelectedHandleColor;
    QColor disabledUnselectedTrackColor;
    QColor disabledUnselectedHandleColor;
    QColor iconColor;
    QColor stateLayerColor;
    QColor focusRingColor;
    QColor labelColor;
    QColor disabledLabelColor;
    TypeRole labelTypeRole = TypeRole::BodyLarge;
    MotionToken motionToken = MotionToken::Short3;
    QSize touchTarget = QSize(52, 32);
    int trackWidth = 52;
    int trackHeight = 32;
    int handleDiameter = 16;
    int selectedHandleDiameter = 24;
    int spacing = 12;
    int stateLayerSize = 40;
};
} // namespace QtMaterial
