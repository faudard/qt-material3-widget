#pragma once
#include <QColor>
#include <QMargins>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT CardSpec {
    QColor containerColor;
    QColor outlineColor;
    QColor contentColor;
    ShapeRole shapeRole = ShapeRole::Large;
    ElevationRole elevationRole = ElevationRole::Level1;
    QMargins contentPadding = QMargins(16, 16, 16, 16);
};
} // namespace QtMaterial
