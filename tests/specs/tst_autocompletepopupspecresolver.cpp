#include <QtTest/QtTest>

#include "qtmaterial/specs/qtmaterialautocompletepopupspecresolver.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_AutocompletePopupSpecResolver : public QObject
{
    Q_OBJECT

private slots:
    void resolvesRuntimeValues();
    void resolvesComponentOverrides();
    void remainsCompatibleWithSpecFactory();
};

void tst_AutocompletePopupSpecResolver::resolvesRuntimeValues()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));

    const AutocompletePopupSpec spec =
        AutocompletePopupSpecResolver()
            .autocompletePopupSpec(theme);

    QCOMPARE(
        spec.containerColor,
        theme.colorScheme().color(
            ColorRole::SurfaceContainerHigh));
    QCOMPARE(
        spec.selectedContainerColor,
        theme.colorScheme().color(
            ColorRole::SecondaryContainer));
    QCOMPARE(
        spec.cornerRadius,
        theme.shapes().radius(ShapeRole::Large));
    QVERIFY(spec.hasResolvedItemFont);
    QVERIFY(spec.hasResolvedSupportingFont);
    QVERIFY(spec.hasResolvedElevationStyle);
    QVERIFY(spec.hasResolvedMotionStyle);
    QCOMPARE(
        spec.elevationStyle.shadowBlur,
        theme.elevations()
            .style(ElevationRole::Level2)
            .shadowBlur);
    QCOMPARE(
        spec.pressStateLayerOpacity,
        theme.stateLayer().pressOpacity);
}

void tst_AutocompletePopupSpecResolver::resolvesComponentOverrides()
{
    ThemeBuilder builder;
    Theme theme =
        builder.buildDarkFromSeed(
            QColor(QStringLiteral("#146C2E")));

    TypographyStyle itemTypography;
    itemTypography.font =
        QFont(QStringLiteral("Monospace"), 13);

    ComponentTokenOverride tokens;
    tokens.typography.insert(
        TypeRole::BodyLarge,
        itemTypography);
    tokens.custom.insert(
        QStringLiteral("cornerRadius"),
        10);
    tokens.custom.insert(
        QStringLiteral("visibleItemCount"),
        8);
    tokens.custom.insert(
        QStringLiteral("itemMinHeight"),
        52);
    tokens.custom.insert(
        QStringLiteral("pressStateLayerOpacity"),
        0.18);

    theme.componentOverrides().setOverride(
        QStringLiteral("autocompletePopup"),
        tokens);

    const AutocompletePopupSpec spec =
        AutocompletePopupSpecResolver()
            .autocompletePopupSpec(theme);

    QCOMPARE(spec.itemFont, itemTypography.font);
    QCOMPARE(spec.cornerRadius, 10);
    QCOMPARE(spec.visibleItemCount, 8);
    QCOMPARE(spec.itemMinSize.height(), 52);
    QCOMPARE(spec.pressStateLayerOpacity, 0.18);
}

void tst_AutocompletePopupSpecResolver::remainsCompatibleWithSpecFactory()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));

    const AutocompletePopupSpec resolved =
        AutocompletePopupSpecResolver()
            .autocompletePopupSpec(theme);
    const AutocompletePopupSpec legacy =
        SpecFactory().autocompletePopupSpec(theme);

    QCOMPARE(
        resolved.containerColor,
        legacy.containerColor);
    QCOMPARE(
        resolved.cornerRadius,
        legacy.cornerRadius);
    QCOMPARE(
        resolved.elevationStyle.shadowBlur,
        legacy.elevationStyle.shadowBlur);
    QCOMPARE(
        resolved.motionStyle.durationMs,
        legacy.motionStyle.durationMs);
}

QTEST_MAIN(tst_AutocompletePopupSpecResolver)

#include "tst_autocompletepopupspecresolver.moc"
