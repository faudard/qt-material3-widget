#include <QtTest/QtTest>
#include "qtmaterial/widgets/data/qtmateriallist.h"
class TestQtMaterialList : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
};
void TestQtMaterialList::constructs()
{
    QtMaterial::QtMaterialList widget;
    QVERIFY(true);
}
QTEST_MAIN(TestQtMaterialList)
#include "tst_list.moc"
