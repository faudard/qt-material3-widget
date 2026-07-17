#include <QtTest/QtTest>

#include "qtmaterial/specs/qtmaterialtabsspecresolver.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_TabsSpecResolver : public QObject
{
    Q_OBJECT

private slots:
    void resolvesThemeDefaults();
    void preservesExplicitValues();
    void resolvesVariantLabelColor();
    void preservesAuthoredSpecWhenGlobalThemeDisabled();
    void returnsIndependentValues();
};

void tst_TabsSpecResolver::resolvesThemeDefaults()
{
    ThemeBuilder builder;
    Theme theme =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));

    theme.stateLayer().hoverOpacity = 0.07;
    theme.stateLayer().focusOpacity = 0.11;
    theme.stateLayer().pressOpacity = 0.15;

    const TabsSpec resolved = TabsSpecResolver().resolve(theme);

    QCOMPARE(
        resolved.containerColor,
        theme.colorScheme().color(ColorRole::Surface));
    QCOMPARE(
        resolved.activeIndicatorColor,
        theme.colorScheme().color(ColorRole::Primary));
    QCOMPARE(
        resolved.inactiveLabelColor,
        theme.colorScheme().color(ColorRole::OnSurfaceVariant));
    QCOMPARE(
        resolved.badgeColor,
        theme.colorScheme().color(ColorRole::Error));
    QCOMPARE(resolved.hoverOpacity, 0.07);
    QCOMPARE(resolved.focusOpacity, 0.11);
    QCOMPARE(resolved.pressedOpacity, 0.15);
    QCOMPARE(
        resolved.hoverStateLayerColor,
        resolved.stateLayerColor);
}

void tst_TabsSpecResolver::preservesExplicitValues()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildDarkFromSeed(QColor(QStringLiteral("#146C2E")));

    TabsSpec authored;
    authored.containerColor = QColor(QStringLiteral("#102030"));
    authored.activeLabelColor = QColor(QStringLiteral("#203040"));
    authored.activeIndicatorColor = QColor(QStringLiteral("#304050"));
    authored.hoverOpacity = 0.21;
    authored.focusOpacity = 0.22;
    authored.pressedOpacity = 0.23;
    authored.minimumTabWidth = 81;
    authored.animationDuration = 260;

    const TabsSpec resolved =
        TabsSpecResolver().resolve(theme, authored);

    QCOMPARE(resolved.containerColor, authored.containerColor);
    QCOMPARE(resolved.activeLabelColor, authored.activeLabelColor);
    QCOMPARE(
        resolved.activeIndicatorColor,
        authored.activeIndicatorColor);
    QCOMPARE(resolved.hoverOpacity, 0.21);
    QCOMPARE(resolved.focusOpacity, 0.22);
    QCOMPARE(resolved.pressedOpacity, 0.23);
    QCOMPARE(resolved.minimumTabWidth, 81);
    QCOMPARE(resolved.animationDuration, 260);
}

void tst_TabsSpecResolver::resolvesVariantLabelColor()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));

    TabsSpec primary;
    primary.variant = TabsVariant::Primary;

    TabsSpec secondary;
    secondary.variant = TabsVariant::Secondary;

    const TabsSpec resolvedPrimary =
        TabsSpecResolver().resolve(theme, primary);
    const TabsSpec resolvedSecondary =
        TabsSpecResolver().resolve(theme, secondary);

    QCOMPARE(
        resolvedPrimary.activeLabelColor,
        theme.colorScheme().color(ColorRole::Primary));
    QCOMPARE(
        resolvedSecondary.activeLabelColor,
        theme.colorScheme().color(ColorRole::OnSurface));
}

void tst_TabsSpecResolver::preservesAuthoredSpecWhenGlobalThemeDisabled()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));

    TabsSpec authored;
    authored.useGlobalTheme = false;
    authored.hoverOpacity = -1.0;

    const TabsSpec resolved =
        TabsSpecResolver().resolve(theme, authored);

    QVERIFY(!resolved.containerColor.isValid());
    QVERIFY(!resolved.activeIndicatorColor.isValid());
    QCOMPARE(resolved.hoverOpacity, -1.0);
}

void tst_TabsSpecResolver::returnsIndependentValues()
{
    ThemeBuilder builder;
    const Theme firstTheme =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));
    const Theme secondTheme =
        builder.buildDarkFromSeed(QColor(QStringLiteral("#146C2E")));

    TabsSpecResolver resolver;
    TabsSpec first = resolver.resolve(firstTheme);
    const QColor firstIndicator = first.activeIndicatorColor;
    const TabsSpec second = resolver.resolve(secondTheme);

    QCOMPARE(first.activeIndicatorColor, firstIndicator);
    QVERIFY(first.activeIndicatorColor != second.activeIndicatorColor);
}

QTEST_MAIN(tst_TabsSpecResolver)

#include "tst_tabsspecresolver.moc"
