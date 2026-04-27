#pragma once

#include <QColor>
#include <QSize>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"

namespace QtMaterial {

struct QTMATERIAL3_SPECS_EXPORT AutocompleteSpec {
    QColor inputContainerColor;
    QColor popupContainerColor;
    QColor inputTextColor;
    QColor suggestionTextColor;
    QColor placeholderColor;
    QColor selectedSuggestionContainerColor;
    QColor selectedSuggestionTextColor;
    QColor stateLayerColor;
    QColor outlineColor;
    QColor focusedOutlineColor;
    QColor focusRingColor;
    QColor disabledTextColor;

    ShapeRole inputShapeRole = ShapeRole::ExtraSmall;
    ShapeRole popupShapeRole = ShapeRole::ExtraSmall;
    TypeRole inputTypeRole = TypeRole::BodyLarge;
    TypeRole suggestionTypeRole = TypeRole::BodyLarge;
    ElevationRole popupElevationRole = ElevationRole::Level2;
    MotionToken popupMotion = MotionToken::Short4;

    int inputMinHeight = 56;
    int popupVisibleItemCount = 6;
    QSize popupMinSize = QSize(240, 48);
    QSize popupMaxSize = QSize(420, 320);
};

} // namespace QtMaterial
