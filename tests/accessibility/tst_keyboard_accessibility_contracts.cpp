#include <QtTest/QtTest>

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
    QtMaterial::QtMaterialFilledButton button;
    button.setText(QStringLiteral("Save"));
    button.show();
    QVERIFY(QTest::qWaitForWindowExposed(&button));

    QSignalSpy clickedSpy(&button, &QAbstractButton::clicked);

    button.setFocus();
    QVERIFY(button.hasFocus());

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