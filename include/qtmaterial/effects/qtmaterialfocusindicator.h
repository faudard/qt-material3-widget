#pragma once
#include <QColor>
#include <QPainterPath>
#include <QRectF>
#include "qtmaterial/qtmaterialglobal.h"
namespace QtMaterial {
class QTMATERIAL3_EFFECTS_EXPORT QtMaterialFocusIndicator
{
public:
    static void paintRectFocusRing(QPainter* painter, const QRectF& rect, const QColor& color, qreal radius, qreal width);
    static void paintPathFocusRing(QPainter* painter, const QPainterPath& path, const QColor& color, qreal width);
};
} // namespace QtMaterial
