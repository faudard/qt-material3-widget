#pragma once
#include <QColor>
#include "qtmaterial/qtmaterialglobal.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT CarouselSpec {
    QColor containerColor;
    QColor indicatorColor;
    QColor activeIndicatorColor;
    int pageSpacing = 12;
};
}
