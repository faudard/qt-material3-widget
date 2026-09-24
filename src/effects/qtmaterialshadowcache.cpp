#include "qtmaterial/effects/private/qtmaterialshadowcache_p.h"

#include <QPixmapCache>
#include <QtGlobal>

namespace QtMaterial {

QtMaterialShadowCache::QtMaterialShadowCache() = default;
QtMaterialShadowCache::~QtMaterialShadowCache() = default;

QString QtMaterialShadowCache::keyFor(
    int width,
    int height,
    qreal radius,
    int blurRadius,
    int yOffset,
    const QColor& color,
    qreal devicePixelRatio) const
{
    const qreal dpr = qMax<qreal>(0.25, devicePixelRatio);
    return QStringLiteral("shadow:%1:%2:%3:%4:%5:%6@%7")
        .arg(width)
        .arg(height)
        .arg(radius, 0, 'f', 3)
        .arg(blurRadius)
        .arg(yOffset)
        .arg(color.rgba())
        .arg(dpr, 0, 'f', 3);
}

bool QtMaterialShadowCache::find(
    const QString& key,
    QPixmap* out) const
{
    return QPixmapCache::find(key, out);
}

void QtMaterialShadowCache::insert(
    const QString& key,
    const QPixmap& pixmap) const
{
    QPixmapCache::insert(key, pixmap);
}

} // namespace QtMaterial
