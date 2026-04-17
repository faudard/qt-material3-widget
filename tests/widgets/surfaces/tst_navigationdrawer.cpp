#include <QtTest/QtTest>
#include "qtmaterial/widgets/surfaces/qtmaterialnavigationdrawer.h"
class TestQtMaterialNavigationDrawer : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
};
void TestQtMaterialNavigationDrawer::constructs()
{
    QtMaterial::QtMaterialNavigationDrawer widget;
    QVERIFY(true);
}
QTEST_MAIN(TestQtMaterialNavigationDrawer)
#include "tst_navigationdrawer.moc"
