#include <QImage>
#include <QPainter>
#include <QtTest/QtTest>

#include "qtmaterial/effects/qtmaterialelevationrenderer.h"

using namespace QtMaterial;

class tst_ElevationRenderer : public QObject
{
    Q_OBJECT

private slots:
    void interpolatesAndClamps();
    void levelZeroPaintsNothing();
    void paintsOutsideSourcePath();
};

void tst_ElevationRenderer::interpolatesAndClamps()
{
    const ElevationStyle from{0, 0, 0.0};
    const ElevationStyle to{8, 4, 0.08};

    const ElevationStyle middle =
        QtMaterialElevationRenderer::interpolate(from, to, 0.5);
    QCOMPARE(middle.shadowBlur, 4);
    QCOMPARE(middle.shadowYOffset, 2);
    QVERIFY(qAbs(middle.tonalOverlayOpacity - 0.04) < 0.0001);

    QCOMPARE(
        QtMaterialElevationRenderer::interpolate(from, to, -1.0).shadowBlur,
        0);
    QCOMPARE(
        QtMaterialElevationRenderer::interpolate(from, to, 2.0).shadowBlur,
        8);
}

void tst_ElevationRenderer::levelZeroPaintsNothing()
{
    QImage image(80, 60, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    QPainterPath path;
    path.addRoundedRect(QRectF(20, 15, 40, 24), 12, 12);

    QPainter painter(&image);
    QtMaterialElevationRenderer::paintPathElevation(
        &painter,
        path,
        QColor(Qt::black),
        ElevationStyle{});
    painter.end();

    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QCOMPARE(qAlpha(image.pixel(x, y)), 0);
        }
    }
}

void tst_ElevationRenderer::paintsOutsideSourcePath()
{
    QImage image(100, 80, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    const QRectF sourceRect(30, 20, 40, 24);
    QPainterPath path;
    path.addRoundedRect(sourceRect, 12, 12);

    QPainter painter(&image);
    QtMaterialElevationRenderer::paintPathElevation(
        &painter,
        path,
        QColor(Qt::black),
        ElevationStyle{8, 3, 0.08});
    painter.end();

    bool foundShadowOutside = false;
    for (int y = 0; y < image.height() && !foundShadowOutside; ++y) {
        for (int x = 0; x < image.width(); ++x) {
            if (sourceRect.contains(QPointF(x, y))) {
                continue;
            }
            if (qAlpha(image.pixel(x, y)) > 0) {
                foundShadowOutside = true;
                break;
            }
        }
    }

    QVERIFY(foundShadowOutside);
}

QTEST_MAIN(tst_ElevationRenderer)

#include "tst_elevationrenderer.moc"
