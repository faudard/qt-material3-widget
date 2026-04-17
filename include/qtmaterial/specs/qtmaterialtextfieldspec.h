#pragma once
#include <QColor>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT TextFieldSpec {
    QColor containerColor;
    QColor activeIndicatorColor;
    QColor outlineColor;
    QColor focusedOutlineColor;
    QColor inputTextColor;
    QColor labelColor;
    QColor supportingTextColor;
    QColor cursorColor;
    QColor errorColor;
    QColor stateLayerColor;
    ShapeRole shapeRole = ShapeRole::ExtraSmall;
    TypeRole inputTypeRole = TypeRole::BodyLarge;
    TypeRole labelTypeRole = TypeRole::BodyLarge;
    TypeRole supportingTextRole = TypeRole::BodySmall;
    MotionToken motionToken = MotionToken::Short4;
    int minHeight = 56;
    int horizontalPadding = 16;
    int verticalPadding = 8;
    int supportingTopSpacing = 4;
};
} // namespace QtMaterial
