#include <QtTest>

#include <QApplication>
#include <QEvent>
#include <QLayout>
#include <QWidget>

#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterial/widgets/surfaces/qtmaterialsnackbar.h"

using namespace QtMaterial;

class tst_SnackbarLifecycle : public QObject
{
    Q_OBJECT

private slots:
    void resolvedMetricsReachWidget();
    void pausePreservesRemainingTime();
    void changingDurationResetsRemainingTime();
    void indefiniteDurationHasNoTimer();
};

namespace {

Theme snackbarTestTheme(
    int shortDurationMs,
    int longDurationMs)
{
    Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));

    MotionStyle immediate;
    immediate.durationMs = 1;
    immediate.easing =
        QEasingCurve(QEasingCurve::Linear);
    theme.motion().setStyle(
        MotionToken::Medium2,
        immediate);
    theme.motion().setStyle(
        MotionToken::Short4,
        immediate);

    ComponentTokenOverride tokens;
    tokens.custom.insert(
        QStringLiteral("shortDurationMs"),
        shortDurationMs);
    tokens.custom.insert(
        QStringLiteral("longDurationMs"),
        longDurationMs);
    tokens.custom.insert(
        QStringLiteral("minWidth"),
        264);
    tokens.custom.insert(
        QStringLiteral("labelMinWidth"),
        136);
    tokens.custom.insert(
        QStringLiteral("slideDistance"),
        27);
    tokens.custom.insert(
        QStringLiteral("actionPaddingHorizontal"),
        17);
    theme.componentOverrides().setOverride(
        QStringLiteral("snackbar"),
        tokens);

    return theme;
}

void sendInteractionEvent(
    QWidget* widget,
    QEvent::Type type)
{
    QEvent event(type);
    QApplication::sendEvent(widget, &event);
}

} // namespace

void tst_SnackbarLifecycle::resolvedMetricsReachWidget()
{
    ThemeContext context(snackbarTestTheme(600, 1000));

    QtMaterialSnackbar snackbar;
    snackbar.setThemeContext(&context);

    const SnackbarSpec& spec =
        snackbar.resolvedSpec();

    QCOMPARE(spec.minWidth, 264);
    QCOMPARE(spec.labelMinWidth, 136);
    QCOMPARE(spec.slideDistance, 27);
    QCOMPARE(spec.actionPaddingHorizontal, 17);
    QCOMPARE(
        snackbar.minimumSizeHint().width(),
        264);
    QVERIFY(snackbar.layout());
    QCOMPARE(
        snackbar.layout()->contentsMargins(),
        spec.contentPadding);
}

void tst_SnackbarLifecycle::pausePreservesRemainingTime()
{
    QWidget host;
    host.resize(800, 600);
    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    ThemeContext context(snackbarTestTheme(650, 1100));
    QtMaterialSnackbar snackbar(&host);
    snackbar.setThemeContext(&context);
    snackbar.setText(QStringLiteral("Saved"));
    snackbar.setDuration(SnackbarDuration::Short);

    QSignalSpy shownSpy(
        &snackbar,
        &QtMaterialSnackbar::shown);
    QSignalSpy pauseSpy(
        &snackbar,
        &QtMaterialSnackbar::autoHidePausedChanged);

    snackbar.showSnackbar();
    QTRY_COMPARE(shownSpy.count(), 1);

    QTest::qWait(120);
    const int beforePause =
        snackbar.remainingAutoHideTimeMs();
    QVERIFY(beforePause > 0);
    QVERIFY(beforePause < 650);

    sendInteractionEvent(&snackbar, QEvent::Enter);
    QVERIFY(snackbar.isAutoHidePaused());
    QCOMPARE(pauseSpy.count(), 1);

    const int pausedRemaining =
        snackbar.remainingAutoHideTimeMs();
    QVERIFY(pausedRemaining > 0);

    QTest::qWait(180);
    QVERIFY(snackbar.isVisible());
    QVERIFY(
        qAbs(
            snackbar.remainingAutoHideTimeMs()
            - pausedRemaining) <= 25);

    sendInteractionEvent(&snackbar, QEvent::Leave);
    QVERIFY(!snackbar.isAutoHidePaused());
    QCOMPARE(pauseSpy.count(), 2);

    QTest::qWait(90);
    const int resumedRemaining =
        snackbar.remainingAutoHideTimeMs();
    QVERIFY(resumedRemaining >= 0);
    QVERIFY(resumedRemaining < pausedRemaining);

    snackbar.dismiss(SnackbarDismissReason::Manual);
}

void tst_SnackbarLifecycle::
changingDurationResetsRemainingTime()
{
    QWidget host;
    host.resize(800, 600);
    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    ThemeContext context(snackbarTestTheme(500, 1200));
    QtMaterialSnackbar snackbar(&host);
    snackbar.setThemeContext(&context);
    snackbar.setText(QStringLiteral("Updated"));
    snackbar.setDuration(SnackbarDuration::Short);

    QSignalSpy shownSpy(
        &snackbar,
        &QtMaterialSnackbar::shown);
    snackbar.showSnackbar();
    QTRY_COMPARE(shownSpy.count(), 1);

    QTest::qWait(120);
    const int shortRemaining =
        snackbar.remainingAutoHideTimeMs();
    QVERIFY(shortRemaining > 0);
    QVERIFY(shortRemaining < 500);

    snackbar.setDuration(SnackbarDuration::Long);
    const int longRemaining =
        snackbar.remainingAutoHideTimeMs();

    QVERIFY(longRemaining > shortRemaining);
    QVERIFY(longRemaining <= 1200);

    snackbar.dismiss(SnackbarDismissReason::Manual);
}

void tst_SnackbarLifecycle::
indefiniteDurationHasNoTimer()
{
    QWidget host;
    host.resize(800, 600);
    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    ThemeContext context(snackbarTestTheme(400, 800));
    QtMaterialSnackbar snackbar(&host);
    snackbar.setThemeContext(&context);
    snackbar.setText(QStringLiteral("Waiting"));
    snackbar.setDuration(SnackbarDuration::Indefinite);

    QSignalSpy shownSpy(
        &snackbar,
        &QtMaterialSnackbar::shown);
    snackbar.showSnackbar();
    QTRY_COMPARE(shownSpy.count(), 1);

    QCOMPARE(snackbar.remainingAutoHideTimeMs(), 0);
    sendInteractionEvent(&snackbar, QEvent::Enter);
    QVERIFY(!snackbar.isAutoHidePaused());

    snackbar.dismiss(SnackbarDismissReason::Manual);
}

QTEST_MAIN(tst_SnackbarLifecycle)
#include "tst_snackbarlifecycle.moc"
