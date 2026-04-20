#include <QtTest/QtTest>

#include <QImage>
#include <QPainter>

#include "qtmaterial/widgets/surfaces/qtmaterialtopappbar.h"

class tst_QtMaterialTopAppBarRender : public QObject
{
    Q_OBJECT

private slots:
    void rendersDefaultFrame();
};

void tst_QtMaterialTopAppBarRender::rendersDefaultFrame()
{
    QtMaterialTopAppBar bar(QStringLiteral("Library"));
    bar.resize(360, 64);
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

QTEST_MAIN(tst_QtMaterialTopAppBarRender)
#include "tst_topappbar_render.moc"
