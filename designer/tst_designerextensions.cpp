#include <QtTest/QTest>

#include <limits>
#include <QColor>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QListWidget>
#include <QScopedPointer>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QToolButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QSlider>
#include <QWidget>

#include "qtmaterial3designerextensions.h"

#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/widgets/buttons/qtmaterialbuttongroup.h"
#include "qtmaterial/widgets/buttons/qtmaterialsplitbutton.h"
#include "qtmaterial/widgets/data/qtmaterialdivider.h"
#include "qtmaterial/widgets/data/qtmaterialsegmentedlist.h"
#include "qtmaterial/widgets/layouts/qtmaterialadaptiveshell.h"
#include "qtmaterial/widgets/navigation/qtmaterialnavigationbar.h"
#include "qtmaterial/widgets/navigation/qtmaterialtabs.h"
#include "qtmaterial/widgets/native/qtmaterialnativeadapter.h"

using namespace QtMaterial;

class QtMaterialPropertyFixture : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double precise MEMBER precise)
    Q_PROPERTY(uint unsignedValue MEMBER unsignedValue)
public:
    double precise = 0.123456789;
    uint unsignedValue = std::numeric_limits<uint>::max();
};

class DesignerExtensionsTest : public QObject
{
    Q_OBJECT

private slots:
    void nativePoliciesUseStrictDynamicPropertyTypes()
    {
        QPushButton button;
        const QStringList names = QtMaterial3Designer::nativeEditablePropertyNames(&button);
        QVERIFY(names.contains(QStringLiteral("qtm3MaterialAdapt")));
        QVERIFY(names.contains(QStringLiteral("qtm3MaterialVariant")));
        QVERIFY(names.contains(QStringLiteral("qtm3MaterialDensity")));
        QVERIFY(names.contains(QStringLiteral("qtm3MaterialOptOut")));
        QVERIFY(!names.contains(QStringLiteral("qtm3MaterialTextFieldVariant")));

        QVERIFY(QtMaterial3Designer::setNativeDesignerProperty(
            &button, QStringLiteral("qtm3MaterialAdapt"), true));
        QVERIFY(QtMaterial3Designer::setNativeDesignerProperty(
            &button, QStringLiteral("qtm3MaterialVariant"), QStringLiteral("filled-tonal")));
        QVERIFY(QtMaterial3Designer::setNativeDesignerProperty(
            &button, QStringLiteral("qtm3MaterialDensity"), QStringLiteral("compact")));
        QVERIFY(!QtMaterial3Designer::setNativeDesignerProperty(
            &button, QStringLiteral("qtm3MaterialAdapt"), QStringLiteral("true")));
        QVERIFY(!QtMaterial3Designer::setNativeDesignerProperty(
            &button, QStringLiteral("qtm3MaterialVariant"), QStringLiteral("invalid")));
        QVERIFY(!QtMaterial3Designer::setNativeDesignerProperty(
            &button, QStringLiteral("unknown"), true));
        QCOMPARE(button.property("qtm3MaterialVariant").toString(), QStringLiteral("filled-tonal"));
        QVERIFY(QtMaterialNativeAdapter::applyDeclared(&button));
        QVERIFY(QtMaterialNativeAdapter::isApplied(&button));
        QCOMPARE(button.metaObject()->className(), "QPushButton");
        QVERIFY(QtMaterialNativeAdapter::remove(&button));

        QVERIFY(QtMaterial3Designer::setNativeDesignerProperty(
            &button, QStringLiteral("qtm3MaterialOptOut"), true));
        QVERIFY(!QtMaterialNativeAdapter::applyDeclared(&button));
        QVERIFY(QtMaterial3Designer::resetNativeDesignerProperties(&button));
        QCOMPARE(button.property("qtm3MaterialAdapt").toBool(), false);
        QCOMPARE(button.property("qtm3MaterialVariant").toString(), QStringLiteral("text"));
        QCOMPARE(button.property("qtm3MaterialDensity").toString(), QStringLiteral("default"));
        QCOMPARE(button.property("qtm3MaterialOptOut").toBool(), false);

        QLineEdit lineEdit;
        QVERIFY(QtMaterial3Designer::nativeEditablePropertyNames(&lineEdit)
                    .contains(QStringLiteral("qtm3MaterialTextFieldVariant")));
        QVERIFY(!QtMaterial3Designer::nativeEditablePropertyNames(&lineEdit)
                    .contains(QStringLiteral("qtm3MaterialVariant")));
        QProgressBar progress;
        QVERIFY(!QtMaterial3Designer::nativeEditablePropertyNames(&progress)
                    .contains(QStringLiteral("qtm3MaterialDensity")));
        QtMaterialSplitButton firstClass;
        QVERIFY(QtMaterial3Designer::nativeEditablePropertyNames(&firstClass).isEmpty());
    }

    void removingNativeDeclarationsIsDistinctFromReset()
    {
        QPushButton button;
        QVERIFY(QtMaterial3Designer::setNativeDesignerProperty(
            &button, QStringLiteral("qtm3MaterialAdapt"), true));
        QVERIFY(QtMaterial3Designer::setNativeDesignerProperty(
            &button, QStringLiteral("qtm3MaterialVariant"), QStringLiteral("filled")));
        QVERIFY(QtMaterial3Designer::setNativeDesignerProperty(
            &button, QStringLiteral("qtm3MaterialOptOut"), true));

        // Reset preserves authored property names and writes canonical defaults.
        QVERIFY(QtMaterial3Designer::resetNativeDesignerProperties(&button));
        QCOMPARE(button.property("qtm3MaterialAdapt").toBool(), false);
        QVERIFY(button.dynamicPropertyNames().contains("qtm3MaterialAdapt"));
        QVERIFY(button.dynamicPropertyNames().contains("qtm3MaterialVariant"));

        // Clear removes the declarations (not just the effective values).
        QVERIFY(QtMaterial3Designer::clearNativeDesignerProperties(&button));
        QVERIFY(!button.dynamicPropertyNames().contains("qtm3MaterialAdapt"));
        QVERIFY(!button.dynamicPropertyNames().contains("qtm3MaterialVariant"));
        QVERIFY(!button.dynamicPropertyNames().contains("qtm3MaterialOptOut"));
        QVERIFY(!QtMaterialNativeAdapter::isDeclared(&button));
        QVERIFY(!QtMaterialNativeAdapter::applyDeclared(&button));
        QVERIFY(!QtMaterial3Designer::clearNativeDesignerProperties(&button));

        // A new declaration remains possible following a removal.
        QVERIFY(QtMaterial3Designer::setNativeDesignerProperty(
            &button, QStringLiteral("qtm3MaterialAdapt"), true));
        QVERIFY(QtMaterialNativeAdapter::applyDeclared(&button));
        QVERIFY(QtMaterialNativeAdapter::remove(&button));
        QVERIFY(QtMaterial3Designer::clearNativeDesignerProperties(&button));

        QtMaterialSplitButton material;
        QVERIFY(!QtMaterial3Designer::clearNativeDesignerProperties(&material));
    }

    void nativePolicyMatrixRejectsUnsupportedDeclarations()
    {
        QPushButton push;
        QToolButton tool;
        QCheckBox check;
        QRadioButton radio;
        QSlider slider;
        QComboBox combo;
        QLineEdit line;
        QProgressBar progress;
        const QList<QWidget*> controls = {
            &push, &tool, &check, &radio, &slider, &combo, &line, &progress
        };
        for (QWidget* control : controls) {
            const QStringList names =
                QtMaterial3Designer::nativeEditablePropertyNames(control);
            QVERIFY(names.contains(QStringLiteral("qtm3MaterialAdapt")));
            QVERIFY(names.contains(QStringLiteral("qtm3MaterialOptOut")));
            QVERIFY(QtMaterial3Designer::setNativeDesignerProperty(
                control, QStringLiteral("qtm3MaterialAdapt"), true));
            QVERIFY(!QtMaterial3Designer::setNativeDesignerProperty(
                control, QStringLiteral("qtm3MaterialAdapt"), QStringLiteral("true")));
            QVERIFY(!QtMaterial3Designer::setNativeDesignerProperty(
                control, QStringLiteral("qtm3MaterialOptOut"), 1));
            QVERIFY(!QtMaterial3Designer::setNativeDesignerProperty(
                control, QStringLiteral("notAProperty"), true));
        }

        const QString variant = QStringLiteral("qtm3MaterialVariant");
        const QString fieldVariant = QStringLiteral("qtm3MaterialTextFieldVariant");
        const QString density = QStringLiteral("qtm3MaterialDensity");
        QVERIFY(QtMaterial3Designer::setNativeDesignerProperty(
            &tool, variant, QStringLiteral("elevated")));
        QVERIFY(QtMaterial3Designer::setNativeDesignerProperty(
            &line, fieldVariant, QStringLiteral("filled")));
        QVERIFY(!QtMaterial3Designer::setNativeDesignerProperty(
            &check, variant, QStringLiteral("outlined")));
        QVERIFY(!QtMaterial3Designer::setNativeDesignerProperty(
            &push, fieldVariant, QStringLiteral("filled")));
        QVERIFY(!QtMaterial3Designer::setNativeDesignerProperty(
            &progress, density, QStringLiteral("compact")));
        QVERIFY(!QtMaterial3Designer::setNativeDesignerProperty(
            &slider, density, QStringLiteral("narrow")));
        QCOMPARE(tool.property("qtm3MaterialVariant").toString(),
                 QStringLiteral("elevated"));
        QCOMPARE(line.property("qtm3MaterialTextFieldVariant").toString(),
                 QStringLiteral("filled"));
        QCOMPARE(progress.property("qtm3MaterialDensity").isValid(), false);

        for (QWidget* control : controls) {
            QVERIFY(QtMaterial3Designer::resetNativeDesignerProperties(control));
            QCOMPARE(control->property("qtm3MaterialAdapt").toBool(), false);
        }
    }

    void exposesCuratedEditableProperties()
    {
        QtMaterialSplitButton split;
        const QStringList splitProperties =
            QtMaterial3Designer::editablePropertyNames(&split);
        QVERIFY(splitProperties.contains(QStringLiteral("text")));
        QVERIFY(splitProperties.contains(QStringLiteral("expressive")));
        QVERIFY(splitProperties.contains(QStringLiteral("expressiveSize")));
        QVERIFY(!splitProperties.contains(QStringLiteral("geometry")));

        QtMaterialButtonGroup group;
        QVERIFY(QtMaterial3Designer::editablePropertyNames(&group)
                    .contains(QStringLiteral("buttonLabels")));

        QtMaterialNavigationBar bar;
        QVERIFY(QtMaterial3Designer::editablePropertyNames(&bar)
                    .contains(QStringLiteral("destinationLabels")));

        QtMaterialSegmentedList list;
        QVERIFY(QtMaterial3Designer::editablePropertyNames(&list)
                    .contains(QStringLiteral("itemLabels")));

        QtMaterialDivider divider;
        const QStringList dividerProperties =
            QtMaterial3Designer::editablePropertyNames(&divider);
        QVERIFY(dividerProperties.contains(QStringLiteral("color")));
        QVERIFY(dividerProperties.contains(QStringLiteral("thickness")));

        QtMaterialAdaptiveShell shell;
        const QStringList shellProperties =
            QtMaterial3Designer::editablePropertyNames(&shell);
        QVERIFY(shellProperties.contains(QStringLiteral("automaticDensity")));
        QVERIFY(shellProperties.contains(QStringLiteral("supportingPaneWidth")));
        QVERIFY(!shellProperties.contains(QStringLiteral("supportingPaneVisible")));
    }

    void propertyDialogNoOpAndCancelPreserveOriginalValues()
    {
        QtMaterialPropertyFixture fixture;
        const double original = fixture.precise;
        const uint originalUnsigned = fixture.unsignedValue;
        {
            QScopedPointer<QDialog> dialog(QtMaterial3Designer::createMaterialPropertyDialog(&fixture));
            QVERIFY(dialog);
            auto* editor = dialog->findChild<QDoubleSpinBox*>(QStringLiteral("unsignedValue"));
            QVERIFY(editor);
            QCOMPARE(editor->value(), static_cast<double>(originalUnsigned));
            auto* buttons = dialog->findChild<QDialogButtonBox*>();
            QVERIFY(buttons);
            buttons->button(QDialogButtonBox::Ok)->click();
            QCOMPARE(dialog->result(), int(QDialog::Accepted));
            QCOMPARE(fixture.precise, original);
            QCOMPARE(fixture.unsignedValue, originalUnsigned);
        }
        {
            QScopedPointer<QDialog> dialog(QtMaterial3Designer::createMaterialPropertyDialog(&fixture));
            auto* editor = dialog->findChild<QDoubleSpinBox*>(QStringLiteral("unsignedValue"));
            QVERIFY(editor);
            editor->setValue(42);
            dialog->findChild<QDialogButtonBox*>()->button(QDialogButtonBox::Cancel)->click();
            QCOMPARE(fixture.unsignedValue, originalUnsigned);
        }
        {
            QScopedPointer<QDialog> dialog(QtMaterial3Designer::createMaterialPropertyDialog(&fixture));
            auto* editor = dialog->findChild<QDoubleSpinBox*>(QStringLiteral("unsignedValue"));
            QVERIFY(editor);
            const uint changedValue = originalUnsigned - 1;
            editor->setValue(static_cast<double>(changedValue));
            dialog->findChild<QDialogButtonBox*>()->button(QDialogButtonBox::Ok)->click();
            QCOMPARE(fixture.unsignedValue, changedValue);
            QCOMPARE(fixture.precise, original);
        }
    }

    void collectionEditorItemsAreEditableAndPersistOnAccept()
    {
        QtMaterialNavigationBar bar;
        bar.setDestinationLabels({QStringLiteral("Home"), QStringLiteral("Account")});
        QScopedPointer<QDialog> dialog(QtMaterial3Designer::createMaterialPropertyDialog(&bar));
        QVERIFY(dialog);
        auto* collection = dialog->findChild<QWidget*>(QStringLiteral("destinationLabels"));
        QVERIFY(collection);
        auto* list = collection->findChild<QListWidget*>();
        QVERIFY(list);
        QCOMPARE(list->count(), 2);
        for (int i = 0; i < list->count(); ++i) {
            QVERIFY(list->item(i)->flags().testFlag(Qt::ItemIsEditable));
        }
        list->item(1)->setText(QStringLiteral("Profile"));
        dialog->findChild<QDialogButtonBox*>()->button(QDialogButtonBox::Ok)->click();
        QCOMPARE(bar.destinationLabels(), QStringList({QStringLiteral("Home"), QStringLiteral("Profile")}));
    }

    void tabsContainerRoundTripsPages()
    {
        QtMaterialTabs tabs;
        QtMaterial3Designer::TabsContainerExtension extension(&tabs);

        auto* first = new QWidget;
        first->setObjectName(QStringLiteral("overviewPage"));
        auto* second = new QWidget;
        second->setObjectName(QStringLiteral("detailsPage"));

        extension.addWidget(first);
        extension.addWidget(second);

        QCOMPARE(extension.count(), 2);
        QCOMPARE(extension.widget(0), first);
        QCOMPARE(extension.widget(1), second);
        QCOMPARE(extension.currentIndex(), 1);

        extension.setCurrentIndex(0);
        QCOMPARE(tabs.currentIndex(), 0);

        extension.remove(1);
        QCOMPARE(extension.count(), 1);
    }

    void adaptiveShellContainerOwnsTwoDesignerSlots()
    {
        QtMaterialAdaptiveShell shell;
        QtMaterial3Designer::AdaptiveShellContainerExtension extension(&shell);

        auto* content = new QWidget;
        auto* supporting = new QWidget;

        extension.addWidget(content);
        extension.addWidget(supporting);

        QCOMPARE(extension.count(), 2);
        QCOMPARE(shell.contentWidget(), content);
        QCOMPARE(shell.supportingWidget(), supporting);

        extension.remove(1);
        QCOMPARE(extension.count(), 1);
        QVERIFY(shell.supportingWidget() == nullptr);
        QCOMPARE(shell.contentWidget(), content);
    }

    void resettableTokenPropertiesReturnToThemeDefaults()
    {
        QtMaterialDivider divider;
        divider.setColor(QColor(QStringLiteral("#ff0000")));
        QVERIFY(divider.color().isValid());
        QVERIFY(QtMaterial3Designer::propertyCanReset(
            &divider, QStringLiteral("color")));
        QVERIFY(QtMaterial3Designer::resetPropertyToDefault(
            &divider, QStringLiteral("color")));
        QVERIFY(!divider.color().isValid());
        QVERIFY(!QtMaterial3Designer::propertyCanReset(
            &divider, QStringLiteral("thickness")));
    }

    void previewWidthsMatchAdaptiveBreakpoints()
    {
        using QtMaterial3Designer::PreviewWidth;
        QCOMPARE(QtMaterial3Designer::previewLogicalWidth(PreviewWidth::Current), -1);
        QCOMPARE(QtMaterial3Designer::previewLogicalWidth(PreviewWidth::Compact), 480);
        QCOMPARE(QtMaterial3Designer::previewLogicalWidth(PreviewWidth::Medium), 720);
        QCOMPARE(QtMaterial3Designer::previewLogicalWidth(PreviewWidth::Expanded), 1024);
    }

    void previewModesAndPresetsAreReversible()
    {
        ThemeManager& manager = ThemeManager::instance();
        const ThemeOptions original = manager.options();

        QtMaterial3Designer::applyPreviewMode(
            QtMaterial3Designer::PreviewMode::Dark);
        QCOMPARE(manager.options().mode, ThemeMode::Dark);
        QCOMPARE(manager.options().variant, ThemeVariant::TonalSpot);

        QtMaterial3Designer::applyPreviewMode(
            QtMaterial3Designer::PreviewMode::Expressive);
        QCOMPARE(manager.options().mode, ThemeMode::Light);
        QCOMPARE(manager.options().variant, ThemeVariant::Expressive);

        QtMaterial3Designer::restorePreviewMode();
        QVERIFY(manager.options() == original);

        const QStringList presets = QtMaterial3Designer::designerThemePresetIds();
        QVERIFY(presets.contains(QStringLiteral("material-default-light")));
        QVERIFY(presets.contains(QStringLiteral("rose-expressive")));
        QVERIFY(QtMaterial3Designer::applyThemePreset(
            QStringLiteral("rose-expressive")));
        QCOMPARE(manager.options().sourceColor, QColor(QStringLiteral("#A73E8C")));
        QCOMPARE(manager.options().variant, ThemeVariant::Expressive);
        QCOMPARE(manager.options().contrast, ContrastMode::Medium);

        QtMaterial3Designer::restorePreviewMode();
        QVERIFY(manager.options() == original);
        QVERIFY(!QtMaterial3Designer::applyThemePreset(
            QStringLiteral("does-not-exist")));
    }
};

QTEST_MAIN(DesignerExtensionsTest)
#include "tst_designerextensions.moc"
