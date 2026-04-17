#pragma once
#include <QColor>
#include <QMargins>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT AutocompletePopupSpec {
    QColor containerColor;
    QColor textColor;
    QColor selectedContainerColor;
    QColor selectedTextColor;
    ShapeRole shapeRole = ShapeRole::Medium;
    ElevationRole elevationRole = ElevationRole::Level2;
    QMargins contentPadding = QMargins(8, 8, 8, 8);
    int rowHeight = 40;
};
} // namespace QtMaterial
