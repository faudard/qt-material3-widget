#pragma once

#include <QPainterPath>
#include <QPolygonF>
#include <QRectF>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

// Internal paint primitive, independent of widgets and animation timing. Shapes
// must be finite, single-contour outlines in the same coordinate system.
class QTMATERIAL3_EFFECTS_EXPORT QtMaterialShapeMorph final
{
public:
    explicit QtMaterialShapeMorph(int sampleCount = 96);
    bool setShapes(const QPainterPath& source, const QPainterPath& target);
    bool isValid() const noexcept;
    void clear();
    QPainterPath pathAt(qreal progress) const;

    static QPainterPath roundedRectangle(const QRectF& rect, qreal radius);

private:
    int m_sampleCount;
    QPainterPath m_source;
    QPainterPath m_target;
    QPolygonF m_sourcePoints;
    QPolygonF m_targetPoints;
};

} // namespace QtMaterial
