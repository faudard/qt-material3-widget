#pragma once

#include <QColor>
#include <QMargins>
#include <QSize>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"

namespace QtMaterial {

struct QTMATERIAL3_SPECS_EXPORT ProgressIndicatorSpec {
    ProgressIndicatorSpec();
    ProgressIndicatorSpec(const ProgressIndicatorSpec& other);
    ProgressIndicatorSpec(ProgressIndicatorSpec&& other) noexcept;
    ProgressIndicatorSpec& operator=(const ProgressIndicatorSpec& other);
    ProgressIndicatorSpec& operator=(ProgressIndicatorSpec&& other) noexcept;
    ~ProgressIndicatorSpec();

    QColor activeColor;
    QColor trackColor;
    QColor stopIndicatorColor;

    ColorRole activeColorRole = ColorRole::Primary;
    ColorRole trackColorRole = ColorRole::SurfaceVariant;
    ShapeRole shapeRole = ShapeRole::Full;

    QMargins margins = QMargins(0, 0, 0, 0);
    QSize circularSize = QSize(48, 48);

    int linearHeight = 4;
    int circularStrokeWidth = 4;
    int trackGap = 4;
    int stopIndicatorSize = 4;
    int animationDurationMs = 1400;
};

} // namespace QtMaterial
