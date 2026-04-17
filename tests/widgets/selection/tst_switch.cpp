#include <QtTest/QtTest>
#include <QWidget>
#include "qtmaterial/widgets/selection/qtmaterialswitch.h"
class tst_Switch : public QObject
{
    Q_OBJECT
private slots:
    void basicConstruction();
};
void tst_Switch::basicConstruction()
{
    QtMaterial::QtMaterialSwitch widget; QVERIFY(widget.isCheckable());
}
QTEST_MAIN(tst_Switch)
#include "tst_switch.moc"
