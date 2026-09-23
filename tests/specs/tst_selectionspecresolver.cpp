#include <QtTest/QtTest>

#include "qtmaterial/specs/qtmaterialselectionspecresolver.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_SelectionSpecResolver : public QObject
{
    Q_OBJECT

private slots:
    void resolvesDefaultColors();
    void resolvesDensity();
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

QTEST_MAIN(tst_SelectionSpecResolver)

#include "tst_selectionspecresolver.moc"
