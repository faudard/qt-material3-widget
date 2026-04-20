#include <QtTest/QtTest>

#include <QImage>
#include <QPainter>

#include "qtmaterial/widgets/surfaces/qtmaterialbottomappbar.h"

class tst_QtMaterialBottomAppBarRender : public QObject
{
    Q_OBJECT

private slots:
    void rendersDefaultFrame();
};

void tst_QtMaterialBottomAppBarRender::rendersDefaultFrame()
{
    QtMaterialBottomAppBar bar(QStringLiteral("Home"));
    bar.resize(400, 96);
    bar.show();
    QVERIFY(QTest::qWaitForWindowExposed(&bar));

    QImage image(bar.size() * bar.devicePixelRatioF(), QImage::Format_ARGB32_Premultiplied);
    image.setDevicePixelRatio(bar.devicePixelRatioF());
    image.fill(Qt::transparent);

    QPainter painter(&image);
    bar.render(&painter);
    painter.end();

    QVERIFY(!image.isNull());
    QVERIFY(image.width() > 0);
    QVERIFY(image.height() > 0);
}

QTEST_MAIN(tst_QtMaterialBottomAppBarRender)
#include "tst_bottomappbar_render.moc"
