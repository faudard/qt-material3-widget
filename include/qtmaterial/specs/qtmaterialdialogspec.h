#pragma once
#include <QColor>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include <QFont>
#include <QtGlobal>
#include "qtmaterial/theme/qtmaterialelevationscale.h"
#include "qtmaterial/theme/qtmaterialmotiontokens.h"
#include "qtmaterial/theme/qtmaterialtypographyscale.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT DialogSpec {
    QColor containerColor;
    QColor headlineColor;
    QColor bodyColor;
    QColor scrimColor;
    QColor shadowColor;

    TypeRole headlineTypeRole = TypeRole::HeadlineSmall;
    TypeRole bodyTypeRole = TypeRole::BodyMedium;
    QFont headlineFont;
    QFont bodyFont;

    qreal scrimOpacity = 0.32;
    qreal cornerRadius = 28.0;
    ElevationStyle elevationStyle;
    MotionStyle enterMotionStyle;
    MotionStyle exitMotionStyle;

    bool hasResolvedHeadlineFont = false;
    bool hasResolvedBodyFont = false;
    bool hasResolvedElevationStyle = false;
    bool hasResolvedEnterMotionStyle = false;
    bool hasResolvedExitMotionStyle = false;
    ShapeRole shapeRole = ShapeRole::ExtraLarge;
    ElevationRole elevationRole = ElevationRole::Level3;
    MotionToken enterMotion = MotionToken::Medium2;
    MotionToken exitMotion = MotionToken::Short4;
    int maxWidth = 560;
    int padding = 24;
    int headlineBottomSpacing = 16;
    int actionsTopSpacing = 24;
};
} // namespace QtMaterial
