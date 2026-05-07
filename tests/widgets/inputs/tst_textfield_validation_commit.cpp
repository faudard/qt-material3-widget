#include <QLineEdit>
#include <QObject>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSignalSpy>
#include <QTest>

#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

class tst_TextFieldValidationCommit : public QObject
{
    Q_OBJECT

private slots:
    void validateInputCommitsValidatorOnCommit();
    void validateInputAcceptsValidValue();
    void resetValidationFeedbackClearsAutomaticError();
    void resetValidationFeedbackKeepsManualError();
    void requiredErrorIsShownAfterValidateInput();
};

void tst_TextFieldValidationCommit::validateInputCommitsValidatorOnCommit()
{
    QtMaterial::QtMaterialOutlinedTextField field;
    QRegularExpressionValidator validator(QRegularExpression(QStringLiteral("[0-9]{3}")), &field);

    field.setValidator(&validator);
    field.setValidationFeedbackMode(
        QtMaterial::QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnCommit);
    field.setErrorDisplayMode(
        QtMaterial::QtMaterialOutlinedTextField::ErrorDisplayMode::AfterCommit);
    field.setText(QStringLiteral("12"));

    QVERIFY(!field.hasAutomaticValidationError());
    QVERIFY(!field.isEffectiveErrorVisible());

    QSignalSpy requestedSpy(
        &field,
        &QtMaterial::QtMaterialOutlinedTextField::validationRequested);

    QVERIFY(!field.validateInput());

    QVERIFY(field.hasAutomaticValidationError());
    QVERIFY(field.hasErrorState());
    QVERIFY(field.isEffectiveErrorVisible());
    QCOMPARE(requestedSpy.count(), 1);
    QCOMPARE(requestedSpy.takeFirst().at(0).toBool(), false);
}

void tst_TextFieldValidationCommit::validateInputAcceptsValidValue()
{
    QtMaterial::QtMaterialOutlinedTextField field;
    QRegularExpressionValidator validator(QRegularExpression(QStringLiteral("[0-9]{3}")), &field);

    field.setValidator(&validator);
    field.setValidationFeedbackMode(
        QtMaterial::QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnCommit);
    field.setErrorDisplayMode(
        QtMaterial::QtMaterialOutlinedTextField::ErrorDisplayMode::AfterCommit);
    field.setText(QStringLiteral("123"));

    QSignalSpy requestedSpy(
        &field,
        &QtMaterial::QtMaterialOutlinedTextField::validationRequested);

    QVERIFY(field.validateInput());

    QVERIFY(!field.hasAutomaticValidationError());
    QVERIFY(!field.hasErrorState());
    QVERIFY(!field.isEffectiveErrorVisible());
    QCOMPARE(requestedSpy.count(), 1);
    QCOMPARE(requestedSpy.takeFirst().at(0).toBool(), true);
}

void tst_TextFieldValidationCommit::resetValidationFeedbackClearsAutomaticError()
{
    QtMaterial::QtMaterialOutlinedTextField field;
    QRegularExpressionValidator validator(QRegularExpression(QStringLiteral("[0-9]{3}")), &field);

    field.setValidator(&validator);
    field.setValidationFeedbackMode(
        QtMaterial::QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnCommit);
    field.setErrorDisplayMode(
        QtMaterial::QtMaterialOutlinedTextField::ErrorDisplayMode::AfterCommit);
    field.setText(QStringLiteral("12"));

    QVERIFY(!field.validateInput());
    QVERIFY(field.hasAutomaticValidationError());
    QVERIFY(field.isEffectiveErrorVisible());

    QSignalSpy resetSpy(
        &field,
        &QtMaterial::QtMaterialOutlinedTextField::validationFeedbackReset);

    field.resetValidationFeedback();

    QVERIFY(!field.hasAutomaticValidationError());
    QVERIFY(!field.hasErrorState());
    QVERIFY(!field.isEffectiveErrorVisible());
    QCOMPARE(resetSpy.count(), 1);
}

void tst_TextFieldValidationCommit::resetValidationFeedbackKeepsManualError()
{
    QtMaterial::QtMaterialOutlinedTextField field;

    field.setErrorText(QStringLiteral("Server rejected this value"));
    field.setHasErrorState(true);
    field.setErrorDisplayMode(
        QtMaterial::QtMaterialOutlinedTextField::ErrorDisplayMode::AfterCommit);

    QVERIFY(field.hasErrorState());
    QVERIFY(field.isEffectiveErrorVisible());

    field.resetValidationFeedback();

    QVERIFY(field.hasErrorState());
    QVERIFY(field.isEffectiveErrorVisible());
    QCOMPARE(field.errorText(), QStringLiteral("Server rejected this value"));
}

void tst_TextFieldValidationCommit::requiredErrorIsShownAfterValidateInput()
{
    QtMaterial::QtMaterialOutlinedTextField field;

    field.setRequired(true);
    field.setValidationFeedbackMode(
        QtMaterial::QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnCommit);
    field.setErrorDisplayMode(
        QtMaterial::QtMaterialOutlinedTextField::ErrorDisplayMode::AfterCommit);

    QVERIFY(!field.hasAutomaticValidationError());
    QVERIFY(!field.isEffectiveErrorVisible());

    QVERIFY(!field.validateInput());

    QVERIFY(field.hasAutomaticValidationError());
    QVERIFY(field.hasErrorState());
    QVERIFY(field.isEffectiveErrorVisible());
    QCOMPARE(field.effectiveErrorText(), QStringLiteral("Required"));
}

QTEST_MAIN(tst_TextFieldValidationCommit)
#include "tst_textfield_validation_commit.moc"
