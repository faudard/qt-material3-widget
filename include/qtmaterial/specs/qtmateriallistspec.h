#pragma once

#include <QColor>
#include <QMargins>
#include <QSize>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"

namespace QtMaterial {

struct QTMATERIAL3_WIDGETS_EXPORT ListSpec
{
    QColor containerColor;
    QColor focusRingColor;
    QColor dividerColor;
    QColor selectionOverlayColor;

    ShapeRole shapeRole = ShapeRole::Large;
    MotionToken motionToken = MotionToken::Short4;

    QMargins contentMargins = QMargins(0, 8, 0, 8);
    QSize minimumViewportSize = QSize(240, 160);
    int itemSpacing = 0;
    int dividerThickness = 1;
    bool showDividers = false;
};

} // namespace QtMaterial
