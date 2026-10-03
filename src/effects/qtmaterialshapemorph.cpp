#include "qtmaterial/effects/private/qtmaterialshapemorph_p.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace QtMaterial {
namespace {

bool singleContour(const QPainterPath& path)
{
    int contours = 0;
    for (int i = 0; i < path.elementCount(); ++i) {
        const auto element = path.elementAt(i);
        if (!std::isfinite(element.x) || !std::isfinite(element.y)) { return false; }
        if (element.isMoveTo()) { ++contours; }
    }
    return contours == 1 && !path.isEmpty();
}

QPolygonF sample(const QPainterPath& path, int count)
{
    QPolygonF points;
    const qreal length = path.length();
    if (!std::isfinite(length) || length <= 0) { return points; }
    points.reserve(count);
    for (int i = 0; i < count; ++i) {
        points.push_back(path.pointAtPercent(path.percentAtLength(length * i / count)));
    }
    return points;
}

qreal signedArea(const QPolygonF& points)
{
    qreal area = 0;
    for (int i = 0; i < points.size(); ++i) {
        const QPointF a = points.at(i);
        const QPointF b = points.at((i + 1) % points.size());
        area += a.x() * b.y() - a.y() * b.x();
    }
    return area / 2;
}

QPolygonF align(const QPolygonF& source, QPolygonF target)
{
    if (signedArea(source) * signedArea(target) < 0) {
        std::reverse(target.begin(), target.end());
    }
    const int count = source.size();
    int bestOffset = 0;
    qreal bestDistance = std::numeric_limits<qreal>::max();
    for (int offset = 0; offset < count; ++offset) {
        qreal distance = 0;
        for (int i = 0; i < count; ++i) {
            const QPointF delta = source.at(i) - target.at((i + offset) % count);
            distance += delta.x() * delta.x() + delta.y() * delta.y();
        }
        if (distance < bestDistance) { bestDistance = distance; bestOffset = offset; }
    }
    QPolygonF aligned;
    aligned.reserve(count);
    for (int i = 0; i < count; ++i) { aligned.push_back(target.at((i + bestOffset) % count)); }
    return aligned;
}

} // namespace

QtMaterialShapeMorph::QtMaterialShapeMorph(int sampleCount)
    : m_sampleCount(qBound(16, sampleCount, 256))
{
}

bool QtMaterialShapeMorph::setShapes(const QPainterPath& source, const QPainterPath& target)
{
    clear();
    if (!singleContour(source) || !singleContour(target)) { return false; }
    m_source = source;
    m_target = target;
    m_source.closeSubpath();
    m_target.closeSubpath();
    m_sourcePoints = sample(m_source, m_sampleCount);
    m_targetPoints = sample(m_target, m_sampleCount);
    if (m_sourcePoints.size() != m_sampleCount || m_targetPoints.size() != m_sampleCount
        || qFuzzyIsNull(signedArea(m_sourcePoints)) || qFuzzyIsNull(signedArea(m_targetPoints))) {
        clear();
        return false;
    }
    m_targetPoints = align(m_sourcePoints, m_targetPoints);
    return true;
}

bool QtMaterialShapeMorph::isValid() const noexcept { return !m_sourcePoints.isEmpty(); }
void QtMaterialShapeMorph::clear()
{
    m_source = QPainterPath();
    m_target = QPainterPath();
    m_sourcePoints.clear();
    m_targetPoints.clear();
}

QPainterPath QtMaterialShapeMorph::pathAt(qreal progress) const
{
    if (!isValid()) { return {}; }
    if (!std::isfinite(progress) || progress <= 0) { return m_source; }
    if (progress >= 1) { return m_target; }
    QPolygonF points;
    points.reserve(m_sampleCount);
    for (int i = 0; i < m_sampleCount; ++i) {
        points.push_back(m_sourcePoints.at(i) * (1 - progress) + m_targetPoints.at(i) * progress);
    }
    QPainterPath path;
    path.setFillRule(m_source.fillRule());
    path.addPolygon(points);
    path.closeSubpath();
    return path;
}

QPainterPath QtMaterialShapeMorph::roundedRectangle(const QRectF& rect, qreal radius)
{
    QPainterPath path;
    if (!std::isfinite(radius) || !std::isfinite(rect.x()) || !std::isfinite(rect.y())
        || !std::isfinite(rect.width()) || !std::isfinite(rect.height())) { return path; }
    const QRectF bounds = rect.normalized();
    if (bounds.isEmpty()) { return path; }
    radius = qBound(qreal(0), radius, qMin(bounds.width(), bounds.height()) / 2);
    path.addRoundedRect(bounds, radius, radius);
    return path;
}

} // namespace QtMaterial
