#include <QtTest/QtTest>
#include "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h"
class TestQtMaterialFilledTonalButton : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
    void staysEnabledByDefault();
};
void TestQtMaterialFilledTonalButton::constructs()
{
    QtMaterial::QtMaterialFilledTonalButton widget;
    QVERIFY(true);
}
void TestQtMaterialFilledTonalButton::staysEnabledByDefault()
{
    QtMaterial::QtMaterialFilledTonalButton widget;
    QVERIFY(widget.isEnabled());
}
QTEST_MAIN(TestQtMaterialFilledTonalButton)
#include "tst_filledtonalbutton.moc"
