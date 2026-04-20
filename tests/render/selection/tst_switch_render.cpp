#include <QtTest/QtTest>
#include <QImage>
#include <QPainter>

#include "qtmaterial/widgets/selection/qtmaterialswitch.h"

class tst_QtMaterialSwitchRender : public QObject
{
    Q_OBJECT

private slots:
    void rendersDefaultState();
};

void tst_QtMaterialSwitchRender::rendersDefaultState()
{
    QtMaterial::QtMaterialSwitch w(QStringLiteral("Bluetooth"));
    w.resize(w.sizeHint());

    QImage image(w.size(), QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    w.render(&painter);

    QVERIFY(!image.isNull());
}

QTEST_MAIN(tst_QtMaterialSwitchRender)
#include "tst_switch_render.moc"
