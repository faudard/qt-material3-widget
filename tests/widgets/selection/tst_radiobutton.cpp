#include <QtTest/QtTest>
#include "qtmaterial/widgets/selection/qtmaterialradiobutton.h"
class TestQtMaterialRadioButton : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
    void isCheckable();
};
void TestQtMaterialRadioButton::constructs()
{
    QtMaterial::QtMaterialRadioButton widget;
    QVERIFY(true);
}
void TestQtMaterialRadioButton::isCheckable()
{
    QtMaterial::QtMaterialRadioButton widget;
    QVERIFY(widget.isCheckable());
}
QTEST_MAIN(TestQtMaterialRadioButton)
#include "tst_radiobutton.moc"
