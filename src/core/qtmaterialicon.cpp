#include "qtmaterial/core/qtmaterialicon.h"

#include <QApplication>
#include <QHash>
#include <QMutex>
#include <QMutexLocker>
#include <QPainter>
#include <QScreen>
#include <QSvgRenderer>

namespace QtMaterial {
namespace {

struct IconCacheKey
{
    int icon = 0;
    int width = 0;
    int height = 0;
    QRgb rgba = 0;
    int dpr100 = 100;

    bool operator==(const IconCacheKey& other) const noexcept
    {
        return icon == other.icon
            && width == other.width
            && height == other.height
            && rgba == other.rgba
            && dpr100 == other.dpr100;
    }
};

size_t qHash(const IconCacheKey& key, size_t seed = 0) noexcept
{
    seed ^= ::qHash(key.icon, seed);
    seed ^= ::qHash(key.width, seed);
    seed ^= ::qHash(key.height, seed);
    seed ^= ::qHash(key.rgba, seed);
    seed ^= ::qHash(key.dpr100, seed);
    return seed;
}

class MaterialIconCache
{
public:
    QPixmap get(const IconCacheKey& key) const
    {
        QMutexLocker locker(&m_mutex);
        return m_cache.value(key);
    }

    void insert(const IconCacheKey& key, const QPixmap& pixmap)
    {
        QMutexLocker locker(&m_mutex);
        m_cache.insert(key, pixmap);
    }

    void clear()
    {
        QMutexLocker locker(&m_mutex);
        m_cache.clear();
    }

private:
    mutable QMutex m_mutex;
    QHash<IconCacheKey, QPixmap> m_cache;
};

Q_GLOBAL_STATIC(MaterialIconCache, g_materialIconCache)

qreal resolveDpr(qreal dpr)
{
    if (dpr > 0.0) {
        return dpr;
    }

    if (qApp && qApp->primaryScreen()) {
        return qApp->primaryScreen()->devicePixelRatio();
    }

    return 1.0;
}

QPixmap renderTintedSvg(const QString& path,
                        const QColor& color,
                        const QSize& logicalSize,
                        qreal dpr)
{
    if (!logicalSize.isValid()) {
        return QPixmap();
    }

    QSvgRenderer renderer(path);
    if (!renderer.isValid()) {
        return QPixmap();
    }

    const QSize pixelSize = QSize(qRound(logicalSize.width() * dpr),
                                  qRound(logicalSize.height() * dpr));

    QPixmap pixmap(pixelSize);
    pixmap.fill(Qt::transparent);
    pixmap.setDevicePixelRatio(dpr);

    {
        QPainter p(&pixmap);
        renderer.render(&p, QRectF(QPointF(0, 0), QSizeF(logicalSize)));
        p.setCompositionMode(QPainter::CompositionMode_SourceIn);
        p.fillRect(QRectF(QPointF(0, 0), QSizeF(logicalSize)), color);
    }

    return pixmap;
}

} // namespace

QString QtMaterialIcon::resourcePath(MaterialIcon icon)
{
    switch (icon) {
    case MaterialIcon::Add:
        return QStringLiteral(":/icons/material/add.svg");
    case MaterialIcon::AddCircle:
        return QStringLiteral(":/icons/material/add_circle.svg");
    case MaterialIcon::ArrowBackIos:
        return QStringLiteral(":/icons/material/arrow_back_ios.svg");
    case MaterialIcon::ArrowForward:
        return QStringLiteral(":/icons/material/arrow_forward.svg");
    case MaterialIcon::ArrowForwardIos:
        return QStringLiteral(":/icons/material/arrow_forward_ios.svg");
    case MaterialIcon::Cancel:
        return QStringLiteral(":/icons/material/cancel.svg");
    case MaterialIcon::CheckCircle:
        return QStringLiteral(":/icons/material/check_circle.svg");
    case MaterialIcon::ChevronLeft:
        return QStringLiteral(":/icons/material/chevron_left.svg");
    case MaterialIcon::ChevronRight:
        return QStringLiteral(":/icons/material/chevron_right.svg");
    case MaterialIcon::Close:
        return QStringLiteral(":/icons/material/close.svg");
    case MaterialIcon::Error:
        return QStringLiteral(":/icons/material/error.svg");
    case MaterialIcon::Home:
        return QStringLiteral(":/icons/material/home.svg");
    case MaterialIcon::MoreVert:
        return QStringLiteral(":/icons/material/more_vert.svg");
    case MaterialIcon::Refresh:
        return QStringLiteral(":/icons/material/refresh.svg");
    case MaterialIcon::Search:
        return QStringLiteral(":/icons/material/search.svg");
    case MaterialIcon::Settings:
        return QStringLiteral(":/icons/material/settings.svg");
    case MaterialIcon::Warning:
        return QStringLiteral(":/icons/material/warning.svg");
    }

    return QString();
}

QPixmap QtMaterialIcon::pixmap(MaterialIcon icon,
                               const QColor& color,
                               const QSize& size,
                               qreal devicePixelRatio)
{
    const qreal dpr = resolveDpr(devicePixelRatio);
    const IconCacheKey key{
        static_cast<int>(icon),
        size.width(),
        size.height(),
        color.rgba(),
        qRound(dpr * 100.0)
    };

    if (const QPixmap cached = g_materialIconCache->get(key); !cached.isNull()) {
        return cached;
    }

    const QString path = resourcePath(icon);
    const QPixmap rendered = renderTintedSvg(path, color, size, dpr);
    if (!rendered.isNull()) {
        g_materialIconCache->insert(key, rendered);
    }

    return rendered;
}

QIcon QtMaterialIcon::icon(MaterialIcon icon,
                           const QColor& color,
                           const QSize& size,
                           qreal devicePixelRatio)
{
    return QIcon(pixmap(icon, color, size, devicePixelRatio));
}

QIcon QtMaterialIcon::icon(MaterialIcon icon,
                           const QColor& normalColor,
                           const QColor& disabledColor,
                           const QSize& size,
                           qreal devicePixelRatio)
{
    QIcon result;
    result.addPixmap(pixmap(icon, normalColor, size, devicePixelRatio), QIcon::Normal, QIcon::Off);
    result.addPixmap(pixmap(icon, disabledColor, size, devicePixelRatio), QIcon::Disabled, QIcon::Off);
    return result;
}

void QtMaterialIcon::clearCache()
{
    g_materialIconCache->clear();
}

} // namespace QtMaterial
