#pragma once

#include <QColor>
#include <QMargins>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"

namespace QtMaterial {

struct QTMATERIAL3_SPECS_EXPORT SnackbarSpec
{
    QColor containerColor;
    QColor textColor;
    QColor actionColor;
    QColor dismissIconColor;

    ShapeRole shapeRole = ShapeRole::Small;
    ElevationRole elevationRole = ElevationRole::Level3;

    MotionToken enterMotion = MotionToken::Medium2;
    MotionToken exitMotion  = MotionToken::Short4;

    QMargins outerMargins   = QMargins(16, 16, 16, 16);
    QMargins contentPadding = QMargins(16, 14, 8, 14);

    int minHeight = 48;
    int maxWidth = 560;
    int actionSpacing = 8;
    int actionMinWidth = 64;
    int dismissButtonSize = 40;
};

} // namespace QtMaterial