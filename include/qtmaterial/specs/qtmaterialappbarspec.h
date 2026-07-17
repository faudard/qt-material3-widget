#pragma once
#include <QColor>
#include <QMargins>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include <QFont>
#include <QtGlobal>
#include "qtmaterial/theme/qtmaterialelevationscale.h"
#include "qtmaterial/theme/qtmaterialtypographyscale.h"

namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT AppBarSpec {
    QColor containerColor;
    QColor titleColor;
    QColor navigationIconColor;
    QColor actionIconColor;
    QColor outlineColor;
    QColor focusRingColor;
    QColor shadowColor;

    TypeRole titleTypeRole = TypeRole::TitleLarge;
    QFont titleFont;

    ElevationRole elevatedElevationRole = ElevationRole::Level2;
    ElevationStyle elevatedElevationStyle;

    int minimumWidth = 200;
    int preferredWidth = 360;
    int actionSlot = 48;
    int betweenSpacing = 12;
    int fabSlot = 64;
    qreal focusRingWidth = 2.0;

    bool hasResolvedTitleFont = false;
    bool hasResolvedElevatedElevationStyle = false;
    ElevationRole elevationRole = ElevationRole::Level0;
    QMargins contentPadding = QMargins(16, 8, 16, 8);
    int preferredHeight = 64;
};
} // namespace QtMaterial
