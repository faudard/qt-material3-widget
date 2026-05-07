#include <QtTest/QtTest>
#include <QLineEdit>

#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

using namespace QtMaterial;

class tst_TextFieldErrorVisibilityAccessibility : public QObject
{
    Q_OBJECT

private slots:
    void hiddenAutomaticErrorIsNotPaintedAsEffectiveError();
};

void tst_TextFieldErrorVisibilityAccessibility::hiddenAutomaticErrorIsNotPaintedAsEffectiveError()
{
    QtMaterialOutlinedTextField field;
    field.setLabelText(QStringLiteral("Email"));
    field.setSupportingText(QStringLiteral("Use your work email"));
    field.setRequired(true);
    field.setRequiredText(QStringLiteral("Email is required"));
    field.setValidationFeedbackMode(QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnEdit);
    field.setErrorDisplayMode(QtMaterialOutlinedTextField::ErrorDisplayMode::WhenTouched);

    QVERIFY(field.hasErrorState());
    QVERIFY(!field.isEffectiveErrorVisible());

    field.setTouched(true);
    QVERIFY(field.isEffectiveErrorVisible());
    QCOMPARE(field.effectiveErrorText(), QStringLiteral("Email is required"));
}

QTEST_MAIN(tst_TextFieldErrorVisibilityAccessibility)
#include "tst_textfield_error_visibility_accessibility.moc"
