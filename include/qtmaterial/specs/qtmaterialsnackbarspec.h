#pragma once

#include <QColor>
#include <QFont>
#include <QMargins>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
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
    MotionToken exitMotion = MotionToken::Short4;

    QMargins outerMargins = QMargins(16, 16, 16, 16);
    QMargins contentPadding = QMargins(16, 14, 8, 14);

    int minHeight = 48;
    int minWidth = 200;
    int maxWidth = 560;
    int labelMinWidth = 120;
    int controlsReserveSpacing = 24;
    int slideDistance = 16;

    int actionSpacing = 8;
    int actionMinWidth = 64;
    int actionPaddingHorizontal = 12;
    int actionPaddingVertical = 8;
    int actionCornerRadius = 20;

    int dismissButtonSize = 40;
    int dismissCornerRadius = 20;

    int shortDurationMs = 4000;
    int longDurationMs = 10000;
};

} // namespace QtMaterial
