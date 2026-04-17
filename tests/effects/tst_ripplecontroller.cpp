#include <QtTest/QtTest>
#include <QWidget>
#include "qtmaterial/effects/qtmaterialripplecontroller.h"
class tst_RippleController : public QObject
{
    Q_OBJECT
private slots:
    void activatesAfterAdd();
};
void tst_RippleController::activatesAfterAdd()
{
    QWidget widget;
    QtMaterial::QtMaterialRippleController controller(&widget);
    QVERIFY(!controller.isActive());
    controller.addRipple(QPointF(10, 10));
    QVERIFY(controller.isActive());
}
QTEST_MAIN(tst_RippleController)
#include "tst_ripplecontroller.moc"
