#include <QtTest/QtTest>
#include <QWidget>
#include "qtmaterial/widgets/selection/qtmaterialcheckbox.h"
class tst_Checkbox : public QObject
{
    Q_OBJECT
private slots:
    void basicConstruction();
};
void tst_Checkbox::basicConstruction()
{
    QtMaterial::QtMaterialCheckbox widget; widget.setText(QStringLiteral("Checkbox")); QVERIFY(widget.isCheckable());
}
QTEST_MAIN(tst_Checkbox)
#include "tst_checkbox.moc"
