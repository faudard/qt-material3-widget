#pragma once
#include <QColor>
#include <QSize>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT ChipSpec {
    QColor containerColor;
    QColor labelColor;
    QColor iconColor;
    QColor outlineColor;
    QColor stateLayerColor;
    ShapeRole shapeRole = ShapeRole::Full;
    QSize touchTarget = QSize(48, 32);
    int height = 32;
    int horizontalPadding = 16;
};
}
