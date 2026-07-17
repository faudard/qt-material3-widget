#include "qtmaterial/specs/qtmaterialdatacomponentspecs.h"

#include <QCoreApplication>
#include <QGuiApplication>

namespace QtMaterial {
namespace {
QFont appFont(int pointSizeDelta = 0, int weight = QFont::Normal)
{
    const auto *guiApplication =
        qobject_cast<QGuiApplication *>(QCoreApplication::instance());

    QFont font =
        guiApplication ? QGuiApplication::font() : QFont();

    if (font.pointSize() > 0 && pointSizeDelta != 0) {
        font.setPointSize(qMax(1, font.pointSize() + pointSizeDelta));
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    font.setWeight(static_cast<QFont::Weight>(weight));
#else
    font.setWeight(weight);
#endif

    return font;
}
}

DatePickerSpec defaultDatePickerSpec()
{
    DatePickerSpec spec;
    spec.backgroundColor = QColor("#FFFBFE");
    spec.foregroundColor = QColor("#1D1B20");
    spec.headlineColor = QColor("#1D1B20");
    spec.selectedDateColor = QColor("#6750A4");
    spec.selectedDateTextColor = QColor("#FFFFFF");
    spec.todayOutlineColor = QColor("#6750A4");
    spec.disabledTextColor = QColor(0x1D, 0x1B, 0x20, 0x61);
    spec.hoverColor = QColor(0x67, 0x50, 0xA4, 0x14);
    spec.focusRingColor = QColor("#6750A4");
    spec.headlineFont = appFont(4, QFont::DemiBold);
    spec.dayFont = appFont(0, QFont::Medium);
    return spec;
}

TableSpec defaultTableSpec()
{
    TableSpec spec;
    spec.backgroundColor = QColor("#FFFBFE");
    spec.foregroundColor = QColor("#1D1B20");
    spec.headerBackgroundColor = QColor("#F7F2FA");
    spec.headerForegroundColor = QColor("#49454F");
    spec.gridColor = QColor("#CAC4D0");
    spec.rowHoverColor = QColor(0x67, 0x50, 0xA4, 0x0F);
    spec.rowSelectedColor = QColor(0x67, 0x50, 0xA4, 0x1F);
    spec.rowSelectedTextColor = QColor("#1D1B20");
    spec.focusRingColor = QColor("#6750A4");
    spec.headerFont = appFont(0, QFont::DemiBold);
    spec.bodyFont = appFont();
    return spec;
}

GridListSpec defaultGridListSpec()
{
    GridListSpec spec;
    spec.backgroundColor = QColor("#FFFBFE");
    spec.foregroundColor = QColor("#1D1B20");
    spec.itemBackgroundColor = QColor("#F7F2FA");
    spec.itemHoverColor = QColor(0x67, 0x50, 0xA4, 0x0F);
    spec.itemSelectedColor = QColor(0x67, 0x50, 0xA4, 0x1F);
    spec.itemSelectedTextColor = QColor("#1D1B20");
    spec.supportingTextColor = QColor("#49454F");
    spec.focusRingColor = QColor("#6750A4");
    spec.titleFont = appFont(0, QFont::DemiBold);
    spec.supportingFont = appFont(-1);
    return spec;
}

CarouselSpec defaultCarouselSpec()
{
    CarouselSpec spec;

    spec.backgroundColor = QColor("#FFFBFE");
    spec.foregroundColor = QColor("#1D1B20");
    spec.pageIndicatorColor = QColor("#CAC4D0");
    spec.activePageIndicatorColor = QColor("#6750A4");
    spec.navigationButtonColor = QColor("#6750A4");
    spec.navigationButtonTextColor = QColor("#FFFFFF");
    spec.focusRingColor = QColor("#6750A4");
    spec.labelFont = appFont(0, QFont::DemiBold);

    spec.itemBackgroundColor = QColor("#F7F2FA");
    spec.itemHoverColor = QColor(0x1D, 0x1B, 0x20, 0x14);
    spec.itemPressedColor = QColor(0x1D, 0x1B, 0x20, 0x1F);
    spec.itemSelectedColor = QColor("#E8DEF8");
    spec.itemSelectedTextColor = QColor("#1D192B");
    spec.supportingTextColor = QColor("#49454F");
    spec.disabledTextColor = QColor(0x1D, 0x1B, 0x20, 0x61);
    spec.outlineColor = QColor("#CAC4D0");
    spec.selectedOutlineColor = QColor("#6750A4");
    spec.iconColor = QColor("#49454F");
    spec.selectedIconColor = QColor("#1D192B");
    spec.supportingFont = appFont(-1);

    spec.preferredPageSize = QSize(360, 220);
    spec.itemSize = QSize(176, 128);
    spec.minimumItemSize = QSize(72, 72);
    spec.outerMargins = QMargins(16, 16, 16, 16);
    spec.contentMargins = QMargins(14, 12, 14, 12);
    spec.shapeRole = ShapeRole::Large;
    spec.cornerRadius = 18.0;
    spec.pageSpacing = 12;
    spec.indicatorSize = 8;
    spec.focusRingWidth = 2;
    spec.outlineWidth = 1;
    spec.selectedOutlineWidth = 2;
    spec.iconSize = 28;
    spec.iconSpacing = 10;
    spec.supportingTopSpacing = 6;

    return spec;
}

} // namespace QtMaterial
