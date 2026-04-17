#include <QtTest/QtTest>
#include "qtmaterial/widgets/inputs/qtmaterialdatefield.h"
class TestQtMaterialDateField : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
};
void TestQtMaterialDateField::constructs()
{
    QtMaterial::QtMaterialDateField widget;
    QVERIFY(true);
}
QTEST_MAIN(TestQtMaterialDateField)
#include "tst_datefield.moc"
