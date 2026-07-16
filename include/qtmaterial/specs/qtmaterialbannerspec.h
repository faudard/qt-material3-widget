#pragma once
#include <QColor>
#include <QMargins>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include <QFont>
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT BannerSpec {
    QColor containerColor;
    QColor headlineColor;
    QColor supportingColor;
    QColor actionColor;
    QColor outlineColor;
    QColor focusRingColor;

    TypeRole headlineTypeRole = TypeRole::TitleMedium;
    TypeRole supportingTypeRole = TypeRole::BodyMedium;
    TypeRole actionTypeRole = TypeRole::LabelLarge;
    QFont headlineFont;
    QFont supportingFont;
    QFont actionFont;

    qreal cornerRadius = 12.0;
    bool hasResolvedHeadlineFont = false;
    bool hasResolvedSupportingFont = false;
    bool hasResolvedActionFont = false;
    ShapeRole shapeRole = ShapeRole::Medium;
    QMargins padding = QMargins(16, 16, 16, 16);
};
} // namespace QtMaterial
