#include <QtTest>

#include "qtmaterial/effects/qtmaterialpaintcache.h"

using namespace QtMaterial;

class tst_PaintCache : public QObject
{
    Q_OBJECT

private slots:
    void createsStableKeys();
    void storesAndClearsPixmaps();
};

void tst_PaintCache::createsStableKeys()
{
    const QString key = QtMaterialPaintCache::makeKey(
        QStringLiteral("button"),
        QSize(20, 10),
        2.0,
        42);

    QCOMPARE(key, QStringLiteral("qtm3:button:20x10@2.00:42"));
}

void tst_PaintCache::storesAndClearsPixmaps()
{
    QtMaterialPaintCache::clear();

    QPixmap source(8, 8);
    source.fill(Qt::transparent);

    const QString key = QtMaterialPaintCache::makeKey(
        QStringLiteral("unit"),
        source.size(),
        source.devicePixelRatio(),
        1);

    QtMaterialPaintCache::insert(key, source);

    QPixmap cached;
    QVERIFY(QtMaterialPaintCache::find(key, &cached));
    QCOMPARE(cached.size(), source.size());

    QtMaterialPaintCache::clear();
    QVERIFY(!QtMaterialPaintCache::find(key, &cached));
}

QTEST_MAIN(tst_PaintCache)
#include "tst_paintcache.moc"
