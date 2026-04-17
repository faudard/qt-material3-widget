#include <QtTest/QtTest>
#include "qtmaterial/widgets/inputs/qtmaterialautocompletepopup.h"
class TestQtMaterialAutocompletePopup : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
};
void TestQtMaterialAutocompletePopup::constructs()
{
    QtMaterial::QtMaterialAutocompletePopup widget;
    QVERIFY(true);
}
QTEST_MAIN(TestQtMaterialAutocompletePopup)
#include "tst_autocompletepopup.moc"
