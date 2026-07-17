#pragma once

#include <QColor>
#include <QFont>
#include <QMargins>
#include <QSize>

#include "qtmaterial/theme/qtmaterialcolortoken.h"

namespace QtMaterial {

struct DatePickerSpec
{
    QColor backgroundColor;
    QColor foregroundColor;
    QColor headlineColor;
    QColor selectedDateColor;
    QColor selectedDateTextColor;
    QColor todayOutlineColor;
    QColor disabledTextColor;
    QColor hoverColor;
    QColor focusRingColor;
    QFont headlineFont;
    QFont dayFont;
    int cornerRadius = 28;
    int cellSize = 40;
    int headerHeight = 64;
    int focusRingWidth = 2;
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
    QColor rowSelectedColor;
    QColor rowSelectedTextColor;
    QColor focusRingColor;
    QFont headerFont;
    QFont bodyFont;
    int rowHeight = 48;
    int headerHeight = 56;
    int cornerRadius = 12;
    int gridWidth = 1;
    int focusRingWidth = 2;
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
    QColor focusRingColor;

    QFont titleFont;
    QFont supportingFont;

    QSize itemSize = QSize(160, 144);

    int itemRadius = 16;
    int spacing = 12;
    int focusRingWidth = 2;
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

DatePickerSpec defaultDatePickerSpec();
TableSpec defaultTableSpec();
GridListSpec defaultGridListSpec();
CarouselSpec defaultCarouselSpec();

} // namespace QtMaterial
