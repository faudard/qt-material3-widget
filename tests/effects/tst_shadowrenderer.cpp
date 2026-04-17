#include <QtTest/QtTest>
#include <QImage>
#include <QPainter>
#include "qtmaterial/effects/qtmaterialshadowrenderer.h"
class tst_ShadowRenderer : public QObject
{
    Q_OBJECT
private slots:
    void paintsIntoImage();
};
void tst_ShadowRenderer::paintsIntoImage()
{
    QImage image(128, 96, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    QtMaterial::QtMaterialShadowRenderer::paintRoundedShadow(&painter, QRectF(16, 16, 80, 48), 16.0, QColor(0,0,0,80), 12, 4);
    QVERIFY(!image.isNull());
}
QTEST_MAIN(tst_ShadowRenderer)
#include "tst_shadowrenderer.moc"
