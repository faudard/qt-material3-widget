#include <QtTest/QtTest>
#include "qtmaterial/widgets/selection/qtmaterialswitch.h"
class tst_Switch : public QObject
{
    Q_OBJECT
private slots:
    void basicConstruction();
    void keepsCheckableBehavior();
};
void tst_Switch::basicConstruction()
{
    QtMaterial::QtMaterialSwitch widget;
    QVERIFY(true);
}
void tst_Switch::keepsCheckableBehavior()
{
    QtMaterial::QtMaterialSwitch widget;
    QVERIFY(widget.isCheckable());
    widget.setChecked(true);
    QVERIFY(widget.isChecked());
}
QTEST_MAIN(tst_Switch)
#include "tst_switch.moc"
