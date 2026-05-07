#include <QtTest/QtTest>

#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"
#include "qtmaterial/widgets/inputs/qtmaterialtextfieldvalidationgroup.h"

#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSignalSpy>

using QtMaterial::QtMaterialOutlinedTextField;
using QtMaterial::QtMaterialTextFieldValidationGroup;

class TextFieldValidationSummaryTest : public QObject
{
    Q_OBJECT

private slots:
    void customLabelsAndMessagesAppearInSummary();
    void summaryKeepsFieldOrder();
    void summaryFallsBackToAccessibleMetadata();
    void validationSummarySignalTracksChanges();
    void removingFieldRemovesItsSummaryItem();
};

static void makeInvalidOnCommit(QtMaterialOutlinedTextField& field, QValidator* validator)
{
    field.setValidator(validator);
    field.setValidationFeedbackMode(QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnCommit);
    field.setText(QStringLiteral("abc"));
}

void TextFieldValidationSummaryTest::customLabelsAndMessagesAppearInSummary()
{
    QtMaterialTextFieldValidationGroup group;
    QtMaterialOutlinedTextField field;
    QRegularExpressionValidator digits(QRegularExpression(QStringLiteral("^\\d+$")));

    makeInvalidOnCommit(field, &digits);
    group.addField(&field);
    group.setFieldLabel(&field, QStringLiteral("Postal code"));
    group.setFieldErrorMessage(&field, QStringLiteral("Use digits only"));

    QVERIFY(!group.validateAll());

    QCOMPARE(group.invalidFields(), QList<QtMaterialOutlinedTextField*>({ &field }));
    QCOMPARE(group.invalidFieldLabels(), QStringList({ QStringLiteral("Postal code") }));
    QCOMPARE(group.validationSummary(), QStringList({ QStringLiteral("Postal code: Use digits only") }));
    QCOMPARE(group.validationSummaryText(QStringLiteral("; ")), QStringLiteral("Postal code: Use digits only"));
}

void TextFieldValidationSummaryTest::summaryKeepsFieldOrder()
{
    QtMaterialTextFieldValidationGroup group;
    QtMaterialOutlinedTextField first;
    QtMaterialOutlinedTextField second;
    QRegularExpressionValidator digits(QRegularExpression(QStringLiteral("^\\d+$")));

    makeInvalidOnCommit(first, &digits);
    makeInvalidOnCommit(second, &digits);

    group.addField(&first);
    group.addField(&second);
    group.setFieldLabel(&first, QStringLiteral("Email"));
    group.setFieldLabel(&second, QStringLiteral("Password"));
    group.setFieldErrorMessage(&first, QStringLiteral("Invalid email"));
    group.setFieldErrorMessage(&second, QStringLiteral("Required"));

    QVERIFY(!group.validateAll());

    QCOMPARE(group.validationSummary(), QStringList({
        QStringLiteral("Email: Invalid email"),
        QStringLiteral("Password: Required"),
    }));
}

void TextFieldValidationSummaryTest::summaryFallsBackToAccessibleMetadata()
{
    QtMaterialTextFieldValidationGroup group;
    QtMaterialOutlinedTextField field;
    QRegularExpressionValidator digits(QRegularExpression(QStringLiteral("^\\d+$")));

    field.setAccessibleName(QStringLiteral("Account number"));
    field.setAccessibleDescription(QStringLiteral("Use numbers only"));
    makeInvalidOnCommit(field, &digits);

    group.addField(&field);

    QVERIFY(!group.validateAll());

    QCOMPARE(group.validationSummary(), QStringList({ QStringLiteral("Account number: Use numbers only") }));
}

void TextFieldValidationSummaryTest::validationSummarySignalTracksChanges()
{
    QtMaterialTextFieldValidationGroup group;
    QtMaterialOutlinedTextField field;
    QRegularExpressionValidator digits(QRegularExpression(QStringLiteral("^\\d+$")));

    makeInvalidOnCommit(field, &digits);
    group.addField(&field);
    group.setFieldLabel(&field, QStringLiteral("Code"));
    group.setFieldErrorMessage(&field, QStringLiteral("Use digits only"));

    QSignalSpy summarySpy(&group, &QtMaterialTextFieldValidationGroup::validationSummaryChanged);

    QVERIFY(!group.validateAll());
    QVERIFY(summarySpy.count() >= 1);
    QCOMPARE(group.validationSummary(), QStringList({ QStringLiteral("Code: Use digits only") }));

    field.setText(QStringLiteral("123"));
    QVERIFY(group.validateAll());
    QCOMPARE(group.validationSummary(), QStringList());
}

void TextFieldValidationSummaryTest::removingFieldRemovesItsSummaryItem()
{
    QtMaterialTextFieldValidationGroup group;
    QtMaterialOutlinedTextField field;
    QRegularExpressionValidator digits(QRegularExpression(QStringLiteral("^\\d+$")));

    makeInvalidOnCommit(field, &digits);
    group.addField(&field);
    group.setFieldLabel(&field, QStringLiteral("Amount"));
    group.setFieldErrorMessage(&field, QStringLiteral("Use digits only"));

    QVERIFY(!group.validateAll());
    QCOMPARE(group.validationSummary().size(), 1);

    group.removeField(&field);

    QVERIFY(group.validationSummary().isEmpty());
    QVERIFY(group.isAcceptable());
    QVERIFY(group.fieldLabel(&field).isEmpty());
    QVERIFY(group.fieldErrorMessage(&field).isEmpty());
}

QTEST_MAIN(TextFieldValidationSummaryTest)

#include "tst_textfield_validation_summary.moc"
