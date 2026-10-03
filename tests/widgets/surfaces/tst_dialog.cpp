#include <QAbstractButton>
#include <QApplication>
#include <QLineEdit>
#include <QPushButton>
#include <QSignalSpy>
#include <QTest>
#include <QVBoxLayout>
#include <QWidget>
#include <QPixmap>

#include "qtmaterial/widgets/surfaces/qtmaterialdialog.h"

class tst_Dialog : public QObject
{
    Q_OBJECT

private slots:
    void basicConstruction();
    void titleAndSupportingTextUpdateAccessibility();
    void openFocusesInitialWidgetAndRestoresPreviousFocusOnClose();
    void escapeRejectsDialogWhenEnabled();
    void escapeCanBeDisabled();
    void tabFocusStaysInsideDialog();
    void enterActivatesDefaultButton();
    void rtlAndDesktopScaleRendering_data();
    void rtlAndDesktopScaleRendering();
};

void tst_Dialog::basicConstruction()
{
    QWidget host;
    host.resize(640, 480);

    QtMaterial::QtMaterialDialog dialog(&host);

    QVERIFY(dialog.parentWidget() == &host);
    QCOMPARE(dialog.bodyWidget(), nullptr);
    QVERIFY(dialog.dismissOnEscape());
    QVERIFY(dialog.restoreFocusOnClose());
    QCOMPARE(dialog.accessibilitySummary(), QStringLiteral("Dialog"));
}

void tst_Dialog::titleAndSupportingTextUpdateAccessibility()
{
    QtMaterial::QtMaterialDialog dialog;

    dialog.setTitleText(QStringLiteral("Delete item"));
    dialog.setSupportingText(QStringLiteral("This action cannot be undone."));

    QCOMPARE(dialog.titleText(), QStringLiteral("Delete item"));
    QCOMPARE(dialog.supportingText(), QStringLiteral("This action cannot be undone."));
    QCOMPARE(dialog.accessibleName(), QStringLiteral("Delete item"));
    QCOMPARE(dialog.accessibleDescription(), QStringLiteral("This action cannot be undone."));
    QCOMPARE(dialog.accessibilitySummary(),
             QStringLiteral("Delete item. This action cannot be undone."));
}

void tst_Dialog::openFocusesInitialWidgetAndRestoresPreviousFocusOnClose()
{
    QWidget host;
    host.resize(640, 480);

    auto* outside = new QLineEdit(&host);
    outside->setObjectName(QStringLiteral("outside"));
    outside->setFocusPolicy(Qt::StrongFocus);

    QtMaterial::QtMaterialDialog dialog(&host);
    auto* body = new QWidget;
    auto* layout = new QVBoxLayout(body);
    auto* first = new QLineEdit(body);
    auto* second = new QLineEdit(body);

    first->setObjectName(QStringLiteral("first"));
    second->setObjectName(QStringLiteral("second"));

    layout->addWidget(first);
    layout->addWidget(second);

    dialog.setBodyWidget(body);
    dialog.setInitialFocusWidget(second);

    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    host.activateWindow();
    outside->setFocus(Qt::OtherFocusReason);
    QTRY_COMPARE(QApplication::focusWidget(), outside);

    dialog.open();
    QVERIFY(dialog.isVisible());
    QTRY_COMPARE(QApplication::focusWidget(), second);

    dialog.close();
    QVERIFY(!dialog.isVisible());
    QTRY_COMPARE(QApplication::focusWidget(), outside);
}

void tst_Dialog::escapeRejectsDialogWhenEnabled()
{
    QWidget host;
    host.resize(640, 480);

    QtMaterial::QtMaterialDialog dialog(&host);
    auto* editor = new QLineEdit;
    editor->setAccessibleName(QStringLiteral("Dialog editor"));
    dialog.setBodyWidget(editor);

    QSignalSpy rejectedSpy(&dialog, &QtMaterial::QtMaterialDialog::rejected);
    QSignalSpy closedSpy(&dialog, &QtMaterial::QtMaterialDialog::closed);

    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    dialog.open();
    QVERIFY(dialog.isVisible());

    QTest::keyClick(&dialog, Qt::Key_Escape);

    QVERIFY(!dialog.isVisible());
    QCOMPARE(rejectedSpy.count(), 1);
    QCOMPARE(closedSpy.count(), 1);
}

void tst_Dialog::escapeCanBeDisabled()
{
    QWidget host;
    host.resize(640, 480);

    QtMaterial::QtMaterialDialog dialog(&host);
    dialog.setDismissOnEscape(false);

    QSignalSpy rejectedSpy(&dialog, &QtMaterial::QtMaterialDialog::rejected);

    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    dialog.open();
    QVERIFY(dialog.isVisible());

    QTest::keyClick(&dialog, Qt::Key_Escape);

    QVERIFY(dialog.isVisible());
    QCOMPARE(rejectedSpy.count(), 0);
}

void tst_Dialog::tabFocusStaysInsideDialog()
{
    QWidget host;
    host.resize(640, 480);

    QtMaterial::QtMaterialDialog dialog(&host);
    auto* body = new QWidget;
    auto* layout = new QVBoxLayout(body);
    auto* first = new QLineEdit(body);
    auto* second = new QLineEdit(body);

    first->setObjectName(QStringLiteral("first"));
    second->setObjectName(QStringLiteral("second"));

    layout->addWidget(first);
    layout->addWidget(second);

    dialog.setBodyWidget(body);
    dialog.setInitialFocusWidget(first);

    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    dialog.open();
    QCOMPARE(QApplication::focusWidget(), first);

    QTest::keyClick(QApplication::focusWidget(), Qt::Key_Tab);
    QCOMPARE(QApplication::focusWidget(), second);

    QTest::keyClick(QApplication::focusWidget(), Qt::Key_Tab);
    QCOMPARE(QApplication::focusWidget(), first);

    QTest::keyClick(QApplication::focusWidget(), Qt::Key_Backtab);
    QCOMPARE(QApplication::focusWidget(), second);
}

void tst_Dialog::enterActivatesDefaultButton()
{
    QWidget host;
    host.resize(640, 480);

    QtMaterial::QtMaterialDialog dialog(&host);
    auto* body = new QWidget;
    auto* layout = new QVBoxLayout(body);
    auto* editor = new QLineEdit(body);
    auto* ok = new QPushButton(QStringLiteral("OK"), body);

    layout->addWidget(editor);
    layout->addWidget(ok);

    dialog.setBodyWidget(body);
    dialog.setInitialFocusWidget(editor);
    dialog.setDefaultButton(ok);

    QSignalSpy clickedSpy(ok, &QPushButton::clicked);

    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    dialog.open();
    QCOMPARE(QApplication::focusWidget(), editor);

    QTest::keyClick(&dialog, Qt::Key_Return);

    QCOMPARE(clickedSpy.count(), 1);
}

void tst_Dialog::rtlAndDesktopScaleRendering_data()
{
    QTest::addColumn<qreal>("dpr");
    QTest::newRow("100-percent") << qreal(1.00);
    QTest::newRow("125-percent") << qreal(1.25);
    QTest::newRow("150-percent") << qreal(1.50);
    QTest::newRow("175-percent") << qreal(1.75);
    QTest::newRow("200-percent") << qreal(2.00);
}

void tst_Dialog::rtlAndDesktopScaleRendering()
{
    QFETCH(qreal, dpr);
    QWidget host;
    host.resize(640, 480);
    QtMaterial::QtMaterialDialog dialog(&host);
    dialog.setTitleText(QStringLiteral("Settings"));
    dialog.setSupportingText(QStringLiteral("Review changes"));
    dialog.setLayoutDirection(Qt::RightToLeft);
    dialog.resize(420, 260);
    QCOMPARE(dialog.layoutDirection(), Qt::RightToLeft);

    QPixmap pixmap(qMax(1, qRound(dialog.width() * dpr)),
                   qMax(1, qRound(dialog.height() * dpr)));
    pixmap.setDevicePixelRatio(dpr);
    pixmap.fill(Qt::transparent);
    dialog.render(&pixmap);
    QVERIFY(!pixmap.isNull());
    QCOMPARE(pixmap.devicePixelRatio(), dpr);
}

QTEST_MAIN(tst_Dialog)
#include "tst_dialog.moc"
