#pragma once

#include <QColor>
#include <QMargins>
#include <Qt>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"

namespace QtMaterial {

struct QTMATERIAL3_SPECS_EXPORT DividerSpec
{
    DividerSpec() = default;
    DividerSpec(const DividerSpec& other) = default;
    DividerSpec(DividerSpec&& other) = default;
    DividerSpec& operator=(const DividerSpec& other) = default;
    DividerSpec& operator=(DividerSpec&& other) = default;
    ~DividerSpec() = default;

    QColor color;
    QColor insetColor;

    ShapeRole shapeRole = ShapeRole::None;
    Qt::Orientation orientation = Qt::Horizontal;
    QMargins margins = QMargins(0, 0, 0, 0);

    int thickness = 1;
    int leadingInset = 0;
    int trailingInset = 0;

    qreal cornerRadius = -1.0;
};

} // namespace QtMaterial
