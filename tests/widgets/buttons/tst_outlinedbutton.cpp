#include <QtTest/QtTest>
#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"
class TestQtMaterialOutlinedButton : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
};
void TestQtMaterialOutlinedButton::constructs()
{
    QtMaterial::QtMaterialOutlinedButton widget;
    QVERIFY(true);
}
QTEST_MAIN(TestQtMaterialOutlinedButton)
#include "tst_outlinedbutton.moc"
