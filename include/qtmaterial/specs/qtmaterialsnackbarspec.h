#pragma once

#include <QColor>
#include <QMargins>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include <QFont>
#include "qtmaterial/theme/qtmaterialelevationscale.h"
#include "qtmaterial/theme/qtmaterialmotiontokens.h"

namespace QtMaterial {

struct QTMATERIAL3_SPECS_EXPORT SnackbarSpec
{
    QColor containerColor;
    QColor textColor;
    QColor actionColor;
    QColor dismissIconColor;
    QColor shadowColor;

    TypeRole textTypeRole = TypeRole::BodyMedium;
    TypeRole actionTypeRole = TypeRole::LabelLarge;
    QFont textFont;
    QFont actionFont;

    qreal cornerRadius = 8.0;
    ElevationStyle elevationStyle;
    MotionStyle enterMotionStyle;
    MotionStyle exitMotionStyle;

    bool hasResolvedTextFont = false;
    bool hasResolvedActionFont = false;
    bool hasResolvedElevationStyle = false;
    bool hasResolvedEnterMotion = false;
    bool hasResolvedExitMotion = false;

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