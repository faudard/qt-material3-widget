#include <QSignalSpy>
#include <QTest>
#include <QWidget>
#include <QFocusEvent>
#include <QPixmap>
#include <QPushButton>

#include "qtmaterial/widgets/surfaces/qtmaterialsnackbar.h"

class tst_QtMaterialSnackbar : public QObject
{
    Q_OBJECT

private slots:
    void construction();
    void requestApi();
    void showAndDismiss();
    void actionButtonTriggersSignalAndDismiss();
    void escapeDismisses();
    void accessibilityTracksRequest();
    void actionFocusPausesAutoHide();
    void rtlAndHighDpiRenderSmoke();
};

void tst_QtMaterialSnackbar::construction()
{
    QtMaterial::QtMaterialSnackbar snackbar;
    QCOMPARE(snackbar.progress(), 0.0);
    QCOMPARE(snackbar.duration(), QtMaterial::SnackbarDuration::Short);
    QVERIFY(snackbar.text().isEmpty());
    QVERIFY(snackbar.actionText().isEmpty());
    QVERIFY(!snackbar.showDismissButton());
}

void tst_QtMaterialSnackbar::requestApi()
{
    QtMaterial::QtMaterialSnackbar snackbar;

    QtMaterial::SnackbarRequest req;
    req.text = QStringLiteral("Draft saved");
    req.actionText = QStringLiteral("Undo");
    req.duration = QtMaterial::SnackbarDuration::Long;
    req.showDismissButton = true;

    snackbar.setRequest(req);

    QCOMPARE(snackbar.request().text, QStringLiteral("Draft saved"));
    QCOMPARE(snackbar.request().actionText, QStringLiteral("Undo"));
    QCOMPARE(snackbar.request().duration, QtMaterial::SnackbarDuration::Long);
    QVERIFY(snackbar.request().showDismissButton);

    QCOMPARE(snackbar.text(), QStringLiteral("Draft saved"));
    QCOMPARE(snackbar.actionText(), QStringLiteral("Undo"));
    QCOMPARE(snackbar.duration(), QtMaterial::SnackbarDuration::Long);
    QVERIFY(snackbar.showDismissButton());
}

void tst_QtMaterialSnackbar::showAndDismiss()
{
    QWidget host;
    host.resize(800, 600);
    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    QtMaterial::QtMaterialSnackbar snackbar(&host);

    QtMaterial::SnackbarRequest req;
    req.text = QStringLiteral("Saved");
    req.duration = QtMaterial::SnackbarDuration::Indefinite;
    snackbar.setRequest(req);

    QSignalSpy shownSpy(&snackbar, &QtMaterial::QtMaterialSnackbar::shown);
    QSignalSpy dismissedSpy(&snackbar, &QtMaterial::QtMaterialSnackbar::dismissed);
    QSignalSpy progressSpy(&snackbar, &QtMaterial::QtMaterialSnackbar::progressChanged);

    snackbar.showSnackbar();

    QTRY_VERIFY(shownSpy.count() >= 1);
    QVERIFY(snackbar.isVisible());
    QVERIFY(progressSpy.count() >= 1);
    QCOMPARE(snackbar.progress(), 1.0);

    snackbar.dismiss(QtMaterial::SnackbarDismissReason::Manual);

    QTRY_VERIFY(dismissedSpy.count() >= 1);
    QCOMPARE(snackbar.progress(), 0.0);
    QVERIFY(!snackbar.isVisible());

    const auto args = dismissedSpy.takeFirst();
    QCOMPARE(args.at(0).value<QtMaterial::SnackbarDismissReason>(),
             QtMaterial::SnackbarDismissReason::Manual);
}

void tst_QtMaterialSnackbar::actionButtonTriggersSignalAndDismiss()
{
    QWidget host;
    host.resize(800, 600);
    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    QtMaterial::QtMaterialSnackbar snackbar(&host);

    QtMaterial::SnackbarRequest req;
    req.text = QStringLiteral("Draft saved");
    req.actionText = QStringLiteral("Undo");
    req.duration = QtMaterial::SnackbarDuration::Indefinite;
    snackbar.setRequest(req);
    snackbar.showSnackbar();

    QTRY_VERIFY(snackbar.isVisible());

    QSignalSpy actionSpy(&snackbar, &QtMaterial::QtMaterialSnackbar::actionTriggered);
    QSignalSpy dismissedSpy(&snackbar, &QtMaterial::QtMaterialSnackbar::dismissed);

    auto buttons = snackbar.findChildren<QPushButton*>();
    QPushButton* actionButton = nullptr;
    for (QPushButton* b : buttons) {
        if (b && b->text() == QStringLiteral("Undo")) {
            actionButton = b;
            break;
        }
    }

    QVERIFY(actionButton != nullptr);
    QVERIFY(actionButton->isVisible());

    QTest::mouseClick(actionButton, Qt::LeftButton);

    QTRY_VERIFY(actionSpy.count() >= 1);
    QTRY_VERIFY(dismissedSpy.count() >= 1);

    const auto args = dismissedSpy.takeFirst();
    QCOMPARE(args.at(0).value<QtMaterial::SnackbarDismissReason>(),
             QtMaterial::SnackbarDismissReason::Action);
}

void tst_QtMaterialSnackbar::escapeDismisses()
{
    QWidget host;
    host.resize(800, 600);
    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    QtMaterial::QtMaterialSnackbar snackbar(&host);

    QtMaterial::SnackbarRequest req;
    req.text = QStringLiteral("Dismiss me");
    req.duration = QtMaterial::SnackbarDuration::Indefinite;
    snackbar.setRequest(req);
    snackbar.showSnackbar();

    QTRY_VERIFY(snackbar.isVisible());

    QSignalSpy dismissedSpy(&snackbar, &QtMaterial::QtMaterialSnackbar::dismissed);

    QTest::keyClick(&snackbar, Qt::Key_Escape);

    QTRY_VERIFY(dismissedSpy.count() >= 1);

    const auto args = dismissedSpy.takeFirst();
    QCOMPARE(args.at(0).value<QtMaterial::SnackbarDismissReason>(),
             QtMaterial::SnackbarDismissReason::Manual);
}

void tst_QtMaterialSnackbar::accessibilityTracksRequest()
{
    QtMaterial::QtMaterialSnackbar snackbar;

    QtMaterial::SnackbarRequest req;
    req.text = QStringLiteral("Draft saved");
    req.actionText = QStringLiteral("Undo");
    req.showDismissButton = true;
    snackbar.setRequest(req);

    QCOMPARE(snackbar.accessibleName(), QStringLiteral("Snackbar"));
    QCOMPARE(
        snackbar.accessibleDescription(),
        QStringLiteral("Draft saved. Action: Undo. Dismissible"));

    QPushButton* actionButton = nullptr;
    for (QPushButton* button : snackbar.findChildren<QPushButton*>()) {
        if (button && button->text() == QStringLiteral("Undo")) {
            actionButton = button;
            break;
        }
    }

    QVERIFY(actionButton != nullptr);
    QCOMPARE(actionButton->accessibleName(), QStringLiteral("Undo"));
}

void tst_QtMaterialSnackbar::actionFocusPausesAutoHide()
{
    QWidget host;
    host.resize(800, 600);
    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    QtMaterial::QtMaterialSnackbar snackbar(&host);

    QtMaterial::SnackbarRequest req;
    req.text = QStringLiteral("Saved");
    req.actionText = QStringLiteral("Undo");
    req.duration = QtMaterial::SnackbarDuration::Long;
    snackbar.setRequest(req);
    snackbar.showSnackbar();
    QTRY_VERIFY(snackbar.isVisible());

    QPushButton* actionButton = nullptr;
    for (QPushButton* button : snackbar.findChildren<QPushButton*>()) {
        if (button && button->text() == QStringLiteral("Undo")) {
            actionButton = button;
            break;
        }
    }

    QVERIFY(actionButton != nullptr);

    QFocusEvent focusIn(QEvent::FocusIn, Qt::OtherFocusReason);
    QCoreApplication::sendEvent(actionButton, &focusIn);
    QTRY_VERIFY(snackbar.isAutoHidePaused());

    QFocusEvent focusOut(QEvent::FocusOut, Qt::OtherFocusReason);
    QCoreApplication::sendEvent(actionButton, &focusOut);
    QTRY_VERIFY(!snackbar.isAutoHidePaused());
}

void tst_QtMaterialSnackbar::rtlAndHighDpiRenderSmoke()
{
    QWidget host;
    host.resize(800, 600);

    QtMaterial::QtMaterialSnackbar snackbar(&host);
    snackbar.setLayoutDirection(Qt::RightToLeft);

    QtMaterial::SnackbarRequest req;
    req.text = QStringLiteral("Message");
    req.actionText = QStringLiteral("Action");
    req.duration = QtMaterial::SnackbarDuration::Indefinite;
    req.showDismissButton = true;
    snackbar.setRequest(req);
    snackbar.resize(520, qMax(64, snackbar.sizeHint().height()));

    QPixmap pixmap(snackbar.width() * 2, snackbar.height() * 2);
    pixmap.setDevicePixelRatio(2.0);
    pixmap.fill(Qt::transparent);
    snackbar.render(&pixmap);

    QVERIFY(!pixmap.isNull());
    QCOMPARE(pixmap.devicePixelRatio(), 2.0);
    QCOMPARE(snackbar.layoutDirection(), Qt::RightToLeft);
}

QTEST_MAIN(tst_QtMaterialSnackbar)
#include "tst_snackbar.moc"