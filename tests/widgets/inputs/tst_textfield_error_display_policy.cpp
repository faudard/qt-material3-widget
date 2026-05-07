#include <QtTest/QtTest>
#include <QIntValidator>
#include <QSignalSpy>

#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

using QtMaterial::QtMaterialOutlinedTextField;

class tst_textfield_error_display_policy : public QObject
{
    Q_OBJECT

private slots:
    void automaticErrorIsVisibleByDefault();
    void whenTouchedHidesAutomaticErrorBeforeInteraction();
    void whenModifiedHidesAutomaticErrorUntilDirty();
    void manualErrorIsAlwaysVisible();
    void showValidationErrorForcesVisibility();
    void resetValidationErrorVisibilityRestoresPolicy();
    void visibilitySignalIsEmittedOnlyWhenVisibleStateChanges();
};

static void makeInvalidOnEdit(QtMaterialOutlinedTextField& field)
{
    auto* validator = new QIntValidator(100, 999, &field);
    field.setValidator(validator);
    field.setValidationFeedbackMode(QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnEdit);
    field.setText(QStringLiteral("1"));
}

void tst_textfield_error_display_policy::automaticErrorIsVisibleByDefault()
{
    QtMaterialOutlinedTextField field;

    makeInvalidOnEdit(field);

    QVERIFY(field.hasAutomaticValidationError());
    QVERIFY(field.hasErrorState());
    QVERIFY(field.isEffectiveErrorVisible());
}

void tst_textfield_error_display_policy::whenTouchedHidesAutomaticErrorBeforeInteraction()
{
    QtMaterialOutlinedTextField field;
    field.setErrorDisplayMode(QtMaterialOutlinedTextField::ErrorDisplayMode::WhenTouched);

    makeInvalidOnEdit(field);

    QVERIFY(field.hasAutomaticValidationError());
    QVERIFY(field.hasErrorState());
    QVERIFY(!field.isTouched());
    QVERIFY(!field.isEffectiveErrorVisible());

    field.setTouched(true);

    QVERIFY(field.isEffectiveErrorVisible());
}

void tst_textfield_error_display_policy::whenModifiedHidesAutomaticErrorUntilDirty()
{
    QtMaterialOutlinedTextField field;
    field.setErrorDisplayMode(QtMaterialOutlinedTextField::ErrorDisplayMode::WhenModified);

    makeInvalidOnEdit(field);
    field.setModified(false);

    QVERIFY(field.hasAutomaticValidationError());
    QVERIFY(!field.isModified());
    QVERIFY(!field.isEffectiveErrorVisible());

    field.setModified(true);

    QVERIFY(field.isEffectiveErrorVisible());
}

void tst_textfield_error_display_policy::manualErrorIsAlwaysVisible()
{
    QtMaterialOutlinedTextField field;
    field.setErrorDisplayMode(QtMaterialOutlinedTextField::ErrorDisplayMode::WhenTouched);

    field.setHasErrorState(true);

    QVERIFY(field.hasErrorState());
    QVERIFY(field.isEffectiveErrorVisible());
}

void tst_textfield_error_display_policy::showValidationErrorForcesVisibility()
{
    QtMaterialOutlinedTextField field;
    field.setErrorDisplayMode(QtMaterialOutlinedTextField::ErrorDisplayMode::WhenTouched);

    makeInvalidOnEdit(field);

    QVERIFY(!field.isTouched());
    QVERIFY(!field.isEffectiveErrorVisible());

    field.showValidationError();

    QVERIFY(field.isEffectiveErrorVisible());
}

void tst_textfield_error_display_policy::resetValidationErrorVisibilityRestoresPolicy()
{
    QtMaterialOutlinedTextField field;
    field.setErrorDisplayMode(QtMaterialOutlinedTextField::ErrorDisplayMode::WhenTouched);

    makeInvalidOnEdit(field);
    field.showValidationError();
    QVERIFY(field.isEffectiveErrorVisible());

    field.resetTouched();
    field.resetValidationErrorVisibility();

    QVERIFY(field.hasErrorState());
    QVERIFY(!field.isEffectiveErrorVisible());
}

void tst_textfield_error_display_policy::visibilitySignalIsEmittedOnlyWhenVisibleStateChanges()
{
    QtMaterialOutlinedTextField field;
    field.setErrorDisplayMode(QtMaterialOutlinedTextField::ErrorDisplayMode::WhenTouched);
    QSignalSpy spy(&field, &QtMaterialOutlinedTextField::effectiveErrorVisibleChanged);

    makeInvalidOnEdit(field);
    QCOMPARE(spy.count(), 0);

    field.setTouched(true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toBool(), true);

    field.setTouched(true);
    QCOMPARE(spy.count(), 0);
}

QTEST_MAIN(tst_textfield_error_display_policy)
#include "tst_textfield_error_display_policy.moc"
