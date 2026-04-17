#pragma once
#include <QColor>
#include <QMargins>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT ListItemSpec {
    QColor containerColor;
    QColor headlineColor;
    QColor supportingColor;
    QColor leadingColor;
    QColor trailingColor;
    QColor stateLayerColor;
    TypeRole headlineTypeRole = TypeRole::BodyLarge;
    TypeRole supportingTypeRole = TypeRole::BodyMedium;
    QMargins contentPadding = QMargins(16, 12, 16, 12);
    int minHeight = 56;
};
} // namespace QtMaterial
