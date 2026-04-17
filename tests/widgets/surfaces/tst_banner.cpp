#include <QtTest/QtTest>
#include "qtmaterial/widgets/surfaces/qtmaterialbanner.h"
class TestQtMaterialBanner : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
};
void TestQtMaterialBanner::constructs()
{
    QtMaterial::QtMaterialBanner widget;
    QVERIFY(true);
}
QTEST_MAIN(TestQtMaterialBanner)
#include "tst_banner.moc"
