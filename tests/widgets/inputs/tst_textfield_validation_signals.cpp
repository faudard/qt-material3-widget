#include <QtTest/QtTest>

#include <QIntValidator>
#include <QSignalSpy>

#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

using namespace QtMaterial;

class tst_TextFieldValidationSignals : public QObject
{
    Q_OBJECT

private slots:
    void acceptableInputSignalTracksRequiredText();
    void manualErrorEmitsOnlyEffectiveErrorState();
    void validatorOnEditEmitsAutomaticValidationTransitions();
    void filledTextFieldInheritsValidationSignalContract();
};

void tst_TextFieldValidationSignals::acceptableInputSignalTracksRequiredText()
{
    QtMaterialOutlinedTextField field;
    field.setRequired(true);

    QSignalSpy acceptableSpy(&field, &QtMaterialOutlinedTextField::acceptableInputChanged);
    QVERIFY(acceptableSpy.isValid());

    field.setText(QStringLiteral("ok"));

    QCOMPARE(acceptableSpy.count(), 1);
    QCOMPARE(acceptableSpy.takeFirst().at(0).toBool(), true);
    QVERIFY(field.isAcceptableInput());

    field.setText(QString());

    QCOMPARE(acceptableSpy.count(), 1);
    QCOMPARE(acceptableSpy.takeFirst().at(0).toBool(), false);
    QVERIFY(!field.isAcceptableInput());
}

void tst_TextFieldValidationSignals::manualErrorEmitsOnlyEffectiveErrorState()
{
    QtMaterialOutlinedTextField field;

    QSignalSpy automaticSpy(&field, &QtMaterialOutlinedTextField::automaticValidationErrorChanged);
    QSignalSpy effectiveSpy(&field, &QtMaterialOutlinedTextField::effectiveErrorStateChanged);
    QSignalSpy acceptableSpy(&field, &QtMaterialOutlinedTextField::acceptableInputChanged);
    QVERIFY(automaticSpy.isValid());
    QVERIFY(effectiveSpy.isValid());
    QVERIFY(acceptableSpy.isValid());

    field.setHasErrorState(true);

    QCOMPARE(automaticSpy.count(), 0);
    QCOMPARE(acceptableSpy.count(), 0);
    QCOMPARE(effectiveSpy.count(), 1);
    QCOMPARE(effectiveSpy.takeFirst().at(0).toBool(), true);

    field.setHasErrorState(false);

    QCOMPARE(automaticSpy.count(), 0);
    QCOMPARE(acceptableSpy.count(), 0);
    QCOMPARE(effectiveSpy.count(), 1);
    QCOMPARE(effectiveSpy.takeFirst().at(0).toBool(), false);
}

void tst_TextFieldValidationSignals::validatorOnEditEmitsAutomaticValidationTransitions()
{
    QtMaterialOutlinedTextField field;
    auto* validator = new QIntValidator(10, 99, &field);

    field.setValidator(validator);
    field.setValidationFeedbackMode(QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnEdit);

    QSignalSpy automaticSpy(&field, &QtMaterialOutlinedTextField::automaticValidationErrorChanged);
    QSignalSpy effectiveSpy(&field, &QtMaterialOutlinedTextField::effectiveErrorStateChanged);
    QSignalSpy acceptableSpy(&field, &QtMaterialOutlinedTextField::acceptableInputChanged);
    QVERIFY(automaticSpy.isValid());
    QVERIFY(effectiveSpy.isValid());
    QVERIFY(acceptableSpy.isValid());

    field.setText(QStringLiteral("7"));

    QCOMPARE(automaticSpy.count(), 1);
    QCOMPARE(automaticSpy.takeFirst().at(0).toBool(), true);
    QCOMPARE(effectiveSpy.count(), 1);
    QCOMPARE(effectiveSpy.takeFirst().at(0).toBool(), true);
    QCOMPARE(acceptableSpy.count(), 1);
    QCOMPARE(acceptableSpy.takeFirst().at(0).toBool(), false);
    QVERIFY(field.hasAutomaticValidationError());
    QVERIFY(!field.isAcceptableInput());

    field.setText(QStringLiteral("42"));

    QCOMPARE(automaticSpy.count(), 1);
    QCOMPARE(automaticSpy.takeFirst().at(0).toBool(), false);
    QCOMPARE(effectiveSpy.count(), 1);
    QCOMPARE(effectiveSpy.takeFirst().at(0).toBool(), false);
    QCOMPARE(acceptableSpy.count(), 1);
    QCOMPARE(acceptableSpy.takeFirst().at(0).toBool(), true);
    QVERIFY(!field.hasAutomaticValidationError());
    QVERIFY(field.isAcceptableInput());
}

void tst_TextFieldValidationSignals::filledTextFieldInheritsValidationSignalContract()
{
    QtMaterialFilledTextField field;
    field.setRequired(true);

    QSignalSpy acceptableSpy(&field, &QtMaterialOutlinedTextField::acceptableInputChanged);
    QVERIFY(acceptableSpy.isValid());

    field.setText(QStringLiteral("value"));

    QCOMPARE(acceptableSpy.count(), 1);
    QCOMPARE(acceptableSpy.takeFirst().at(0).toBool(), true);
    QVERIFY(field.isAcceptableInput());
}

QTEST_MAIN(tst_TextFieldValidationSignals)
#include "tst_textfield_validation_signals.moc"
