#pragma once
#include <QColor>
#include <QPainterPath>
#include <QRectF>
#include "qtmaterial/qtmaterialglobal.h"
namespace QtMaterial {
class QTMATERIAL3_EFFECTS_EXPORT QtMaterialStateLayerPainter
{
public:
    static void paintRect(QPainter* painter, const QRectF& rect, const QColor& color, qreal opacity, qreal radius);
    static void paintPath(QPainter* painter, const QPainterPath& path, const QColor& color, qreal opacity);
};
} // namespace QtMaterial
