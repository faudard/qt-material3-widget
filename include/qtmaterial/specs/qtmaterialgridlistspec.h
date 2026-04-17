#pragma once
#include <QColor>
#include "qtmaterial/qtmaterialglobal.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT GridListSpec {
    QColor containerColor;
    QColor textColor;
    int cellSpacing = 8;
    int minCellWidth = 120;
};
}
