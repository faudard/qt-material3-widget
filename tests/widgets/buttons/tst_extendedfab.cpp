#include <QtTest/QtTest>
#include "qtmaterial/widgets/buttons/qtmaterialextendedfab.h"
class TestQtMaterialExtendedFab : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
    void keepsMinimumHeight();
};
void TestQtMaterialExtendedFab::constructs()
{
    QtMaterial::QtMaterialExtendedFab widget;
    widget.setText("Create");
    QVERIFY(true);
}
void TestQtMaterialExtendedFab::keepsMinimumHeight()
{
    QtMaterial::QtMaterialExtendedFab widget;
    QVERIFY(widget.minimumHeight() >= 56);
}
QTEST_MAIN(TestQtMaterialExtendedFab)
#include "tst_extendedfab.moc"
