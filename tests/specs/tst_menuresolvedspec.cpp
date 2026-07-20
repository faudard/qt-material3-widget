#include <QtTest>

#include "qtmaterial/specs/qtmaterialmenuspecresolver.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_MenuResolvedSpec : public QObject
{
    Q_OBJECT

private slots:
    void resolvesRuntimeValues();
    void densityChangesItemHeight();
    void componentOverridesWin();
};

void tst_MenuResolvedSpec::resolvesRuntimeValues()
{
    Theme theme = ThemeBuilder().buildDarkFromSeed(
        QColor(QStringLiteral("#6750A4")));
    theme.shapes().setRadius(ShapeRole::ExtraSmall, 11);

    const MenuSpec spec =
        MenuSpecResolver().menuSpec(theme);

    QCOMPARE(spec.cornerRadius, 11.0);
    QCOMPARE(
        spec.labelFont,
        theme.typography().style(TypeRole::LabelLarge).font);
    QCOMPARE(
        spec.shortcutFont,
        theme.typography().style(TypeRole::LabelMedium).font);
    QVERIFY(spec.elevationStyle.shadowBlur >= 0);
    QVERIFY(spec.enterMotionStyle.durationMs >= 0);
    QVERIFY(spec.disabledItemIconColor.alpha() < 255);
    QVERIFY(spec.hoverStateLayerOpacity >= 0.0);
}

void tst_MenuResolvedSpec::densityChangesItemHeight()
{
    const Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    const MenuSpecResolver resolver;

    QCOMPARE(
        resolver.menuSpec(theme, Density::Compact)
            .minItemSize.height(),
        40);
    QCOMPARE(
        resolver.menuSpec(theme, Density::Default)
            .minItemSize.height(),
        48);
    QCOMPARE(
        resolver.menuSpec(theme, Density::Comfortable)
            .minItemSize.height(),
        52);
}

void tst_MenuResolvedSpec::componentOverridesWin()
{
    Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#006874")));

    ComponentTokenOverride tokens;
    tokens.custom.insert(QStringLiteral("cornerRadius"), 17);
    tokens.custom.insert(QStringLiteral("minItemHeight"), 56);
    tokens.custom.insert(QStringLiteral("shortcutSpacing"), 24);
    tokens.custom.insert(
        QStringLiteral("containerColor"),
        QStringLiteral("#123456"));
    tokens.hasStateLayer = true;
    tokens.stateLayer.pressOpacity = 0.21;

    theme.componentOverrides().setOverride(
        QStringLiteral("menu"),
        tokens);

    const MenuSpec spec =
        MenuSpecResolver().menuSpec(theme);

    QCOMPARE(spec.cornerRadius, 17.0);
    QCOMPARE(spec.minItemSize.height(), 56);
    QCOMPARE(spec.shortcutSpacing, 24);
    QCOMPARE(
        spec.containerColor,
        QColor(QStringLiteral("#123456")));
    QCOMPARE(spec.pressStateLayerOpacity, 0.21);
}

QTEST_MAIN(tst_MenuResolvedSpec)
#include "tst_menuresolvedspec.moc"
