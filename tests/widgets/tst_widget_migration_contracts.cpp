#include <QtTest/QtTest>

#include <QSignalSpy>
#include <QWidget>

#include "qtmaterial/core/qtmaterialabstractbutton.h"
#include "qtmaterial/core/qtmaterialautomation.h"
#include "qtmaterial/core/qtmaterialformfieldcontrol.h"
#include "qtmaterial/core/qtmaterialinputcontrol.h"
#include "qtmaterial/core/qtmaterialnavigationmodel.h"
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"
#include "qtmaterial/widgets/navigation/qtmaterialtabs.h"
#include "qtmaterial/widgets/progress/qtmaterialcircularprogressindicator.h"
#include "qtmaterial/widgets/progress/qtmateriallinearprogressindicator.h"

class TestMaterialButton : public QtMaterial::QtMaterialAbstractButton {
public:
    explicit TestMaterialButton(QWidget* parent = nullptr) : QtMaterialAbstractButton(parent) {}
protected:
    void paintEvent(QPaintEvent*) override {}
};

class tst_WidgetMigrationContracts : public QObject {
    Q_OBJECT
private slots:
    void inputControlIsFormFieldControl();
    void textFieldUsesFormFieldContract();
    void progressIndicatorsExposeAsyncStateAndMaterialMetadata();
    void tabsBindToNavigationModel();
    void abstractButtonExposesAutomationMetadata();
};

void tst_WidgetMigrationContracts::inputControlIsFormFieldControl() {
    QtMaterial::QtMaterialInputControl input;
    auto* formField = qobject_cast<QtMaterial::QtMaterialFormFieldControl*>(&input);
    QVERIFY(formField != nullptr);

    input.setLabelText(QStringLiteral("Name"));
    input.setSupportingText(QStringLiteral("Required"));
    input.setErrorText(QStringLiteral("Invalid"));
    input.setHasErrorState(true);

    QCOMPARE(formField->label(), QStringLiteral("Name"));
    QCOMPARE(formField->helperText(), QStringLiteral("Required"));
    QCOMPARE(formField->errorText(), QStringLiteral("Invalid"));
    QVERIFY(formField->isInvalid());
    QCOMPARE(input.labelText(), formField->label());
    QCOMPARE(input.supportingText(), formField->helperText());
    QCOMPARE(input.errorText(), formField->errorText());
}

void tst_WidgetMigrationContracts::textFieldUsesFormFieldContract() {
    QtMaterial::QtMaterialOutlinedTextField field;
    auto* formField = qobject_cast<QtMaterial::QtMaterialFormFieldControl*>(&field);
    QVERIFY(formField != nullptr);

    field.setLabelText(QStringLiteral("Email"));
    field.setSupportingText(QStringLiteral("Use your work email"));
    field.setErrorText(QStringLiteral("Invalid email"));
    field.setHasErrorState(true);

    QCOMPARE(formField->label(), QStringLiteral("Email"));
    QCOMPARE(formField->helperText(), QStringLiteral("Use your work email"));
    QCOMPARE(formField->errorText(), QStringLiteral("Invalid email"));
    QVERIFY(formField->isInvalid());
}

void tst_WidgetMigrationContracts::progressIndicatorsExposeAsyncStateAndMaterialMetadata() {
    QtMaterial::QtMaterialLinearProgressIndicator linear;
    QCOMPARE(linear.materialComponent(), QStringLiteral("LinearProgressIndicator"));
    linear.setBusy(true);
    QVERIFY(linear.isBusy());
    linear.setIndeterminate(true);
    QVERIFY(linear.isIndeterminate());
    QVERIFY(linear.materialState().contains(QStringLiteral("busy")));
    QVERIFY(linear.materialState().contains(QStringLiteral("indeterminate")));
    linear.setIndeterminate(false);
    linear.setValue(0.5);
    QVERIFY(linear.asyncState().hasProgress());
    QCOMPARE(linear.asyncState().progress(), 0.5);

    QtMaterial::QtMaterialCircularProgressIndicator circular;
    QCOMPARE(circular.materialComponent(), QStringLiteral("CircularProgressIndicator"));
    circular.setStatusText(QStringLiteral("Loading"));
    QCOMPARE(circular.statusText(), QStringLiteral("Loading"));
    QVERIFY(circular.materialState().contains(QStringLiteral("statusText")));

    QCOMPARE(linear.property(QtMaterial::QtMaterialAutomation::componentPropertyName()).toString(),
             QStringLiteral("LinearProgressIndicator"));

    linear.setBusy(true);
    QVERIFY(linear.property(QtMaterial::QtMaterialAutomation::statePropertyName())
                .toString()
                .contains(QStringLiteral("busy")));
}

void tst_WidgetMigrationContracts::tabsBindToNavigationModel() {
    QtMaterial::QtMaterialTabs tabs;
    tabs.addTab(new QWidget(&tabs), QStringLiteral("Home"));
    tabs.addTab(new QWidget(&tabs), QStringLiteral("Settings"));
    tabs.setTabId(0, QStringLiteral("home"));
    tabs.setTabId(1, QStringLiteral("settings"));
    tabs.setRoute(0, QStringLiteral("/home"));
    tabs.setRoute(1, QStringLiteral("/settings"));

    QtMaterial::QtMaterialNavigationModel model;
    tabs.setNavigationModel(&model);

    QCOMPARE(tabs.navigationModel(), &model);
    QCOMPARE(model.rowCount(), 2);
    QCOMPARE(model.itemAt(0).id, QStringLiteral("home"));
    QCOMPARE(model.itemAt(1).route, QStringLiteral("/settings"));

    QVERIFY(model.setSelectedRoute(QStringLiteral("/settings")));
    QCOMPARE(tabs.currentIndex(), 1);

    tabs.setCurrentIndex(0);
    QCOMPARE(model.selectedRoute(), QStringLiteral("/home"));
}

void tst_WidgetMigrationContracts::abstractButtonExposesAutomationMetadata() {
    TestMaterialButton button;
    button.setMaterialComponent(QStringLiteral("Button"));
    button.setMaterialVariant(QStringLiteral("Filled"));
    button.setMaterialRole(QStringLiteral("primaryAction"));
    button.setMaterialTestId(QStringLiteral("save-button"));

    QCOMPARE(button.materialComponent(), QStringLiteral("Button"));
    QCOMPARE(button.property(QtMaterial::QtMaterialAutomation::componentPropertyName()).toString(), QStringLiteral("Button"));
    QCOMPARE(button.materialVariant(), QStringLiteral("Filled"));
    QCOMPARE(button.materialRole(), QStringLiteral("primaryAction"));
    QCOMPARE(button.materialTestId(), QStringLiteral("save-button"));

    button.setCheckable(true);
    button.setChecked(true);
    QVERIFY(button.property(QtMaterial::QtMaterialAutomation::statePropertyName()).toString().contains(QStringLiteral("checked"))); 
}

QTEST_MAIN(tst_WidgetMigrationContracts)
#include "tst_widget_migration_contracts.moc"
