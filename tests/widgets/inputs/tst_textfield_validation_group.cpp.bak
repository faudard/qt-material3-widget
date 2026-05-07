#include <QtTest/QtTest>

#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"
#include "qtmaterial/widgets/inputs/qtmaterialtextfieldvalidationgroup.h"

#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSignalSpy>

using QtMaterial::QtMaterialOutlinedTextField;
using QtMaterial::QtMaterialTextFieldValidationGroup;

class TextFieldValidationGroupTest : public QObject
{
    Q_OBJECT

private slots:
    void emptyGroupIsAcceptable();
    void addAndRemoveFields();
    void validateAllReturnsFalseWhenAnyFieldIsInvalid();
    void resetValidationFeedbackForwardsToFields();
    void acceptableChangedTracksFieldState();
};

void TextFieldValidationGroupTest::emptyGroupIsAcceptable()
{
    QtMaterialTextFieldValidationGroup group;

    QVERIFY(group.isEmpty());
    QCOMPARE(group.count(), 0);
    QVERIFY(group.isAcceptable());
    QVERIFY(group.validateAll());
}

void TextFieldValidationGroupTest::addAndRemoveFields()
{
    QtMaterialTextFieldValidationGroup group;
    QtMaterialOutlinedTextField first;
    QtMaterialOutlinedTextField second;

    QSignalSpy addedSpy(&group, &QtMaterialTextFieldValidationGroup::fieldAdded);
    QSignalSpy removedSpy(&group, &QtMaterialTextFieldValidationGroup::fieldRemoved);

    group.addField(&first);
    group.addField(&first);
    group.addField(&second);

    QCOMPARE(group.count(), 2);
    QCOMPARE(addedSpy.count(), 2);

    group.removeField(&first);

    QCOMPARE(group.count(), 1);
    QCOMPARE(group.fields().first(), &second);
    QCOMPARE(removedSpy.count(), 1);

    group.clear();

    QVERIFY(group.isEmpty());
    QCOMPARE(removedSpy.count(), 2);
}

void TextFieldValidationGroupTest::validateAllReturnsFalseWhenAnyFieldIsInvalid()
{
    QtMaterialTextFieldValidationGroup group;
    QtMaterialOutlinedTextField first;
    QtMaterialOutlinedTextField second;

    QRegularExpressionValidator digits(QRegularExpression(QStringLiteral("^\\d+$")));
    first.setValidator(&digits);
    first.setValidationFeedbackMode(QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnCommit);
    first.setText(QStringLiteral("abc"));

    second.setValidator(&digits);
    second.setValidationFeedbackMode(QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnCommit);
    second.setText(QStringLiteral("123"));

    group.addField(&first);
    group.addField(&second);

    QSignalSpy validationSpy(&group, &QtMaterialTextFieldValidationGroup::validationRequested);

    QVERIFY(!group.validateAll());
    QCOMPARE(validationSpy.count(), 1);
    QCOMPARE(validationSpy.takeFirst().at(0).toBool(), false);
    QCOMPARE(group.firstInvalidField(), &first);
    QVERIFY(!group.isAcceptable());

    first.setText(QStringLiteral("456"));
    QVERIFY(group.validateAll());
    QVERIFY(group.isAcceptable());
}

void TextFieldValidationGroupTest::resetValidationFeedbackForwardsToFields()
{
    QtMaterialTextFieldValidationGroup group;
    QtMaterialOutlinedTextField field;

    QRegularExpressionValidator digits(QRegularExpression(QStringLiteral("^\\d+$")));
    field.setValidator(&digits);
    field.setValidationFeedbackMode(QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnCommit);
    field.setText(QStringLiteral("abc"));

    group.addField(&field);

    QVERIFY(!group.validateAll());
    QVERIFY(field.hasAutomaticValidationError());

    group.resetValidationFeedback();

    QVERIFY(!field.hasAutomaticValidationError());
}

void TextFieldValidationGroupTest::acceptableChangedTracksFieldState()
{
    QtMaterialTextFieldValidationGroup group;
    QtMaterialOutlinedTextField field;

    QRegularExpressionValidator digits(QRegularExpression(QStringLiteral("^\\d+$")));
    field.setValidator(&digits);
    field.setValidationFeedbackMode(QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnEdit);
    field.setText(QStringLiteral("123"));

    group.addField(&field);
    QVERIFY(group.isAcceptable());

    QSignalSpy acceptableSpy(&group, &QtMaterialTextFieldValidationGroup::acceptableChanged);

    field.setText(QStringLiteral("abc"));

    QTRY_VERIFY(!group.isAcceptable());
    QVERIFY(acceptableSpy.count() >= 1);

    field.setText(QStringLiteral("456"));

    QTRY_VERIFY(group.isAcceptable());
}

QTEST_MAIN(TextFieldValidationGroupTest)

#include "tst_textfield_validation_group.moc"
