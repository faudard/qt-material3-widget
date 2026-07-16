#include <QtTest/QtTest>

#include "qtmaterial/specs/qtmaterialselectionspecresolver.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_SelectionSpecResolver : public QObject
{
    Q_OBJECT

private slots:
    void resolvesDefaultColors();
    void resolvesDensity();
    void remainsCompatibleWithSpecFactory();
};

void tst_SelectionSpecResolver::resolvesDefaultColors()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));
    SelectionSpecResolver resolver;

    const CheckboxSpec checkbox = resolver.checkboxSpec(theme);
    const RadioButtonSpec radio = resolver.radioButtonSpec(theme);
    const SwitchSpec switchSpec = resolver.switchSpec(theme);

    QCOMPARE(
        checkbox.selectedContainerColor,
        theme.colorScheme().color(ColorRole::Primary));
    QCOMPARE(
        radio.selectedColor,
        theme.colorScheme().color(ColorRole::Primary));
    QCOMPARE(
        switchSpec.selectedTrackColor,
        theme.colorScheme().color(ColorRole::Primary));
    QCOMPARE(
        switchSpec.selectedHandleColor,
        theme.colorScheme().color(ColorRole::OnPrimary));
}

void tst_SelectionSpecResolver::resolvesDensity()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));
    SelectionSpecResolver resolver;

    QCOMPARE(
        resolver.checkboxSpec(theme, Density::Compact).spacing,
        8);
    QCOMPARE(
        resolver.checkboxSpec(theme, Density::Comfortable).spacing,
        14);
    QCOMPARE(
        resolver.radioButtonSpec(theme, Density::Compact).spacing,
        8);
    QCOMPARE(
        resolver.switchSpec(theme, Density::Comfortable).spacing,
        14);
}

void tst_SelectionSpecResolver::remainsCompatibleWithSpecFactory()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildDarkFromSeed(QColor(QStringLiteral("#6750A4")));
    SelectionSpecResolver resolver;
    SpecFactory factory;

    const CheckboxSpec resolvedCheckbox =
        resolver.checkboxSpec(theme, Density::Compact);
    const CheckboxSpec legacyCheckbox =
        factory.checkboxSpec(theme, Density::Compact);

    QCOMPARE(
        resolvedCheckbox.selectedContainerColor,
        legacyCheckbox.selectedContainerColor);
    QCOMPARE(
        resolvedCheckbox.unselectedOutlineColor,
        legacyCheckbox.unselectedOutlineColor);
    QCOMPARE(resolvedCheckbox.spacing, legacyCheckbox.spacing);

    const RadioButtonSpec resolvedRadio =
        resolver.radioButtonSpec(theme, Density::Comfortable);
    const RadioButtonSpec legacyRadio =
        factory.radioButtonSpec(theme, Density::Comfortable);

    QCOMPARE(resolvedRadio.selectedColor, legacyRadio.selectedColor);
    QCOMPARE(
        resolvedRadio.unselectedOutlineColor,
        legacyRadio.unselectedOutlineColor);
    QCOMPARE(resolvedRadio.spacing, legacyRadio.spacing);

    const SwitchSpec resolvedSwitch =
        resolver.switchSpec(theme, Density::Default);
    const SwitchSpec legacySwitch =
        factory.switchSpec(theme, Density::Default);

    QCOMPARE(
        resolvedSwitch.selectedTrackColor,
        legacySwitch.selectedTrackColor);
    QCOMPARE(
        resolvedSwitch.unselectedHandleColor,
        legacySwitch.unselectedHandleColor);
    QCOMPARE(resolvedSwitch.spacing, legacySwitch.spacing);
}

QTEST_MAIN(tst_SelectionSpecResolver)

#include "tst_selectionspecresolver.moc"
