#include <QtTest/QtTest>
#include "qtmaterial/widgets/surfaces/qtmaterialtopappbar.h"
class TestQtMaterialTopAppBar : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
};
void TestQtMaterialTopAppBar::constructs()
{
    QtMaterial::QtMaterialTopAppBar widget;
    QVERIFY(true);
}
QTEST_MAIN(TestQtMaterialTopAppBar)
#include "tst_topappbar.moc"
