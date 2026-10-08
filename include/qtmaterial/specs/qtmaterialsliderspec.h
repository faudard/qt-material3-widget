#pragma once

#include <QColor>
#include <QSize>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

/**
 * Resolved Material metrics and colors for a single-value slider.
 *
 * This spec is intentionally independent from QSlider/QStyle so the same
 * semantic values can be consumed by native adapters and first-class widgets.
 */
struct QTMATERIAL3_SPECS_EXPORT SliderSpec
{
    QColor activeTrackColor;
    QColor inactiveTrackColor;
    QColor handleColor;
    QColor disabledActiveTrackColor;
    QColor disabledInactiveTrackColor;
    QColor disabledHandleColor;
    QColor stateLayerColor;
    QColor focusRingColor;

    QSize touchTarget = QSize(48, 48);
    int trackThickness = 4;
    int handleDiameter = 20;
    int stateLayerSize = 40;
    int focusRingWidth = 2;

    qreal hoverStateLayerOpacity = 0.08;
    qreal focusStateLayerOpacity = 0.12;
    qreal pressStateLayerOpacity = 0.12;
};

} // namespace QtMaterial
