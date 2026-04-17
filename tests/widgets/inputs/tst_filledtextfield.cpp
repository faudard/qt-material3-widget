#include <QtTest/QtTest>
#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"
class TestQtMaterialFilledTextField : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
};
void TestQtMaterialFilledTextField::constructs()
{
    QtMaterial::QtMaterialFilledTextField widget;
    QVERIFY(true);
}
QTEST_MAIN(TestQtMaterialFilledTextField)
#include "tst_filledtextfield.moc"
