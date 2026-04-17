#include <QtTest/QtTest>
#include "qtmaterial/widgets/surfaces/qtmaterialbottomappbar.h"
class TestQtMaterialBottomAppBar : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
};
void TestQtMaterialBottomAppBar::constructs()
{
    QtMaterial::QtMaterialBottomAppBar widget;
    QVERIFY(true);
}
QTEST_MAIN(TestQtMaterialBottomAppBar)
#include "tst_bottomappbar.moc"
