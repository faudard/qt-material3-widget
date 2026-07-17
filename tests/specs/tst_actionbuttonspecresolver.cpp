#include <QtTest/QtTest>

#include "qtmaterial/specs/qtmaterialactionbuttonspecresolver.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_ActionButtonSpecResolver : public QObject
{
    Q_OBJECT

private slots:
    void resolvesFabRuntimeValues();
    void resolvesExtendedFab();
    void resolvesFabColorVariants();
    void resolvesIconButtonRuntimeValues();
    void remainsCompatibleWithSpecFactory();
    void resolvesComponentOverrides();
};

void tst_ActionButtonSpecResolver::resolvesFabRuntimeValues()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));

    const FabSpec spec =
        ActionButtonSpecResolver().fabSpec(theme);

    QCOMPARE(
        spec.containerColor,
        theme.colorScheme().color(
            ColorRole::PrimaryContainer));
    QCOMPARE(
        spec.shadowColor,
        theme.colorScheme().color(ColorRole::Shadow));
    QCOMPARE(spec.cornerRadius, -1.0);
    QVERIFY(spec.hasResolvedLabelFont);
    QVERIFY(spec.hasResolvedMotionStyle);
    QVERIFY(spec.hasResolvedElevationStyle);

    const ButtonSpec button =
        ActionButtonSpecResolver().fabButtonSpec(theme);
    QCOMPARE(button.containerColor, spec.containerColor);
    QCOMPARE(button.cornerRadius, spec.cornerRadius);
    QCOMPARE(
        button.elevationStyle.shadowBlur,
        spec.elevationStyle.shadowBlur);
    QCOMPARE(
        button.pressStateLayerOpacity,
        spec.pressStateLayerOpacity);
}

void tst_ActionButtonSpecResolver::resolvesExtendedFab()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildDarkFromSeed(
            QColor(QStringLiteral("#146C2E")));

    const FabSpec fab =
        ActionButtonSpecResolver().extendedFabSpec(theme);
    const ButtonSpec button =
        ActionButtonSpecResolver()
            .extendedFabButtonSpec(theme);

    QCOMPARE(fab.touchTarget, QSize(80, 56));
    QCOMPARE(button.horizontalPadding, 20);
    QCOMPARE(button.iconSpacing, 12);
    QCOMPARE(button.labelFont, fab.labelFont);
}

void tst_ActionButtonSpecResolver::resolvesFabColorVariants()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));
    ActionButtonSpecResolver resolver;

    const ButtonSpec secondary =
        resolver.extendedFabButtonSpec(
            theme,
            Density::Default,
            FabColorVariant::Secondary);
    const ButtonSpec tertiary =
        resolver.extendedFabButtonSpec(
            theme,
            Density::Default,
            FabColorVariant::Tertiary);
    const ButtonSpec surface =
        resolver.extendedFabButtonSpec(
            theme,
            Density::Default,
            FabColorVariant::Surface);

    QCOMPARE(
        secondary.containerColor,
        theme.colorScheme().color(
            ColorRole::SecondaryContainer));
    QCOMPARE(
        tertiary.containerColor,
        theme.colorScheme().color(
            ColorRole::TertiaryContainer));
    QCOMPARE(
        surface.containerColor,
        theme.colorScheme().color(
            ColorRole::SurfaceContainerHigh));
}

void tst_ActionButtonSpecResolver::resolvesIconButtonRuntimeValues()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));

    const IconButtonSpec spec =
        ActionButtonSpecResolver().iconButtonSpec(theme);

    QCOMPARE(spec.cornerRadius, -1.0);
    QVERIFY(spec.hasResolvedMotionStyle);
    QCOMPARE(
        spec.focusRingColor,
        theme.colorScheme().color(ColorRole::Primary));
    QCOMPARE(
        spec.pressStateLayerOpacity,
        theme.stateLayer().pressOpacity);
}

void tst_ActionButtonSpecResolver::remainsCompatibleWithSpecFactory()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));

    ActionButtonSpecResolver resolver;
    SpecFactory factory;

    const FabSpec resolvedFab = resolver.fabSpec(theme);
    const FabSpec legacyFab = factory.fabSpec(theme);
    QCOMPARE(
        resolvedFab.containerColor,
        legacyFab.containerColor);
    QCOMPARE(
        resolvedFab.cornerRadius,
        legacyFab.cornerRadius);

    const IconButtonSpec resolvedIcon =
        resolver.iconButtonSpec(theme);
    const IconButtonSpec legacyIcon =
        factory.iconButtonSpec(theme);
    QCOMPARE(
        resolvedIcon.iconColor,
        legacyIcon.iconColor);
    QCOMPARE(
        resolvedIcon.cornerRadius,
        legacyIcon.cornerRadius);
}

void tst_ActionButtonSpecResolver::resolvesComponentOverrides()
{
    ThemeBuilder builder;
    Theme theme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));

    ComponentTokenOverride tokens;
    tokens.shapes.insert(ShapeRole::Small, 9);
    tokens.custom.insert(
        QStringLiteral("cornerRadius"),
        7.0);
    tokens.custom.insert(
        QStringLiteral("pressStateLayerOpacity"),
        0.19);
    tokens.custom.insert(
        QStringLiteral("containerDiameter"),
        60);

    theme.componentOverrides().setOverride(
        QStringLiteral("fab"),
        tokens);

    const FabSpec spec =
        ActionButtonSpecResolver().fabSpec(theme);

    QCOMPARE(spec.cornerRadius, 7.0);
    QCOMPARE(spec.pressStateLayerOpacity, 0.19);
    QCOMPARE(spec.containerDiameter, 60);
}

QTEST_MAIN(tst_ActionButtonSpecResolver)

#include "tst_actionbuttonspecresolver.moc"
