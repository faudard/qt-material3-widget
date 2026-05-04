#include <QtTest/QtTest>

#include <QIntValidator>
#include <QLineEdit>
#include <QSignalSpy>
#include <QToolButton>

#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

using QtMaterial::QtMaterialFilledTextField;
using QtMaterial::QtMaterialOutlinedTextField;

namespace {

QToolButton* endActionButton(QtMaterialOutlinedTextField& field)
{
    field.resize(320, 88);
    field.show();
    QTest::qWaitForWindowExposed(&field);
    field.repaint();
    QCoreApplication::processEvents();
    return field.findChild<QToolButton*>(QStringLiteral("endActionButton"));
}

} // namespace

class tst_TextFieldAccessibility : public QObject
{
    Q_OBJECT

private slots:
    void accessibleDescriptionIncludesSupportingAndErrorText();
    void clearActionHasAccessibleNameAndKeyboardFocus();
    void passwordToggleUpdatesAccessibleName();
    void customTrailingActionUsesProvidedAccessibleText();
    void filledTextFieldInheritsAccessibleActionContract();
};

void tst_TextFieldAccessibility::accessibleDescriptionIncludesSupportingAndErrorText()
{
    QtMaterialOutlinedTextField field;
    field.setLabelText(QStringLiteral("Code"));
    field.setSupportingText(QStringLiteral("Use 4 digits"));
    field.setErrorText(QStringLiteral("Only numbers are accepted"));

    field.setHasErrorState(true);
    field.show();
    QTest::qWaitForWindowExposed(&field);
    QCoreApplication::processEvents();

    QCOMPARE(field.accessibleName(), QStringLiteral("Code"));
    QVERIFY2(field.accessibleDescription().contains(QStringLiteral("Use 4 digits")),
             qPrintable(field.accessibleDescription()));
    QVERIFY2(field.accessibleDescription().contains(QStringLiteral("Only numbers are accepted")),
             qPrintable(field.accessibleDescription()));

    auto* editor = field.lineEdit();
    QVERIFY(editor != nullptr);
    QCOMPARE(editor->accessibleName(), QStringLiteral("Code"));
    QVERIFY2(editor->accessibleDescription().contains(QStringLiteral("Use 4 digits")),
             qPrintable(editor->accessibleDescription()));
    QVERIFY2(editor->accessibleDescription().contains(QStringLiteral("Only numbers are accepted")),
             qPrintable(editor->accessibleDescription()));
}

void tst_TextFieldAccessibility::clearActionHasAccessibleNameAndKeyboardFocus()
{
    QtMaterialOutlinedTextField field;
    field.setLabelText(QStringLiteral("Search"));
    field.setText(QStringLiteral("query"));
    field.setClearButtonEnabled(true);
    field.setEndActionMode(QtMaterialOutlinedTextField::EndActionMode::ClearText);

    auto* button = endActionButton(field);
    QVERIFY(button != nullptr);
    QVERIFY(button->isVisible());
    QCOMPARE(button->focusPolicy(), Qt::StrongFocus);
    QCOMPARE(button->accessibleName(), QStringLiteral("Clear text"));
    QVERIFY2(button->accessibleDescription().contains(QStringLiteral("Clear text")),
             qPrintable(button->accessibleDescription()));

    QSignalSpy clearSpy(&field, &QtMaterialOutlinedTextField::clearTriggered);
    QTest::mouseClick(button, Qt::LeftButton);
    QCOMPARE(clearSpy.count(), 1);
    QCOMPARE(field.text(), QString());
}

void tst_TextFieldAccessibility::passwordToggleUpdatesAccessibleName()
{
    QtMaterialOutlinedTextField field;
    field.setLabelText(QStringLiteral("Password"));
    field.setText(QStringLiteral("secret"));
    field.setEchoMode(QLineEdit::Password);
    field.setEndActionMode(QtMaterialOutlinedTextField::EndActionMode::TogglePasswordVisibility);

    auto* button = endActionButton(field);
    QVERIFY(button != nullptr);
    QVERIFY(button->isVisible());
    QCOMPARE(button->focusPolicy(), Qt::StrongFocus);
    QCOMPARE(button->accessibleName(), QStringLiteral("Show password"));

    QSignalSpy visibilitySpy(&field, &QtMaterialOutlinedTextField::passwordVisibilityChanged);
    QTest::mouseClick(button, Qt::LeftButton);
    QCOMPARE(visibilitySpy.count(), 1);
    QCOMPARE(button->accessibleName(), QStringLiteral("Hide password"));
}

void tst_TextFieldAccessibility::customTrailingActionUsesProvidedAccessibleText()
{
    QtMaterialOutlinedTextField field;
    field.setLabelText(QStringLiteral("Token"));
    field.setEndActionMode(QtMaterialOutlinedTextField::EndActionMode::CustomTrailingAction);
    field.setTrailingActionText(QStringLiteral("Scan"));
    field.setTrailingActionToolTip(QStringLiteral("Open token scanner"));
    field.setTrailingActionVisibleWhenEmpty(true);

    auto* button = endActionButton(field);
    QVERIFY(button != nullptr);
    QVERIFY(button->isVisible());
    QCOMPARE(button->focusPolicy(), Qt::StrongFocus);
    QCOMPARE(button->text(), QStringLiteral("Scan"));
    QCOMPARE(button->accessibleName(), QStringLiteral("Scan"));
    QCOMPARE(button->accessibleDescription(), QStringLiteral("Open token scanner"));

    QSignalSpy actionSpy(&field, &QtMaterialOutlinedTextField::trailingActionTriggered);
    QTest::mouseClick(button, Qt::LeftButton);
    QCOMPARE(actionSpy.count(), 1);
}

void tst_TextFieldAccessibility::filledTextFieldInheritsAccessibleActionContract()
{
    QtMaterialFilledTextField field;
    field.setLabelText(QStringLiteral("Search"));
    field.setText(QStringLiteral("material"));
    field.setClearButtonEnabled(true);
    field.setEndActionMode(QtMaterialOutlinedTextField::EndActionMode::ClearText);

    auto* button = endActionButton(field);
    QVERIFY(button != nullptr);
    QVERIFY(button->isVisible());
    QCOMPARE(button->focusPolicy(), Qt::StrongFocus);
    QCOMPARE(button->accessibleName(), QStringLiteral("Clear text"));
}

QTEST_MAIN(tst_TextFieldAccessibility)
#include "tst_textfield_accessibility.moc"
