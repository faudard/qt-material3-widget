#include <QtTest/QtTest>

#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"
#include "qtmaterial/widgets/inputs/qtmaterialtextfieldvalidationgroup.h"

using namespace QtMaterial;

class tst_TextFieldValidationGroupLifecycle : public QObject
{
    Q_OBJECT

private slots:
    void removingInvalidFieldRecomputesAcceptableState();
    void destroyedFieldIsForgottenByGroup();
};

void tst_TextFieldValidationGroupLifecycle::removingInvalidFieldRecomputesAcceptableState()
{
    QtMaterialTextFieldValidationGroup group;
    QtMaterialOutlinedTextField field;
    field.setRequired(true);
    field.setValidationFeedbackMode(QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnEdit);

    group.addField(&field);
    QVERIFY(!group.isAcceptable());

    group.removeField(&field);
    QVERIFY(group.isAcceptable());
}

void tst_TextFieldValidationGroupLifecycle::destroyedFieldIsForgottenByGroup()
{
    QtMaterialTextFieldValidationGroup group;
    auto* field = new QtMaterialOutlinedTextField;
    field->setRequired(true);
    field->setValidationFeedbackMode(QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnEdit);
    group.addField(field);
    QVERIFY(!group.isAcceptable());

    delete field;

    QVERIFY(group.isAcceptable());
}

QTEST_MAIN(tst_TextFieldValidationGroupLifecycle)
#include "tst_textfield_validation_group_lifecycle.moc"
