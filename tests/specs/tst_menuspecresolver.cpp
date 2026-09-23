#include <QtTest>

#include "qtmaterial/specs/qtmaterialmenuspecresolver.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_MenuSpecResolver : public QObject
{
    Q_OBJECT

private slots:
    void resolvesThemeRoles();
    void resolvesDensity();
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

QTEST_MAIN(tst_MenuSpecResolver)
#include "tst_menuspecresolver.moc"
