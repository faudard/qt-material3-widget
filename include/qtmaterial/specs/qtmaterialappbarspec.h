#pragma once
#include <QColor>
#include <QMargins>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT AppBarSpec {
    QColor containerColor;
    QColor titleColor;
    QColor navigationIconColor;
    QColor actionIconColor;
    ElevationRole elevationRole = ElevationRole::Level0;
    QMargins contentPadding = QMargins(16, 8, 16, 8);
    int preferredHeight = 64;
};
} // namespace QtMaterial
