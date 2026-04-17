#include <QtTest/QtTest>
#include "qtmaterial/widgets/selection/qtmaterialradiobutton.h"
class TestQtMaterialRadioButton : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
};
void TestQtMaterialRadioButton::constructs()
{
    QtMaterial::QtMaterialRadioButton widget;
    QVERIFY(true);
}
QTEST_MAIN(TestQtMaterialRadioButton)
#include "tst_radiobutton.moc"
