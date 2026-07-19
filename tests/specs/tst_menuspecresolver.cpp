#include <QtTest>

#include "qtmaterial/specs/qtmaterialmenuspecresolver.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_MenuSpecResolver : public QObject
{
    Q_OBJECT

private slots:
    void resolvesThemeRoles();
    void resolvesDensity();
    void remainsCompatibleWithSpecFactory();
};

void tst_MenuSpecResolver::resolvesThemeRoles()
{
    const Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    const MenuSpec spec = MenuSpecResolver().menuSpec(theme);

    QCOMPARE(
        spec.containerColor,
        theme.colorScheme().color(ColorRole::SurfaceContainer));
    QCOMPARE(
        spec.itemLabelColor,
        theme.colorScheme().color(ColorRole::OnSurface));
    QCOMPARE(
        spec.dividerColor,
        theme.colorScheme().color(ColorRole::OutlineVariant));
}

void tst_MenuSpecResolver::resolvesDensity()
{
    const Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    const MenuSpecResolver resolver;

    QCOMPARE(
        resolver.menuSpec(theme, Density::Compact).minItemSize.height(),
        40);
    QCOMPARE(
        resolver.menuSpec(theme, Density::Default).minItemSize.height(),
        48);
    QCOMPARE(
        resolver.menuSpec(theme, Density::Comfortable).minItemSize.height(),
        52);
}

void tst_MenuSpecResolver::remainsCompatibleWithSpecFactory()
{
    const Theme theme = ThemeBuilder().buildDarkFromSeed(
        QColor(QStringLiteral("#6750A4")));
    const MenuSpec resolved =
        MenuSpecResolver().menuSpec(theme, Density::Compact);
    const MenuSpec legacy =
        SpecFactory().menuSpec(theme, Density::Compact);

    QCOMPARE(resolved.containerColor, legacy.containerColor);
    QCOMPARE(resolved.itemLabelColor, legacy.itemLabelColor);
    QCOMPARE(resolved.focusRingColor, legacy.focusRingColor);
    QCOMPARE(resolved.minItemSize, legacy.minItemSize);
}

QTEST_MAIN(tst_MenuSpecResolver)
#include "tst_menuspecresolver.moc"
