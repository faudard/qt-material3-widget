#include <QtTest/QtTest>
#include <QImage>
#include <QPainter>
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
class tst_StateLayerPainter : public QObject
{
    Q_OBJECT
private slots:
    void paintsIntoImage();
};
void tst_StateLayerPainter::paintsIntoImage()
{
    QImage image(64, 64, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    QtMaterial::QtMaterialStateLayerPainter::paintRect(&painter, QRectF(8, 8, 48, 48), QColor("#6750A4"), 0.12, 12.0);
    QVERIFY(!image.isNull());
}
QTEST_MAIN(tst_StateLayerPainter)
#include "tst_statelayerpainter.moc"
