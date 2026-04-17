#pragma once
#include <QColor>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT NavigationRailSpec {
    QColor containerColor;
    QColor iconColor;
    QColor activeIndicatorColor;
    QColor labelColor;
    ShapeRole shapeRole = ShapeRole::Large;
    int railWidth = 80;
};
}
