#include <QtTest/QtTest>

#include "qtmaterial/specs/qtmaterialnavigationrailspecresolver.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_NavigationRailSpecResolver
    : public QObject
{
    Q_OBJECT

private slots:
    void resolvesRuntimeValues();
    void resolvesOverrides();
    void remainsCompatibleWithFactory();
};

void tst_NavigationRailSpecResolver::
resolvesRuntimeValues()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));

    const NavigationRailSpec spec =
        NavigationRailSpecResolver()
            .navigationRailSpec(theme);

    QCOMPARE(
        spec.indicatorColor,
        theme.colorScheme().color(
            ColorRole::SecondaryContainer));
    QCOMPARE(spec.indicatorRadius, -1.0);
    QVERIFY(spec.hasResolvedLabelFont);
    QVERIFY(spec.hasResolvedMotionStyle);
    QCOMPARE(
        spec.pressStateLayerOpacity,
        theme.stateLayer().pressOpacity);
}

void tst_NavigationRailSpecResolver::
resolvesOverrides()
{
    ThemeBuilder builder;
    Theme theme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#146C2E")));

    ComponentTokenOverride tokens;
    tokens.custom.insert(
        QStringLiteral("railWidth"),
        96);
    tokens.custom.insert(
        QStringLiteral("indicatorRadius"),
        14.0);
    tokens.custom.insert(
        QStringLiteral("pressStateLayerOpacity"),
        0.18);

    theme.componentOverrides().setOverride(
        QStringLiteral("navigationRail"),
        tokens);

    const NavigationRailSpec spec =
        NavigationRailSpecResolver()
            .navigationRailSpec(theme);

    QCOMPARE(spec.railWidth, 96);
    QCOMPARE(spec.indicatorRadius, 14.0);
    QCOMPARE(spec.pressStateLayerOpacity, 0.18);
}

void tst_NavigationRailSpecResolver::
remainsCompatibleWithFactory()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));

    const NavigationRailSpec resolved =
        NavigationRailSpecResolver()
            .navigationRailSpec(theme);
    const NavigationRailSpec legacy =
        NavigationRailSpecResolver().navigationRailSpec(theme);

    QCOMPARE(
        resolved.containerColor,
        legacy.containerColor);
    QCOMPARE(
        resolved.indicatorRadius,
        legacy.indicatorRadius);
    QCOMPARE(
        resolved.labelFont,
        legacy.labelFont);
}

QTEST_MAIN(tst_NavigationRailSpecResolver)

#include "tst_navigationrailspecresolver.moc"
