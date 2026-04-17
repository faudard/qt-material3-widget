#pragma once
#include <QColor>
#include <QRectF>
#include "qtmaterial/qtmaterialglobal.h"
class QPainter;
namespace QtMaterial {
class QTMATERIAL3_EFFECTS_EXPORT QtMaterialShadowRenderer
{
public:
    static void paintRoundedShadow(QPainter* painter, const QRectF& rect, qreal radius, const QColor& shadowColor, int blurRadius, int yOffset);
};
} // namespace QtMaterial
