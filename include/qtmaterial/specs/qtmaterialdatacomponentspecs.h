#pragma once

#include <QColor>
#include <QFont>
#include <QMargins>
#include <QSize>

#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

struct DatePickerSpec
{
    QColor backgroundColor;
    QColor foregroundColor;
    QColor headlineColor;
    QColor weekdayTextColor;
    QColor selectedDateColor;
    QColor selectedDateTextColor;
    QColor todayOutlineColor;
    QColor disabledTextColor;
    QColor hoverColor;
    QColor navigationButtonTextColor;
    QColor focusRingColor;
    QFont headlineFont;
    QFont dayFont;
    QFont weekdayFont;
    int cornerRadius = 28;
    int cellSize = 40;
    int headerHeight = 64;
    int focusRingWidth = 2;
    int contentSpacing = 8;
    int navigationButtonRadius = 16;
    int navigationButtonPaddingHorizontal = 10;
    int navigationButtonPaddingVertical = 6;
    QMargins contentMargins = QMargins(16, 16, 16, 16);
};

struct TableSpec
{
    QColor backgroundColor;
    QColor foregroundColor;
    QColor headerBackgroundColor;
    QColor headerForegroundColor;
    QColor gridColor;
    QColor rowHoverColor;
    QColor rowPressedColor;
    QColor rowSelectedColor;
    QColor rowSelectedTextColor;
    QColor disabledTextColor;
    QColor focusRingColor;

    QFont headerFont;
    QFont bodyFont;

    TypeRole headerTypeRole = TypeRole::LabelLarge;
    TypeRole bodyTypeRole = TypeRole::BodyMedium;
    ShapeRole shapeRole = ShapeRole::Medium;

    int rowHeight = 48;
    int denseRowHeight = 36;
    int headerHeight = 56;
    int gridWidth = 1;
    int focusRingWidth = 2;
    int cellHorizontalPadding = 12;
    int headerHorizontalPadding = 12;
    int minimumColumnWidth = 72;

    qreal cornerRadius = -1.0;
};

struct GridListSpec
{
    QColor backgroundColor;
    QColor foregroundColor;
    QColor itemBackgroundColor;
    QColor itemHoverColor;
    QColor itemSelectedColor;
    QColor itemSelectedTextColor;
    QColor supportingTextColor;
    QColor disabledTextColor;
    QColor disabledSupportingTextColor;
    QColor itemOutlineColor;
    QColor focusRingColor;

    QFont titleFont;
    QFont supportingFont;

    QSize itemSize = QSize(160, 144);
    QSize iconSize = QSize(32, 32);
    QMargins itemContentMargins = QMargins(12, 12, 12, 12);

    int itemRadius = 16;
    int spacing = 12;
    int focusRingWidth = 2;
    int outlineWidth = 1;
    int titleSupportingSpacing = 4;
    int iconTextSpacing = 8;
    int columns = 3;
    int minimumCellWidth = 120;
};

struct CarouselSpec
{
    // Legacy page-carousel tokens retained for source compatibility.
    QColor backgroundColor;
    QColor foregroundColor;
    QColor pageIndicatorColor;
    QColor activePageIndicatorColor;
    QColor navigationButtonColor;
    QColor navigationButtonTextColor;
    QColor focusRingColor;
    QFont labelFont;
    QSize preferredPageSize = QSize(360, 220);
    int indicatorSize = 8;

    // Resolved item-carousel tokens used by QtMaterialCarousel.
    QColor itemBackgroundColor;
    QColor itemHoverColor;
    QColor itemPressedColor;
    QColor itemSelectedColor;
    QColor itemSelectedTextColor;
    QColor supportingTextColor;
    QColor disabledTextColor;
    QColor outlineColor;
    QColor selectedOutlineColor;
    QColor iconColor;
    QColor selectedIconColor;

    QFont supportingFont;

    QSize itemSize = QSize(176, 128);
    QSize minimumItemSize = QSize(72, 72);

    QMargins outerMargins = QMargins(16, 16, 16, 16);
    QMargins contentMargins = QMargins(14, 12, 14, 12);

    ShapeRole shapeRole = ShapeRole::Large;

    qreal cornerRadius = -1.0;

    int pageSpacing = 12;
    int focusRingWidth = 2;
    int outlineWidth = 1;
    int selectedOutlineWidth = 2;
    int iconSize = 28;
    int iconSpacing = 10;
    int supportingTopSpacing = 6;
};

QTMATERIAL3_SPECS_EXPORT DatePickerSpec defaultDatePickerSpec();
QTMATERIAL3_SPECS_EXPORT TableSpec defaultTableSpec();
QTMATERIAL3_SPECS_EXPORT GridListSpec defaultGridListSpec();
QTMATERIAL3_SPECS_EXPORT CarouselSpec defaultCarouselSpec();

} // namespace QtMaterial
