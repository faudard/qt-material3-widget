#include <QtTest/QtTest>
#include <QImage>
#include <QPainter>

#include "qtmaterial/widgets/surfaces/qtmaterialnavigationdrawer.h"

class tst_QtMaterialNavigationDrawerRender : public QObject
{
    Q_OBJECT

private slots:
    void renderDefault();
};

void tst_QtMaterialNavigationDrawerRender::renderDefault()
{
    QtMaterialNavigationDrawer drawer;
    drawer.resize(360, 640);
    drawer.show();

    QImage image(drawer.size(), QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    drawer.render(&painter);

    QVERIFY(!image.isNull());
}

QTEST_MAIN(tst_QtMaterialNavigationDrawerRender)
#include "tst_navigationdrawer_render.moc"
