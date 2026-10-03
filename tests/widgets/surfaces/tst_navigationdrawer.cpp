#include <QtTest/QtTest>
#include <QPixmap>

#include "qtmaterial/widgets/surfaces/qtmaterialnavigationdrawer.h"

class tst_QtMaterialNavigationDrawer : public QObject
{
    Q_OBJECT

private slots:
    void construction();
    void edgeApi();
    void escapeCloses();
    void rtlAndDpr2RenderingContract();
};

void tst_QtMaterialNavigationDrawer::construction()
{
    QtMaterial::QtMaterialNavigationDrawer drawer;
    QVERIFY(!drawer.isOpen());
    QVERIFY(drawer.minimumSizeHint().width() > 0);
}

void tst_QtMaterialNavigationDrawer::edgeApi()
{
    QtMaterial::QtMaterialNavigationDrawer drawer;
    QCOMPARE(drawer.edge(), QtMaterial::QtMaterialNavigationDrawer::Edge::Left);
    drawer.setEdge(QtMaterial::QtMaterialNavigationDrawer::Edge::Right);
    QCOMPARE(drawer.edge(), QtMaterial::QtMaterialNavigationDrawer::Edge::Right);
}

void tst_QtMaterialNavigationDrawer::escapeCloses()
{
    QWidget host;
    host.resize(800, 600);
    host.show();

    QtMaterial::QtMaterialNavigationDrawer drawer(&host);
    drawer.setGeometry(host.rect());
    drawer.open();
    QVERIFY(drawer.isOpen());

    QTest::keyClick(&drawer, Qt::Key_Escape);
    QVERIFY(!drawer.isOpen());
}

void tst_QtMaterialNavigationDrawer::rtlAndDpr2RenderingContract()
{
    QtMaterial::QtMaterialNavigationDrawer widget;
    widget.setLayoutDirection(Qt::RightToLeft);
    widget.resize(qMax(320, widget.sizeHint().width()), qMax(96, widget.sizeHint().height()));
    QCOMPARE(widget.layoutDirection(), Qt::RightToLeft);
    QPixmap pixmap(widget.width() * 2, widget.height() * 2);
    pixmap.setDevicePixelRatio(2.0);
    pixmap.fill(Qt::transparent);
    widget.render(&pixmap);
    QVERIFY(!pixmap.isNull());
    QCOMPARE(pixmap.devicePixelRatio(), 2.0);
}

QTEST_MAIN(tst_QtMaterialNavigationDrawer)
#include "tst_navigationdrawer.moc"
