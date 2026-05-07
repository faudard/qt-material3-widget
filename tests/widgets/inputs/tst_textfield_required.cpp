#include <QtTest/QtTest>

#include <QLineEdit>

#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

using namespace QtMaterial;

class tst_TextFieldRequired : public QObject
{
    Q_OBJECT

private slots:
    void requiredDefaultsDisabled();
    void requiredEmptyTextFailsValidatorOnEdit();
    void requiredNonEmptyClearsAutomaticError();
    void requiredModeCanBeDisabledAgain();
    void requiredErrorTextIsUsedForAccessibility();
    void requiredDoesNotCreateAutomaticErrorInManualMode();
    void filledTextFieldInheritsRequiredContract();
};

void tst_TextFieldRequired::requiredDefaultsDisabled()
{
    QtMaterialOutlinedTextField field;

    QVERIFY(!field.isRequired());
    QCOMPARE(field.requiredText(), QStringLiteral("Required"));
    QVERIFY(!field.hasAutomaticValidationError());
    QVERIFY(!field.hasErrorState());
}

void tst_TextFieldRequired::requiredEmptyTextFailsValidatorOnEdit()
{
    QtMaterialOutlinedTextField field;
    field.setValidationFeedbackMode(QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnEdit);
    field.setRequired(true);

    QVERIFY(field.isRequired());
    QVERIFY(field.hasAutomaticValidationError());
    QVERIFY(field.hasErrorState());
}

void tst_TextFieldRequired::requiredNonEmptyClearsAutomaticError()
{
    QtMaterialOutlinedTextField field;
    field.setRequired(true);
    field.setValidationFeedbackMode(QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnEdit);

    QVERIFY(field.hasAutomaticValidationError());

    field.setText(QStringLiteral("ok"));

    QVERIFY(!field.hasAutomaticValidationError());
    QVERIFY(!field.hasErrorState());
}

void tst_TextFieldRequired::requiredModeCanBeDisabledAgain()
{
    QtMaterialOutlinedTextField field;
    field.setValidationFeedbackMode(QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnEdit);
    field.setRequired(true);

    QVERIFY(field.hasAutomaticValidationError());

    field.setRequired(false);

    QVERIFY(!field.isRequired());
    QVERIFY(!field.hasAutomaticValidationError());
    QVERIFY(!field.hasErrorState());
}

void tst_TextFieldRequired::requiredErrorTextIsUsedForAccessibility()
{
    QtMaterialOutlinedTextField field;
    field.setLabelText(QStringLiteral("Email"));
    field.setSupportingText(QStringLiteral("Use your work email."));
    field.setErrorText(QStringLiteral("Invalid email."));
    field.setRequiredText(QStringLiteral("Email is required."));
    field.setValidationFeedbackMode(QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnEdit);
    field.setRequired(true);

    QVERIFY(field.hasAutomaticValidationError());
    QCOMPARE(field.requiredText(), QStringLiteral("Email is required."));
    QVERIFY(field.accessibleDescription().contains(QStringLiteral("Email is required.")));
    QVERIFY(!field.accessibleDescription().contains(QStringLiteral("Invalid email.")));
}

void tst_TextFieldRequired::requiredDoesNotCreateAutomaticErrorInManualMode()
{
    QtMaterialOutlinedTextField field;
    field.setValidationFeedbackMode(QtMaterialOutlinedTextField::ValidationFeedbackMode::ManualOnly);
    field.setRequired(true);

    QVERIFY(field.isRequired());
    QVERIFY(!field.hasAutomaticValidationError());
    QVERIFY(!field.hasErrorState());

    field.setHasErrorState(true);
    QVERIFY(field.hasErrorState());
    QVERIFY(!field.hasAutomaticValidationError());
}

void tst_TextFieldRequired::filledTextFieldInheritsRequiredContract()
{
    QtMaterialFilledTextField field;
    field.setValidationFeedbackMode(QtMaterialFilledTextField::ValidationFeedbackMode::ValidatorOnEdit);
    field.setRequired(true);

    QVERIFY(field.isRequired());
    QVERIFY(field.hasAutomaticValidationError());

    field.setText(QStringLiteral("value"));

    QVERIFY(!field.hasAutomaticValidationError());
    QVERIFY(!field.hasErrorState());
}

QTEST_MAIN(tst_TextFieldRequired)
#include "tst_textfield_required.moc"
