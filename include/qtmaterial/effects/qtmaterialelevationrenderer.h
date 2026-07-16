#pragma once

#include <QColor>
#include <QPainterPath>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialelevationscale.h"

class QPainter;

namespace QtMaterial {

/**
 * Shared elevation renderer for arbitrary painter paths.
 *
 * ElevationStyle values are expressed in Qt logical coordinates. The painter
 * handles device-pixel scaling, so callers must not pre-scale blur or offsets.
 */
class QTMATERIAL3_EFFECTS_EXPORT QtMaterialElevationRenderer final
{
public:
    static ElevationStyle interpolate(
        const ElevationStyle& from,
        const ElevationStyle& to,
        qreal progress) noexcept;

    static void paintPathElevation(
        QPainter* painter,
        const QPainterPath& path,
        const QColor& shadowColor,
        const ElevationStyle& style);

    static void paintInterpolatedPathElevation(
        QPainter* painter,
        const QPainterPath& path,
        const QColor& shadowColor,
        const ElevationStyle& from,
        const ElevationStyle& to,
        qreal progress);
};

} // namespace QtMaterial
