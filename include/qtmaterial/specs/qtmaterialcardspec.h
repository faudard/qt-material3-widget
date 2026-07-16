#pragma once
#include <QColor>
#include <QMargins>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include <QFont>
#include "qtmaterial/theme/qtmaterialelevationscale.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT CardSpec {
    QColor containerColor;
    QColor outlineColor;
    QColor contentColor;
    QColor shadowColor;

    TypeRole titleTypeRole = TypeRole::TitleMedium;
    TypeRole bodyTypeRole = TypeRole::BodyMedium;
    QFont titleFont;
    QFont bodyFont;

    qreal cornerRadius = 12.0;
    ElevationStyle elevationStyle;
    qreal hoverStateLayerOpacity = 0.08;
    qreal focusStateLayerOpacity = 0.12;
    qreal pressStateLayerOpacity = 0.12;

    bool hasResolvedTitleFont = false;
    bool hasResolvedBodyFont = false;
    bool hasResolvedElevationStyle = false;
    ShapeRole shapeRole = ShapeRole::Large;
    ElevationRole elevationRole = ElevationRole::Level1;
    QMargins contentPadding = QMargins(16, 16, 16, 16);
};
} // namespace QtMaterial
