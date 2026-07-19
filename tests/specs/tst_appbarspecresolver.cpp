#include <QtTest/QtTest>

#include "qtmaterial/specs/qtmaterialappbarspecresolver.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_AppBarSpecResolver : public QObject
{
    Q_OBJECT

private slots:
    void resolvesTopAppBar();
    void resolvesBottomAppBar();
    void remainsCompatibleWithSpecFactory();
    void constructsVariantsIndependently();
    void returnsIndependentValues();
};

void tst_AppBarSpecResolver::resolvesTopAppBar()
{
    ThemeBuilder builder;
    const Theme theme = builder.buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    const AppBarSpec spec =
        AppBarSpecResolver().topAppBarSpec(theme);

    QCOMPARE(spec.containerColor,
        theme.colorScheme().color(ColorRole::Surface));
    QCOMPARE(spec.titleColor,
        theme.colorScheme().color(ColorRole::OnSurface));
    QCOMPARE(spec.preferredHeight, 64);
    QCOMPARE(spec.minimumWidth, 200);
    QCOMPARE(spec.actionSlot, 48);
    QVERIFY(spec.hasResolvedTitleFont);
    QVERIFY(spec.hasResolvedElevatedElevationStyle);
}

void tst_AppBarSpecResolver::resolvesBottomAppBar()
{
    ThemeBuilder builder;
    const Theme theme = builder.buildDarkFromSeed(
        QColor(QStringLiteral("#146C2E")));
    const AppBarSpec spec =
        AppBarSpecResolver().bottomAppBarSpec(theme);

    QCOMPARE(spec.preferredHeight, 80);
    QCOMPARE(spec.minimumWidth, 240);
    QCOMPARE(spec.fabSlot, 64);
    QCOMPARE(spec.elevationRole, ElevationRole::Level2);
    QCOMPARE(spec.titleTypeRole, TypeRole::TitleMedium);
    QVERIFY(spec.hasResolvedTitleFont);
    QVERIFY(spec.hasResolvedElevatedElevationStyle);
}

void tst_AppBarSpecResolver::remainsCompatibleWithSpecFactory()
{
    ThemeBuilder builder;
    const Theme theme = builder.buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    AppBarSpecResolver resolver;

    const AppBarSpec resolvedTop = resolver.topAppBarSpec(theme);
    const AppBarSpec legacyTop = AppBarSpecResolver().topAppBarSpec(theme);
    QCOMPARE(resolvedTop.containerColor, legacyTop.containerColor);
    QCOMPARE(resolvedTop.titleColor, legacyTop.titleColor);
    QCOMPARE(resolvedTop.preferredHeight, legacyTop.preferredHeight);

    const AppBarSpec resolvedBottom = resolver.bottomAppBarSpec(theme);
    const AppBarSpec legacyBottom = AppBarSpecResolver().bottomAppBarSpec(theme);
    QCOMPARE(resolvedBottom.containerColor, legacyBottom.containerColor);
    QCOMPARE(resolvedBottom.elevationRole, legacyBottom.elevationRole);
    QCOMPARE(resolvedBottom.preferredHeight, legacyBottom.preferredHeight);
}

void tst_AppBarSpecResolver::constructsVariantsIndependently()
{
    ThemeBuilder builder;
    const Theme theme = builder.buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    AppBarSpecResolver resolver;

    const AppBarSpec top = resolver.topAppBarSpec(theme);
    const AppBarSpec bottom = resolver.bottomAppBarSpec(theme);

    QCOMPARE(top.preferredHeight, 64);
    QCOMPARE(bottom.preferredHeight, 80);
    QCOMPARE(top.fabSlot, 0);
    QCOMPARE(bottom.fabSlot, 64);
    QCOMPARE(top.titleTypeRole, TypeRole::TitleLarge);
    QCOMPARE(bottom.titleTypeRole, TypeRole::TitleMedium);
}

void tst_AppBarSpecResolver::returnsIndependentValues()
{
    ThemeBuilder builder;
    const Theme firstTheme = builder.buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    const Theme secondTheme = builder.buildDarkFromSeed(
        QColor(QStringLiteral("#146C2E")));

    AppBarSpecResolver resolver;
    AppBarSpec first = resolver.topAppBarSpec(firstTheme);
    const QColor firstContainer = first.containerColor;
    const AppBarSpec second = resolver.topAppBarSpec(secondTheme);

    QCOMPARE(first.containerColor, firstContainer);
    QCOMPARE(second.containerColor,
        secondTheme.colorScheme().color(ColorRole::Surface));
}

QTEST_MAIN(tst_AppBarSpecResolver)

#include "tst_appbarspecresolver.moc"
