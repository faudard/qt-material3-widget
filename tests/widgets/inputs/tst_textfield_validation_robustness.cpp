#include <QLabel>
#include <QLineEdit>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

using namespace QtMaterial;

namespace {

void showAndProcess(QWidget& widget)
{
    widget.resize(
        qMax(360, widget.sizeHint().width()),
        widget.sizeHint().height());
    widget.show();
    QCoreApplication::processEvents();
}

QLabel* counterLabel(
    QtMaterialOutlinedTextField& field)
{
    return field.findChild<QLabel*>(
        QStringLiteral(
            "qtmaterial_textfield_characterCounter"));
}

} // namespace

class tst_TextFieldValidationRobustness : public QObject
{
    Q_OBJECT

private slots:
    void counterLabelHasStableLifetime();
    void counterCanBeToggledRepeatedly();
    void counterUpdatesWhenMaximumChanges();
    void counterReservesSupportingGeometry();
    void filledCounterUsesSameLifecycle();
    void destructionAfterCounterEnableIsSafe();

    void requiredSignalsAreEmittedOnce();
    void validatorSignalsAreEmittedOnce();
    void commitValidationDoesNotDuplicateSignals();
    void validateInputRefreshesOnlyOnce();
    void manualErrorDoesNotChangeAcceptability();
};

void tst_TextFieldValidationRobustness::
counterLabelHasStableLifetime()
{
    QtMaterialOutlinedTextField field;

    QLabel* label = counterLabel(field);
    QVERIFY(label);
    QVERIFY(label->isHidden());

    field.setCharacterCounterEnabled(true);
    QCOMPARE(counterLabel(field), label);

    field.setCharacterCounterEnabled(false);
    QCOMPARE(counterLabel(field), label);
    QVERIFY(label->isHidden());
}

void tst_TextFieldValidationRobustness::
counterCanBeToggledRepeatedly()
{
    QtMaterialOutlinedTextField field;
    field.setMaxLength(12);
    field.setText(QStringLiteral("abc"));
    showAndProcess(field);

    QLabel* label = counterLabel(field);
    QVERIFY(label);

    for (int index = 0; index < 64; ++index) {
        const bool enabled = (index % 2) == 0;
        field.setCharacterCounterEnabled(enabled);
        field.resize(
            360 + (index % 5),
            field.sizeHint().height());
        QCoreApplication::processEvents();

        QCOMPARE(counterLabel(field), label);
        QCOMPARE(label->isHidden(), !enabled);
    }
}

void tst_TextFieldValidationRobustness::
counterUpdatesWhenMaximumChanges()
{
    QtMaterialOutlinedTextField field;
    field.setCharacterCounterEnabled(true);
    field.setText(QStringLiteral("abcd"));
    showAndProcess(field);

    QLabel* label = counterLabel(field);
    QVERIFY(label);

    field.setMaxLength(10);
    QCoreApplication::processEvents();
    QCOMPARE(label->text(), QStringLiteral("4/10"));

    field.setMaxLength(6);
    QCoreApplication::processEvents();
    QCOMPARE(label->text(), QStringLiteral("4/6"));
}

void tst_TextFieldValidationRobustness::
counterReservesSupportingGeometry()
{
    QtMaterialOutlinedTextField field;
    field.setSupportingText(QString());
    field.setCharacterCounterEnabled(true);
    field.setMaxLength(20);
    field.setText(QStringLiteral("abc"));
    showAndProcess(field);

    QLabel* label = counterLabel(field);
    QVERIFY(label);
    QVERIFY(!label->isHidden());
    QVERIFY(label->geometry().isValid());
    QVERIFY(label->geometry().height() > 0);
}

void tst_TextFieldValidationRobustness::
filledCounterUsesSameLifecycle()
{
    QtMaterialFilledTextField field;
    field.setCharacterCounterEnabled(true);
    field.setMaxLength(8);
    field.setText(QStringLiteral("abc"));
    showAndProcess(field);

    QLabel* label = counterLabel(field);
    QVERIFY(label);
    QCOMPARE(label->text(), QStringLiteral("3/8"));
    QVERIFY(!label->isHidden());
}

void tst_TextFieldValidationRobustness::
destructionAfterCounterEnableIsSafe()
{
    for (int index = 0; index < 32; ++index) {
        auto* field =
            new QtMaterialOutlinedTextField;
        field->setCharacterCounterEnabled(true);
        field->setText(QStringLiteral("abc"));
        delete field;
    }

    QCoreApplication::processEvents();
    QVERIFY(true);
}

void tst_TextFieldValidationRobustness::
requiredSignalsAreEmittedOnce()
{
    QtMaterialOutlinedTextField field;
    field.setValidationFeedbackMode(
        QtMaterialOutlinedTextField::
            ValidationFeedbackMode::ValidatorOnEdit);

    QSignalSpy automaticSpy(
        &field,
        &QtMaterialOutlinedTextField::
            automaticValidationErrorChanged);
    QSignalSpy effectiveSpy(
        &field,
        &QtMaterialOutlinedTextField::
            effectiveErrorStateChanged);
    QSignalSpy acceptableSpy(
        &field,
        &QtMaterialOutlinedTextField::
            acceptableInputChanged);

    field.setRequired(true);

    QCOMPARE(automaticSpy.count(), 1);
    QCOMPARE(effectiveSpy.count(), 1);
    QCOMPARE(acceptableSpy.count(), 1);
    QVERIFY(field.hasAutomaticValidationError());
    QVERIFY(!field.isAcceptableInput());

    field.setRequired(true);

    QCOMPARE(automaticSpy.count(), 1);
    QCOMPARE(effectiveSpy.count(), 1);
    QCOMPARE(acceptableSpy.count(), 1);
}

void tst_TextFieldValidationRobustness::
validatorSignalsAreEmittedOnce()
{
    QtMaterialOutlinedTextField field;
    QRegularExpressionValidator validator(
        QRegularExpression(
            QStringLiteral("^[0-9]{3}$")),
        &field);

    field.setValidator(&validator);
    field.setValidationFeedbackMode(
        QtMaterialOutlinedTextField::
            ValidationFeedbackMode::ValidatorOnEdit);

    QSignalSpy automaticSpy(
        &field,
        &QtMaterialOutlinedTextField::
            automaticValidationErrorChanged);
    QSignalSpy effectiveSpy(
        &field,
        &QtMaterialOutlinedTextField::
            effectiveErrorStateChanged);
    QSignalSpy acceptableSpy(
        &field,
        &QtMaterialOutlinedTextField::
            acceptableInputChanged);

    field.setText(QStringLiteral("abc"));

    QCOMPARE(automaticSpy.count(), 1);
    QCOMPARE(effectiveSpy.count(), 1);
    QCOMPARE(acceptableSpy.count(), 1);

    field.setText(QStringLiteral("123"));

    QCOMPARE(automaticSpy.count(), 2);
    QCOMPARE(effectiveSpy.count(), 2);
    QCOMPARE(acceptableSpy.count(), 2);
}

void tst_TextFieldValidationRobustness::
commitValidationDoesNotDuplicateSignals()
{
    QtMaterialOutlinedTextField field;
    field.setRequired(true);
    field.setValidationFeedbackMode(
        QtMaterialOutlinedTextField::
            ValidationFeedbackMode::ValidatorOnCommit);

    QSignalSpy automaticSpy(
        &field,
        &QtMaterialOutlinedTextField::
            automaticValidationErrorChanged);
    QSignalSpy effectiveSpy(
        &field,
        &QtMaterialOutlinedTextField::
            effectiveErrorStateChanged);

    QVERIFY(field.lineEdit());
    QVERIFY(QMetaObject::invokeMethod(
        field.lineEdit(),
        "editingFinished",
        Qt::DirectConnection));

    QCOMPARE(automaticSpy.count(), 1);
    QCOMPARE(effectiveSpy.count(), 1);

    QVERIFY(QMetaObject::invokeMethod(
        field.lineEdit(),
        "editingFinished",
        Qt::DirectConnection));

    QCOMPARE(automaticSpy.count(), 1);
    QCOMPARE(effectiveSpy.count(), 1);
}

void tst_TextFieldValidationRobustness::
validateInputRefreshesOnlyOnce()
{
    QtMaterialOutlinedTextField field;
    field.setRequired(true);
    field.setValidationFeedbackMode(
        QtMaterialOutlinedTextField::
            ValidationFeedbackMode::ValidatorOnCommit);

    QSignalSpy automaticSpy(
        &field,
        &QtMaterialOutlinedTextField::
            automaticValidationErrorChanged);
    QSignalSpy visibilitySpy(
        &field,
        &QtMaterialOutlinedTextField::
            effectiveErrorVisibleChanged);
    QSignalSpy requestedSpy(
        &field,
        &QtMaterialOutlinedTextField::
            validationRequested);

    QVERIFY(!field.validateInput());

    QCOMPARE(automaticSpy.count(), 1);
    QCOMPARE(visibilitySpy.count(), 1);
    QCOMPARE(requestedSpy.count(), 1);
}

void tst_TextFieldValidationRobustness::
manualErrorDoesNotChangeAcceptability()
{
    QtMaterialOutlinedTextField field;
    field.setText(QStringLiteral("valid"));

    QSignalSpy acceptableSpy(
        &field,
        &QtMaterialOutlinedTextField::
            acceptableInputChanged);

    field.setHasErrorState(true);

    QVERIFY(field.hasErrorState());
    QVERIFY(field.isAcceptableInput());
    QCOMPARE(acceptableSpy.count(), 0);

    field.setHasErrorState(false);

    QVERIFY(!field.hasErrorState());
    QVERIFY(field.isAcceptableInput());
    QCOMPARE(acceptableSpy.count(), 0);
}

QTEST_MAIN(tst_TextFieldValidationRobustness)

#include "tst_textfield_validation_robustness.moc"
