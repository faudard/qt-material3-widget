#include <QtTest/QtTest>
#include "qtmaterial/widgets/buttons/qtmaterialelevatedbutton.h"
class TestQtMaterialElevatedButton : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
    void staysEnabledByDefault();
};
void TestQtMaterialElevatedButton::constructs()
{
    QtMaterial::QtMaterialElevatedButton widget;
    QVERIFY(true);
}
void TestQtMaterialElevatedButton::staysEnabledByDefault()
{
    QtMaterial::QtMaterialElevatedButton widget;
    QVERIFY(widget.isEnabled());
}
QTEST_MAIN(TestQtMaterialElevatedButton)
#include "tst_elevatedbutton.moc"
