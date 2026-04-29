#include <QtTest/QtTest>
#include <QPixmap>

#include "qtmaterial/core/qtmaterialpaintcache.h"

class BenchmarkPaintCache : public QObject {
    Q_OBJECT
private slots:
    void makeKey()
    {
        QBENCHMARK {
            auto key = QtMaterial::QtMaterialPaintCache::makeKey(QStringLiteral("shadow"), QSize(64, 64), 2.0, 42);
            QVERIFY(!key.isEmpty());
        }
    }

    void insertAndFind()
    {
        QPixmap pixmap(16, 16);
        pixmap.fill(Qt::transparent);
        const QString key = QtMaterial::QtMaterialPaintCache::makeKey(QStringLiteral("test"), pixmap.size(), 1.0, 1);
        QBENCHMARK {
            QtMaterial::QtMaterialPaintCache::insert(key, pixmap);
            QPixmap found;
            QVERIFY(QtMaterial::QtMaterialPaintCache::find(key, &found));
        }
    }
};

QTEST_MAIN(BenchmarkPaintCache)
#include "benchmark_paintcache.moc"
