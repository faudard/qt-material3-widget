#pragma once
#include <QColor>
#include <QMargins>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT BannerSpec {
    QColor containerColor;
    QColor headlineColor;
    QColor supportingColor;
    QColor actionColor;
    ShapeRole shapeRole = ShapeRole::Medium;
    QMargins padding = QMargins(16, 16, 16, 16);
};
} // namespace QtMaterial
