#pragma once
#include <QColor>
#include <QSize>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
namespace QtMaterial {
struct QTMATERIAL3_SPECS_EXPORT SegmentedButtonSpec {
    QColor containerColor;
    QColor selectedContainerColor;
    QColor labelColor;
    QColor selectedLabelColor;
    QColor outlineColor;
    ShapeRole shapeRole = ShapeRole::Full;
    QSize touchTarget = QSize(48, 40);
    int segmentHeight = 40;
};
}
