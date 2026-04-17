#include <QtTest/QtTest>
#include "qtmaterial/widgets/surfaces/qtmaterialbottomsheet.h"
class TestQtMaterialBottomSheet : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
};
void TestQtMaterialBottomSheet::constructs()
{
    QtMaterial::QtMaterialBottomSheet widget;
    QVERIFY(true);
}
QTEST_MAIN(TestQtMaterialBottomSheet)
#include "tst_bottomsheet.moc"
