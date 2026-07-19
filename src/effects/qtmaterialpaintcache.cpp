#include "qtmaterial/effects/qtmaterialpaintcache.h"

#include <QPixmapCache>

namespace QtMaterial {

QString QtMaterialPaintCache::makeKey(const QString& category, const QSize& size, qreal devicePixelRatio, quint64 revision)
{
    return QStringLiteral("qtm3:%1:%2x%3@%4:%5")
        .arg(category)
        .arg(size.width())
        .arg(size.height())
        .arg(devicePixelRatio, 0, 'f', 2)
        .arg(revision);
}

bool QtMaterialPaintCache::find(const QString& key, QPixmap* pixmap)
{
    return QPixmapCache::find(key, pixmap);
}

void QtMaterialPaintCache::insert(const QString& key, const QPixmap& pixmap)
{
    QPixmapCache::insert(key, pixmap);
}

void QtMaterialPaintCache::clear()
{
    QPixmapCache::clear();
}

} // namespace QtMaterial
