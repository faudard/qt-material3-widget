#include <QtTest>

#include "qtmaterial/specs/qtmaterialnavigationrailspecresolver.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_NavigationRailResolvedSpec : public QObject
{
    Q_OBJECT

private slots:
    void resolvesConcreteRuntimeValues();
    void resolvesDisabledColors();
    void componentOverridesWin();
};

void tst_NavigationRailResolvedSpec::
resolvesConcreteRuntimeValues()
{
    Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    theme.shapes().setRadius(ShapeRole::Full, 99);

    const NavigationRailSpec spec =
        NavigationRailSpecResolver().navigationRailSpec(theme);

    QVERIFY(spec.hasResolvedLabelFont);
    QVERIFY(spec.hasResolvedMotionStyle);
    QCOMPARE(
        spec.labelFont,
        theme.typography().style(
            TypeRole::LabelMedium).font);
    QCOMPARE(spec.indicatorRadius, -1.0);
    QCOMPARE(
        spec.pressStateLayerOpacity,
        theme.stateLayer().pressOpacity);
    QVERIFY(spec.focusRingWidth >= 1.0);
}

void tst_NavigationRailResolvedSpec::
resolvesDisabledColors()
{
    const Theme theme = ThemeBuilder().buildDarkFromSeed(
        QColor(QStringLiteral("#006874")));

    const NavigationRailSpec spec =
        NavigationRailSpecResolver().navigationRailSpec(theme);

    QVERIFY(spec.disabledIconColor.isValid());
    QVERIFY(spec.disabledLabelColor.isValid());
    QVERIFY(
        spec.disabledIconColor.alpha()
        < spec.unselectedIconColor.alpha());
    QVERIFY(
        spec.disabledLabelColor.alpha()
        < spec.unselectedLabelColor.alpha());
}

void tst_NavigationRailResolvedSpec::
componentOverridesWin()
{
    Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#146C2E")));

    ComponentTokenOverride tokens;
    tokens.custom.insert(
        QStringLiteral("railWidth"),
        96);
    tokens.custom.insert(
        QStringLiteral("indicatorRadius"),
        14.0);
    tokens.custom.insert(
        QStringLiteral("indicatorTopOffset"),
        7);
    tokens.custom.insert(
        QStringLiteral("showDivider"),
        true);
    tokens.custom.insert(
        QStringLiteral("focusRingWidth"),
        3.0);
    tokens.custom.insert(
        QStringLiteral("disabledLabelColor"),
        QStringLiteral("#123456"));
    tokens.hasStateLayer = true;
    tokens.stateLayer.pressOpacity = 0.19;

    theme.componentOverrides().setOverride(
        QStringLiteral("navigationRail"),
        tokens);

    const NavigationRailSpec spec =
        NavigationRailSpecResolver().navigationRailSpec(theme);

    QCOMPARE(spec.railWidth, 96);
    QCOMPARE(spec.indicatorRadius, 14.0);
    QCOMPARE(spec.indicatorTopOffset, 7);
    QVERIFY(spec.showDivider);
    QCOMPARE(spec.focusRingWidth, 3.0);
    QCOMPARE(spec.pressStateLayerOpacity, 0.19);
    QCOMPARE(
        spec.disabledLabelColor,
        QColor(QStringLiteral("#123456")));
}

QTEST_MAIN(tst_NavigationRailResolvedSpec)
#include "tst_navigationrailresolvedspec.moc"
