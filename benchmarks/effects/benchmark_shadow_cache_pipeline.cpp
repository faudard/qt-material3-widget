#include <QtTest/QtTest>
#include <QColor>
#include <QPixmap>
#include <QString>

#include "qtmaterial/effects/qtmaterialshadowcache.h"

class benchmark_ShadowCachePipeline : public QObject
{
    Q_OBJECT

private slots:
    void keyGeneration_data()
    {
        QTest::addColumn<qreal>("devicePixelRatio");
        QTest::newRow("dpr-1x") << 1.0;
        QTest::newRow("dpr-2x") << 2.0;
    }

    void keyGeneration()
    {
        QFETCH(qreal, devicePixelRatio);

        QtMaterial::QtMaterialShadowCache cache;
        const QColor color(0, 0, 0, 80);

        QBENCHMARK {
            const QString key =
                cache.keyFor(
                    160,
                    96,
                    12.0,
                    24,
                    4,
                    color,
                    devicePixelRatio);
            QVERIFY(!key.isEmpty());
        }
    }

    void dprProducesDistinctCacheIdentity()
    {
        QtMaterial::QtMaterialShadowCache cache;
        const QColor color(0, 0, 0, 80);
        const QString one =
            cache.keyFor(160, 96, 12.0, 24, 4, color, 1.0);
        const QString two =
            cache.keyFor(160, 96, 12.0, 24, 4, color, 2.0);

        QVERIFY(one != two);
    }

    void insertAndFindPixmap()
    {
        QtMaterial::QtMaterialShadowCache cache;
        QPixmap pixmap(192, 128);
        pixmap.setDevicePixelRatio(2.0);
        pixmap.fill(Qt::transparent);
        const QString key = cache.keyFor(
            96,
            64,
            8.0,
            16,
            2,
            QColor(0, 0, 0, 64),
            pixmap.devicePixelRatio());

        QBENCHMARK {
            cache.insert(key, pixmap);
            QPixmap out;
            const bool found = cache.find(key, &out);
            QVERIFY(found);
            QVERIFY(!out.isNull());
            QCOMPARE(out.devicePixelRatio(), 2.0);
        }
    }
};

QTEST_MAIN(benchmark_ShadowCachePipeline)
#include "benchmark_shadow_cache_pipeline.moc"
