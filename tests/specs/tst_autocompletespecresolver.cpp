#include <QtTest>

#include "qtmaterial/specs/qtmaterialautocompletespecresolver.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_AutocompleteSpecResolver : public QObject
{
    Q_OBJECT

private slots:
    void resolvesThemeRoles();
    void resolvesDensity();
    void remainsCompatibleWithSpecFactory();
};

void tst_AutocompleteSpecResolver::resolvesThemeRoles()
{
    const Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    const AutocompleteSpec spec =
        AutocompleteSpecResolver().autocompleteSpec(theme);

    QCOMPARE(
        spec.inputContainerColor,
        theme.colorScheme().color(ColorRole::SurfaceContainerHighest));
    QCOMPARE(
        spec.popupContainerColor,
        theme.colorScheme().color(ColorRole::SurfaceContainerHigh));
    QCOMPARE(
        spec.focusedOutlineColor,
        theme.colorScheme().color(ColorRole::Primary));
}

void tst_AutocompleteSpecResolver::resolvesDensity()
{
    const Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    const AutocompleteSpecResolver resolver;

    QCOMPARE(
        resolver.autocompleteSpec(theme, Density::Compact).inputMinHeight,
        48);
    QCOMPARE(
        resolver.autocompleteSpec(theme, Density::Default).inputMinHeight,
        56);
    QCOMPARE(
        resolver.autocompleteSpec(theme, Density::Comfortable).inputMinHeight,
        60);
}

void tst_AutocompleteSpecResolver::remainsCompatibleWithSpecFactory()
{
    const Theme theme = ThemeBuilder().buildDarkFromSeed(
        QColor(QStringLiteral("#6750A4")));
    const AutocompleteSpec resolved =
        AutocompleteSpecResolver().autocompleteSpec(
            theme, Density::Compact);
    const AutocompleteSpec legacy =
        AutocompleteSpecResolver().autocompleteSpec(theme, Density::Compact);

    QCOMPARE(resolved.inputContainerColor, legacy.inputContainerColor);
    QCOMPARE(resolved.popupContainerColor, legacy.popupContainerColor);
    QCOMPARE(resolved.focusRingColor, legacy.focusRingColor);
    QCOMPARE(resolved.inputMinHeight, legacy.inputMinHeight);
}

QTEST_MAIN(tst_AutocompleteSpecResolver)
#include "tst_autocompletespecresolver.moc"
