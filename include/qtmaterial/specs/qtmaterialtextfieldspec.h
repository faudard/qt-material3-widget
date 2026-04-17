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
    QColor disabledOutlineColor;
    QColor inputTextColor;
    QColor disabledInputTextColor;
    QColor labelColor;
    QColor focusedLabelColor;
    QColor disabledLabelColor;
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
    int supportingHeight = 16;
    int cornerRadius = 4;
    int labelRestingTop = 18;
    int labelFloatingTop = 8;
    int editorTopPadding = 24;
    int editorBottomPadding = 8;
};
} // namespace QtMaterial
