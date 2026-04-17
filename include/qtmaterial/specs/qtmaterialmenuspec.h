#pragma once
#include <QColor>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT MenuSpec {
    QColor containerColor;
    QColor itemTextColor;
    QColor stateLayerColor;
    ShapeRole shapeRole = ShapeRole::Small;
    ElevationRole elevationRole = ElevationRole::Level2;
    int minWidth = 112;
};
}
