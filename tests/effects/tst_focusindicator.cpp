#include <QtTest/QtTest>
#include <QImage>
#include <QPainter>
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
class tst_FocusIndicator : public QObject
{
    Q_OBJECT
private slots:
    void paintsIntoImage();
};
void tst_FocusIndicator::paintsIntoImage()
{
    QImage image(64, 64, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    QtMaterial::QtMaterialFocusIndicator::paintRectFocusRing(&painter, QRectF(8, 8, 48, 48), QColor("#00639B"), 12.0, 2.0);
    QVERIFY(!image.isNull());
}
QTEST_MAIN(tst_FocusIndicator)
#include "tst_focusindicator.moc"
