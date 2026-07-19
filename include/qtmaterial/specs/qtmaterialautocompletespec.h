#pragma once

#include <QColor>
#include <QSize>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialelevationscale.h"
#include "qtmaterial/theme/qtmaterialmotiontokens.h"
#include <QFont>

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

    // Concrete runtime values resolved by the resolver boundary.
    QFont inputFont;
    QFont suggestionFont;
    MotionStyle popupMotionStyle;
    ElevationStyle popupElevationStyle;
    qreal inputCornerRadius = 4.0;
    qreal popupCornerRadius = 4.0;
    qreal hoverStateLayerOpacity = 0.08;
    qreal focusStateLayerOpacity = 0.12;
    qreal pressStateLayerOpacity = 0.12;
    qreal outlineWidth = 1.0;
    qreal focusedOutlineWidth = 2.0;
    qreal focusRingWidth = 2.0;
    bool hasResolvedInputFont = false;
    bool hasResolvedSuggestionFont = false;
    bool hasResolvedPopupMotionStyle = false;
    bool hasResolvedPopupElevationStyle = false;

    int inputMinHeight = 56;
    int horizontalPadding = 16;
    int verticalInset = 4;
    int popupVisibleItemCount = 6;
    QSize popupMinSize = QSize(240, 48);
    QSize popupMaxSize = QSize(420, 320);
};

} // namespace QtMaterial
