#include <QtTest/QtTest>
#include <QColor>
#include <QPixmap>
#include <QString>

#include "qtmaterial/effects/qtmaterialshadowcache.h"

class benchmark_ShadowCachePipeline : public QObject
{
    Q_OBJECT

private slots:
    void keyGeneration()
    {
        QtMaterial::QtMaterialShadowCache cache;
        const QColor color(0, 0, 0, 80);

        QBENCHMARK {
            const QString key = cache.keyFor(160, 96, 12.0, 24, 4, color);
            QVERIFY(!key.isEmpty());
        }
    }

    void insertAndFindPixmap()
    {
        QtMaterial::QtMaterialShadowCache cache;
        QPixmap pixmap(96, 64);
        pixmap.fill(Qt::transparent);
        const QString key = cache.keyFor(96, 64, 8.0, 16, 2, QColor(0, 0, 0, 64));

        QBENCHMARK {
            cache.insert(key, pixmap);
            QPixmap out;
            const bool found = cache.find(key, &out);
            QVERIFY(found);
            QVERIFY(!out.isNull());
        }
    }
};

QTEST_MAIN(benchmark_ShadowCachePipeline)
#include "benchmark_shadow_cache_pipeline.moc"
