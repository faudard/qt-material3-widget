#include <QtTest/QtTest>
#include "qtmaterial/widgets/buttons/qtmaterialfab.h"
class TestQtMaterialFab : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
};
void TestQtMaterialFab::constructs()
{
    QtMaterial::QtMaterialFab widget;
    QVERIFY(true);
}
QTEST_MAIN(TestQtMaterialFab)
#include "tst_fab.moc"
