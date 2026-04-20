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
    QColor disabledLabelColor;
    QColor supportingTextColor;
    QColor disabledSupportingTextColor;
    QColor errorTextColor;
    QColor cursorColor;
    QColor errorColor;
    QColor stateLayerColor;
    QColor disabledErrorTextColor;
    QColor focusRingColor;

    ShapeRole shapeRole = ShapeRole::ExtraSmall;
    TypeRole inputTypeRole = TypeRole::BodyLarge;
    TypeRole labelTypeRole = TypeRole::BodyLarge;
    TypeRole supportingTextRole = TypeRole::BodySmall;
    MotionToken motionToken = MotionToken::Short4;

    int minHeight = 56;
    int horizontalPadding = 16;
    int verticalPadding = 8;
    int supportingTopSpacing = 4;
    int topLabelHeight = 20;
    int supportingHeight = 20;
    int outlineWidth = 1;
    int focusedOutlineWidth = 2;
    qreal focusRingWidth = 1.25;
    int supportingLineHeight = 16;
    bool reserveSupportingLine = true;
};

} // namespace QtMaterial
