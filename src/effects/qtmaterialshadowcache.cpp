#include "qtmaterial/effects/qtmaterialshadowcache.h"
#include <QPixmapCache>
namespace QtMaterial {
QtMaterialShadowCache::QtMaterialShadowCache() = default;
QtMaterialShadowCache::~QtMaterialShadowCache() = default;
QString QtMaterialShadowCache::keyFor(int width, int height, qreal radius, int blurRadius, int yOffset, const QColor& color) const
{
    return QStringLiteral("shadow:%1:%2:%3:%4:%5:%6").arg(width).arg(height).arg(radius).arg(blurRadius).arg(yOffset).arg(color.rgba());
}
bool QtMaterialShadowCache::find(const QString& key, QPixmap* out) const { return QPixmapCache::find(key, out); }
void QtMaterialShadowCache::insert(const QString& key, const QPixmap& pixmap) const { QPixmapCache::insert(key, pixmap); }
} // namespace QtMaterial
