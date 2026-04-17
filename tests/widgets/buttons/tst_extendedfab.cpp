#include <QtTest/QtTest>
#include "qtmaterial/widgets/buttons/qtmaterialextendedfab.h"
class TestQtMaterialExtendedFab : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
};
void TestQtMaterialExtendedFab::constructs()
{
    QtMaterial::QtMaterialExtendedFab widget;
    QVERIFY(true);
}
QTEST_MAIN(TestQtMaterialExtendedFab)
#include "tst_extendedfab.moc"
