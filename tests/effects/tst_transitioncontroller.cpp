#include <QtTest/QtTest>
#include <QSignalSpy>
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
class tst_TransitionController : public QObject
{
    Q_OBJECT
private slots:
    void emitsProgressChanged();
};
void tst_TransitionController::emitsProgressChanged()
{
    QtMaterial::QtMaterialTransitionController controller;
    QSignalSpy spy(&controller, &QtMaterial::QtMaterialTransitionController::progressChanged);
    controller.startForward();
    QVERIFY(spy.count() >= 1);
}
QTEST_MAIN(tst_TransitionController)
#include "tst_transitioncontroller.moc"
