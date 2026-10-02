#include <QGraphicsOpacityEffect>
#include <QSignalSpy>
#include <QTest>
#include <QWidget>

#include "qtmaterial/widgets/surfaces/qtmaterialsnackbarhost.h"

class tst_QtMaterialSnackbarHost : public QObject
{
    Q_OBJECT

private slots:
    void construction();
    void showSingleMessage();
    void transitionOpacityCoversWholeSnackbar();
    void queueMessages();
    void replaceCurrent();
    void dismissCurrent();
};

void tst_QtMaterialSnackbarHost::construction()
{
    QWidget host;
    QtMaterial::QtMaterialSnackbarHost snackbarHost(&host);

    QCOMPARE(snackbarHost.hostWidget(), &host);
    QVERIFY(snackbarHost.snackbar() != nullptr);
    QVERIFY(!snackbarHost.isShowing());
    QCOMPARE(snackbarHost.pendingCount(), 0);
}

void tst_QtMaterialSnackbarHost::showSingleMessage()
{
    QWidget host;
    host.resize(800, 600);
    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    QtMaterial::QtMaterialSnackbarHost snackbarHost(&host);

    QtMaterial::SnackbarRequest req;
    req.text = QStringLiteral("Saved");
    req.duration = QtMaterial::SnackbarDuration::Indefinite;

    snackbarHost.showMessage(req);

    QTRY_VERIFY(snackbarHost.isShowing());
    QCOMPARE(snackbarHost.pendingCount(), 0);
    QCOMPARE(snackbarHost.snackbar()->text(), QStringLiteral("Saved"));
    QVERIFY(snackbarHost.snackbar()->isVisible());
}

void tst_QtMaterialSnackbarHost::transitionOpacityCoversWholeSnackbar()
{
    QWidget host;
    host.resize(800, 600);
    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    QtMaterial::QtMaterialSnackbarHost snackbarHost(&host);
    auto* snackbar = snackbarHost.snackbar();
    QVERIFY(snackbar != nullptr);

    auto* opacityEffect =
        qobject_cast<QGraphicsOpacityEffect*>(snackbar->graphicsEffect());
    QVERIFY(opacityEffect != nullptr);
    QVERIFY(qFuzzyIsNull(opacityEffect->opacity()));

    QSignalSpy shownSpy(snackbar, &QtMaterial::QtMaterialSnackbar::shown);
    QSignalSpy dismissedSpy(snackbar, &QtMaterial::QtMaterialSnackbar::dismissed);

    QtMaterial::SnackbarRequest req;
    req.text = QStringLiteral("Transition opacity");
    req.actionText = QStringLiteral("Action");
    req.showDismissButton = true;
    req.duration = QtMaterial::SnackbarDuration::Indefinite;

    snackbarHost.showMessage(req);

    QTRY_COMPARE(shownSpy.count(), 1);
    QVERIFY(qFuzzyCompare(opacityEffect->opacity(), qreal(1.0)));

    snackbarHost.dismissCurrent();

    QTRY_COMPARE(dismissedSpy.count(), 1);
    QVERIFY(qFuzzyIsNull(opacityEffect->opacity()));
}

void tst_QtMaterialSnackbarHost::queueMessages()
{
    QWidget host;
    host.resize(800, 600);
    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    QtMaterial::QtMaterialSnackbarHost snackbarHost(&host);

    QtMaterial::SnackbarRequest first;
    first.text = QStringLiteral("First");
    first.duration = QtMaterial::SnackbarDuration::Indefinite;

    QtMaterial::SnackbarRequest second;
    second.text = QStringLiteral("Second");
    second.duration = QtMaterial::SnackbarDuration::Indefinite;

    snackbarHost.showMessage(first);
    snackbarHost.showMessage(second);

    QTRY_VERIFY(snackbarHost.isShowing());
    QCOMPARE(snackbarHost.snackbar()->text(), QStringLiteral("First"));
    QCOMPARE(snackbarHost.pendingCount(), 1);

    snackbarHost.dismissCurrent();

    QTRY_VERIFY(snackbarHost.isShowing());
    QTRY_COMPARE(snackbarHost.pendingCount(), 0);
    QCOMPARE(snackbarHost.snackbar()->text(), QStringLiteral("Second"));
}

void tst_QtMaterialSnackbarHost::replaceCurrent()
{
    QWidget host;
    host.resize(800, 600);
    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    QtMaterial::QtMaterialSnackbarHost snackbarHost(&host);

    QtMaterial::SnackbarRequest first;
    first.text = QStringLiteral("First");
    first.duration = QtMaterial::SnackbarDuration::Indefinite;

    QtMaterial::SnackbarRequest replacement;
    replacement.text = QStringLiteral("Replacement");
    replacement.duration = QtMaterial::SnackbarDuration::Indefinite;

    QSignalSpy dismissedSpy(snackbarHost.snackbar(), &QtMaterial::QtMaterialSnackbar::dismissed);

    snackbarHost.showMessage(first);
    QTRY_VERIFY(snackbarHost.isShowing());

    snackbarHost.showMessage(replacement, true);

    QTRY_VERIFY(dismissedSpy.count() >= 1);

    const auto args = dismissedSpy.takeFirst();
    QCOMPARE(args.at(0).value<QtMaterial::SnackbarDismissReason>(),
             QtMaterial::SnackbarDismissReason::Consecutive);

    QTRY_VERIFY(snackbarHost.isShowing());
    QCOMPARE(snackbarHost.pendingCount(), 0);
    QCOMPARE(snackbarHost.snackbar()->text(), QStringLiteral("Replacement"));
}

void tst_QtMaterialSnackbarHost::dismissCurrent()
{
    QWidget host;
    host.resize(800, 600);
    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    QtMaterial::QtMaterialSnackbarHost snackbarHost(&host);

    QtMaterial::SnackbarRequest req;
    req.text = QStringLiteral("Dismiss me");
    req.duration = QtMaterial::SnackbarDuration::Indefinite;

    QSignalSpy dismissedSpy(snackbarHost.snackbar(), &QtMaterial::QtMaterialSnackbar::dismissed);

    snackbarHost.showMessage(req);
    QTRY_VERIFY(snackbarHost.isShowing());

    snackbarHost.dismissCurrent();

    QTRY_VERIFY(dismissedSpy.count() >= 1);
    QTRY_VERIFY(!snackbarHost.isShowing());

    const auto args = dismissedSpy.takeFirst();
    QCOMPARE(args.at(0).value<QtMaterial::SnackbarDismissReason>(),
             QtMaterial::SnackbarDismissReason::Manual);
}

QTEST_MAIN(tst_QtMaterialSnackbarHost)
#include "tst_snackbarhost.moc"