#pragma once
#include <QColor>
#include "qtmaterial/qtmaterialglobal.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT TableSpec {
    QColor headerColor;
    QColor rowColor;
    QColor dividerColor;
    QColor textColor;
    int rowHeight = 52;
};
}
