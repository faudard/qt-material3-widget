#pragma once
#include <QColor>
#include <QMargins>
#include "qtmaterial/qtmaterialglobal.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT DividerSpec {
    QColor color;
    QMargins margins = QMargins(0, 0, 0, 0);
    int thickness = 1;
};
} // namespace QtMaterial
