#pragma once

#include <QColor>
#include <QMargins>
#include <Qt>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"

namespace QtMaterial {

struct QTMATERIAL3_WIDGETS_EXPORT DividerSpec
{
    QColor color;
    QColor insetColor;
    ShapeRole shapeRole = ShapeRole::None;
    Qt::Orientation orientation = Qt::Horizontal;
    QMargins margins = QMargins(0, 0, 0, 0);
    int thickness = 1;
    int leadingInset = 0;
    int trailingInset = 0;
};

} // namespace QtMaterial
