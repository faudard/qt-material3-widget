#include <QtTest/QtTest>
#include <QLineEdit>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

using namespace QtMaterial;

class tst_TextFieldValidationContracts : public QObject
{
    Q_OBJECT

private slots:
    void requiredNonBlankRejectsWhitespace();
    void requiredNonEmptyAllowsWhitespace();
    void requiredTextOverridesDefaultMessage();
    void validatorErrorTextIsUsedForValidatorFailures();
    void inputMaskErrorTextIsUsedForIncompleteMasks();
    void optionalEmptyInputMaskDoesNotError();
    void requiredTakesPriorityOverInputMask();
    void filledTextFieldUsesSameValidationContract();
};

void tst_TextFieldValidationContracts::requiredNonBlankRejectsWhitespace()
{
    QtMaterialOutlinedTextField field;
    field.setRequired(true);
    field.setRequiredValidationMode(QtMaterialOutlinedTextField::RequiredValidationMode::NonBlank);
    field.setRequiredText(QStringLiteral("Required"));
    field.setValidationFeedbackMode(QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnEdit);

    field.setText(QStringLiteral("   "));

    QVERIFY(field.hasAutomaticValidationError());
    QVERIFY(field.hasErrorState());
    QCOMPARE(field.effectiveErrorText(), QStringLiteral("Required"));

    field.setText(QStringLiteral("abc"));
    QVERIFY(!field.hasAutomaticValidationError());
    QVERIFY(!field.hasErrorState());
}

void tst_TextFieldValidationContracts::requiredNonEmptyAllowsWhitespace()
{
    QtMaterialOutlinedTextField field;
    field.setRequired(true);
    field.setRequiredValidationMode(QtMaterialOutlinedTextField::RequiredValidationMode::NonEmpty);
    field.setValidationFeedbackMode(QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnEdit);

    field.setText(QStringLiteral("   "));

    QVERIFY(!field.hasAutomaticValidationError());
    QVERIFY(!field.hasErrorState());
}

void tst_TextFieldValidationContracts::requiredTextOverridesDefaultMessage()
{
    QtMaterialOutlinedTextField field;
    field.setRequired(true);
    field.setRequiredText(QStringLiteral("Email is required"));
    field.setValidationFeedbackMode(QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnEdit);

    QCOMPARE(field.effectiveErrorText(), QStringLiteral("Email is required"));
}

void tst_TextFieldValidationContracts::validatorErrorTextIsUsedForValidatorFailures()
{
    QtMaterialOutlinedTextField field;
    QRegularExpressionValidator validator(QRegularExpression(QStringLiteral("\\d{3}")), &field);

    field.setValidator(&validator);
    field.setValidatorErrorText(QStringLiteral("Use three digits"));
    field.setValidationFeedbackMode(QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnEdit);

    field.setText(QStringLiteral("12"));
    QVERIFY(field.hasAutomaticValidationError());
    QCOMPARE(field.effectiveErrorText(), QStringLiteral("Use three digits"));

    field.setText(QStringLiteral("123"));
    QVERIFY(!field.hasAutomaticValidationError());
}

void tst_TextFieldValidationContracts::inputMaskErrorTextIsUsedForIncompleteMasks()
{
    QtMaterialOutlinedTextField field;
    field.setInputMask(QStringLiteral("0000;_"));
    field.setInputMaskErrorText(QStringLiteral("Enter four digits"));
    field.setValidationFeedbackMode(QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnEdit);

    field.setText(QStringLiteral("12"));
    QVERIFY(field.hasAutomaticValidationError());
    QCOMPARE(field.effectiveErrorText(), QStringLiteral("Enter four digits"));

    field.setText(QStringLiteral("1234"));
    QVERIFY(!field.hasAutomaticValidationError());
}

void tst_TextFieldValidationContracts::optionalEmptyInputMaskDoesNotError()
{
    QtMaterialOutlinedTextField field;
    field.setInputMask(QStringLiteral("0000;_"));
    field.setInputMaskErrorText(QStringLiteral("Enter four digits"));
    field.setValidationFeedbackMode(QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnEdit);

    field.setText(QString());

    QVERIFY(!field.hasAutomaticValidationError());
    QVERIFY(!field.hasErrorState());
}

void tst_TextFieldValidationContracts::requiredTakesPriorityOverInputMask()
{
    QtMaterialOutlinedTextField field;
    field.setRequired(true);
    field.setRequiredText(QStringLiteral("Required"));
    field.setInputMask(QStringLiteral("0000;_"));
    field.setInputMaskErrorText(QStringLiteral("Enter four digits"));
    field.setValidationFeedbackMode(QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnEdit);

    QCOMPARE(field.effectiveErrorText(), QStringLiteral("Required"));
}

void tst_TextFieldValidationContracts::filledTextFieldUsesSameValidationContract()
{
    QtMaterialFilledTextField field;
    field.setRequired(true);
    field.setRequiredText(QStringLiteral("Required"));
    field.setValidationFeedbackMode(QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnEdit);

    QVERIFY(field.hasAutomaticValidationError());
    QCOMPARE(field.effectiveErrorText(), QStringLiteral("Required"));

    field.setText(QStringLiteral("value"));
    QVERIFY(!field.hasAutomaticValidationError());
    QVERIFY(!field.hasErrorState());
}

QTEST_MAIN(tst_TextFieldValidationContracts)
#include "tst_textfield_validation_contracts.moc"
