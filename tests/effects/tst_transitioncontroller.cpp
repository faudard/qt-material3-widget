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
    controller.setDuration(0);

    QSignalSpy spy(&controller, &QtMaterial::QtMaterialTransitionController::progressChanged);
    QVERIFY(spy.isValid());

    controller.startForward();

    QTRY_VERIFY_WITH_TIMEOUT(spy.count() >= 1, 1000);
    QCOMPARE(controller.progress(), 1.0);
}
QTEST_MAIN(tst_TransitionController)
#include "tst_transitioncontroller.moc"
