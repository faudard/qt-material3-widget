#pragma once
#include <QColor>
#include "qtmaterial/qtmaterialglobal.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT MenuItemSpec {
    QColor textColor;
    QColor iconColor;
    QColor stateLayerColor;
    int height = 48;
    int horizontalPadding = 12;
};
}
