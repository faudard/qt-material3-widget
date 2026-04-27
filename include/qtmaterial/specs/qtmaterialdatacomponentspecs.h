#pragma once

#include <QColor>
#include <QFont>
#include <QMargins>
#include <QSize>

namespace QtMaterial {

struct DatePickerSpec {
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

struct TableSpec {
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

struct GridListSpec {
    QColor backgroundColor;
    QColor foregroundColor;
    QColor itemBackgroundColor;
    QColor itemHoverColor;
    QColor itemSelectedColor;
    QColor itemSelectedTextColor;
    QColor supportingTextColor;
    QColor focusRingColor;
    QFont titleFont;
    QFont supportingFont;
    QSize itemSize = QSize(160, 144);
    int itemRadius = 16;
    int spacing = 12;
    int focusRingWidth = 2;
};

struct CarouselSpec {
    QColor backgroundColor;
    QColor foregroundColor;
    QColor pageIndicatorColor;
    QColor activePageIndicatorColor;
    QColor navigationButtonColor;
    QColor navigationButtonTextColor;
    QColor focusRingColor;
    QFont labelFont;
    QSize preferredPageSize = QSize(360, 220);
    int cornerRadius = 28;
    int pageSpacing = 12;
    int indicatorSize = 8;
    int focusRingWidth = 2;
};

DatePickerSpec defaultDatePickerSpec();
TableSpec defaultTableSpec();
GridListSpec defaultGridListSpec();
CarouselSpec defaultCarouselSpec();

} // namespace QtMaterial
