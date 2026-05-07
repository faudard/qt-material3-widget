#include <QtTest/QtTest>

#include <QLineEdit>
#include <QSignalSpy>
#include <QToolButton>

#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

using namespace QtMaterial;

class tst_TextFieldReadOnly : public QObject
{
    Q_OBJECT

private slots:
    void outlinedReadOnlyDelegatesToLineEdit();
    void readOnlyPreventsUserTextMutation();
    void readOnlyHidesClearAction();
    void passwordToggleRemainsAvailableWhenReadOnly();
    void filledTextFieldInheritsReadOnlyContract();
};

void tst_TextFieldReadOnly::outlinedReadOnlyDelegatesToLineEdit()
{
    QtMaterialOutlinedTextField field;

    QVERIFY(!field.isReadOnly());
    QVERIFY(field.lineEdit());
    QVERIFY(!field.lineEdit()->isReadOnly());

    field.setReadOnly(true);
    QVERIFY(field.isReadOnly());
    QVERIFY(field.lineEdit()->isReadOnly());
    QVERIFY(field.isEnabled());

    field.setReadOnly(false);
    QVERIFY(!field.isReadOnly());
    QVERIFY(!field.lineEdit()->isReadOnly());
}

void tst_TextFieldReadOnly::readOnlyPreventsUserTextMutation()
{
    QtMaterialOutlinedTextField field;
    field.setText(QStringLiteral("locked"));
    field.setReadOnly(true);

    QSignalSpy changedSpy(field.lineEdit(), &QLineEdit::textChanged);

    field.lineEdit()->setCursorPosition(field.text().size());
    field.lineEdit()->insert(QStringLiteral("!"));

    QCOMPARE(field.text(), QStringLiteral("locked"));
    QCOMPARE(changedSpy.count(), 0);
}

void tst_TextFieldReadOnly::readOnlyHidesClearAction()
{
    QtMaterialOutlinedTextField field;
    field.resize(260, 80);
    field.setText(QStringLiteral("locked"));
    field.setClearButtonEnabled(true);
    field.setEndActionMode(QtMaterialOutlinedTextField::EndActionMode::ClearText);

    field.show();
    QVERIFY(QTest::qWaitForWindowExposed(&field));

    auto* button = field.findChild<QToolButton*>(QStringLiteral("endActionButton"));
    QVERIFY(button);

    QVERIFY(button->isVisible());

    field.setReadOnly(true);
    QCoreApplication::processEvents();

    QVERIFY(!button->isVisible());
    QCOMPARE(field.text(), QStringLiteral("locked"));
}

void tst_TextFieldReadOnly::passwordToggleRemainsAvailableWhenReadOnly()
{
    QtMaterialOutlinedTextField field;
    field.resize(260, 80);
    field.setText(QStringLiteral("secret"));
    field.setEchoMode(QLineEdit::Password);
    field.setEndActionMode(QtMaterialOutlinedTextField::EndActionMode::TogglePasswordVisibility);
    field.setReadOnly(true);

    field.show();
    QVERIFY(QTest::qWaitForWindowExposed(&field));

    auto* button = field.findChild<QToolButton*>(QStringLiteral("endActionButton"));
    QVERIFY(button);
    QVERIFY(button->isVisible());
    QVERIFY(button->isEnabled());

    QSignalSpy visibilitySpy(&field, &QtMaterialOutlinedTextField::passwordVisibilityChanged);
    QTest::mouseClick(button, Qt::LeftButton);

    QCOMPARE(visibilitySpy.count(), 1);
    QCOMPARE(field.lineEdit()->echoMode(), QLineEdit::Normal);
    QVERIFY(field.isReadOnly());
}

void tst_TextFieldReadOnly::filledTextFieldInheritsReadOnlyContract()
{
    QtMaterialFilledTextField field;

    QVERIFY(!field.isReadOnly());
    field.setReadOnly(true);

    QVERIFY(field.isReadOnly());
    QVERIFY(field.lineEdit());
    QVERIFY(field.lineEdit()->isReadOnly());
}

QTEST_MAIN(tst_TextFieldReadOnly)
#include "tst_textfield_readonly.moc"
