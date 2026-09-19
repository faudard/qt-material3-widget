#include <QtTest/QtTest>

#include <QVBoxLayout>
#include <QWidget>

#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/selection/qtmaterialcheckbox.h"
#include "qtmaterial/widgets/selection/qtmaterialradiobutton.h"
#include "qtmaterial/widgets/selection/qtmaterialswitch.h"

class tst_KeyboardAccessibilityContracts : public QObject
{
    Q_OBJECT

private slots:
    void buttonActivatesWithSpace();
    void checkboxTogglesWithSpace();
    void radioSelectsWithSpace();
    void switchTogglesWithSpace();
};

void tst_KeyboardAccessibilityContracts::buttonActivatesWithSpace()
{
    QWidget window;
    QVBoxLayout layout(&window);

    QtMaterial::QtMaterialFilledButton button;
    button.setText(QStringLiteral("Save"));
    layout.addWidget(&button);

    window.show();
    QVERIFY(QTest::qWaitForWindowExposed(&window));

    QSignalSpy clickedSpy(&button, &QAbstractButton::clicked);

    // On macOS a standalone top-level button is not guaranteed to become the
    // Cocoa first responder. Exercise the button in a normal window hierarchy
    // and allow the native event loop to deliver focus.
    window.activateWindow();
    button.setFocus(Qt::OtherFocusReason);
    QTRY_VERIFY(button.hasFocus());

    QTest::keyClick(&button, Qt::Key_Space);
    QCOMPARE(clickedSpy.count(), 1);
}

void tst_KeyboardAccessibilityContracts::checkboxTogglesWithSpace()
{
    QtMaterial::QtMaterialCheckbox checkbox;
    checkbox.setText(QStringLiteral("Enable notifications"));
    checkbox.show();
    QVERIFY(QTest::qWaitForWindowExposed(&checkbox));

    checkbox.setFocus();
    QVERIFY(checkbox.hasFocus());

    QVERIFY(!checkbox.isChecked());
    QTest::keyClick(&checkbox, Qt::Key_Space);
    QVERIFY(checkbox.isChecked());
}

void tst_KeyboardAccessibilityContracts::radioSelectsWithSpace()
{
    QtMaterial::QtMaterialRadioButton radio;
    radio.setText(QStringLiteral("Option A"));
    radio.show();
    QVERIFY(QTest::qWaitForWindowExposed(&radio));

    radio.setFocus();
    QVERIFY(radio.hasFocus());

    QVERIFY(!radio.isChecked());
    QTest::keyClick(&radio, Qt::Key_Space);
    QVERIFY(radio.isChecked());
}

void tst_KeyboardAccessibilityContracts::switchTogglesWithSpace()
{
    QtMaterial::QtMaterialSwitch sw;
    sw.setText(QStringLiteral("Use dark mode"));
    sw.show();
    QVERIFY(QTest::qWaitForWindowExposed(&sw));

    sw.setFocus();
    QVERIFY(sw.hasFocus());

    QVERIFY(!sw.isChecked());
    QTest::keyClick(&sw, Qt::Key_Space);
    QVERIFY(sw.isChecked());
}

QTEST_MAIN(tst_KeyboardAccessibilityContracts)
#include "tst_keyboard_accessibility_contracts.moc"