#include <QtTest/QtTest>
#include <QSignalSpy>

#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_TransitionController : public QObject
{
    Q_OBJECT

private slots:
    void emitsProgressChanged();
    void reducedMotionCompletesImmediately();
    void retargetingEndsAtLatestTarget();
    void stopPreservesCurrentProgress();
    void themeMotionTokenAppliesReducedMotion();
};

void tst_TransitionController::emitsProgressChanged()
{
    QtMaterialTransitionController controller;
    controller.setDuration(0);

    QSignalSpy spy(
        &controller,
        &QtMaterialTransitionController::progressChanged);
    QVERIFY(spy.isValid());

    controller.startForward();

    QTRY_VERIFY_WITH_TIMEOUT(spy.count() >= 1, 1000);
    QCOMPARE(controller.progress(), 1.0);
}

void tst_TransitionController::reducedMotionCompletesImmediately()
{
    QtMaterialTransitionController controller;
    controller.setDuration(500);
    controller.setReducedMotion(true);

    QSignalSpy finishedSpy(
        &controller,
        &QtMaterialTransitionController::finished);

    controller.startForward();

    QCOMPARE(controller.progress(), 1.0);
    QVERIFY(!controller.isRunning());
    QCOMPARE(finishedSpy.count(), 1);
}

void tst_TransitionController::retargetingEndsAtLatestTarget()
{
    QtMaterialTransitionController controller;
    controller.setDuration(30);

    controller.startForward();
    QVERIFY(controller.isRunning());
    controller.startBackward();

    QTRY_VERIFY_WITH_TIMEOUT(!controller.isRunning(), 1000);
    QCOMPARE(controller.progress(), 0.0);
}

void tst_TransitionController::stopPreservesCurrentProgress()
{
    QtMaterialTransitionController controller;
    controller.setDuration(200);
    controller.setProgress(0.25);
    controller.startForward();
    controller.stop();

    QVERIFY(!controller.isRunning());
    QVERIFY(controller.progress() >= 0.25);
    QVERIFY(controller.progress() < 1.0);
}

void tst_TransitionController::themeMotionTokenAppliesReducedMotion()
{
    ThemeOptions options;
    Theme theme = ThemeBuilder().build(options);
    theme.accessibility().reducedMotion = true;

    QtMaterialTransitionController controller;
    controller.applyMotionToken(theme, MotionToken::Medium1);

    QVERIFY(controller.reducedMotion());
    controller.startForward();
    QCOMPARE(controller.progress(), 1.0);
}

QTEST_MAIN(tst_TransitionController)
#include "tst_transitioncontroller.moc"
