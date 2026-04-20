#include <QtTest/QtTest>

#include "qtmaterial/widgets/surfaces/qtmaterialnavigationdrawer.h"

class tst_QtMaterialNavigationDrawer : public QObject
{
    Q_OBJECT

private slots:
    void construction();
    void edgeApi();
    void escapeCloses();
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
    // QCOMPARE(drawer.edge(), QtMaterialNavigationDrawer::Edge::Left);
    // drawer.setEdge(QtMaterialNavigationDrawer::Edge::Right);
    // QCOMPARE(drawer.edge(), QtMaterialNavigationDrawer::Edge::Right);
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

QTEST_MAIN(tst_QtMaterialNavigationDrawer)
#include "tst_navigationdrawer.moc"
