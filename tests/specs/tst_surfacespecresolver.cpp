#include <QtTest/QtTest>

#include "qtmaterial/specs/qtmaterialsurfacespecresolver.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_SurfaceSpecResolver : public QObject
{
    Q_OBJECT

private slots:
    void resolvesSurfaceColors();
    void returnsIndependentValues();
};

void tst_SurfaceSpecResolver::resolvesSurfaceColors()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));
    SurfaceSpecResolver resolver;

    const BannerSpec banner = resolver.bannerSpec(theme);
    const CardSpec card = resolver.cardSpec(theme);
    const SnackbarSpec snackbar = resolver.snackbarSpec(theme);

    QCOMPARE(
        banner.containerColor,
        theme.colorScheme().color(ColorRole::SurfaceContainerLow));
    QCOMPARE(
        card.outlineColor,
        theme.colorScheme().color(ColorRole::OutlineVariant));
    QCOMPARE(
        snackbar.containerColor,
        theme.colorScheme().color(ColorRole::InverseSurface));
    QCOMPARE(snackbar.shapeRole, ShapeRole::Small);
    QCOMPARE(snackbar.elevationRole, ElevationRole::Level3);
}

void tst_SurfaceSpecResolver::returnsIndependentValues()
{
    ThemeBuilder builder;
    const Theme firstTheme =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));
    const Theme secondTheme =
        builder.buildDarkFromSeed(QColor(QStringLiteral("#146C2E")));

    SurfaceSpecResolver resolver;
    SnackbarSpec first = resolver.snackbarSpec(firstTheme);
    const QColor firstColor = first.containerColor;
    const SnackbarSpec second = resolver.snackbarSpec(secondTheme);

    QCOMPARE(first.containerColor, firstColor);
    QVERIFY(first.containerColor != second.containerColor);
}

QTEST_MAIN(tst_SurfaceSpecResolver)

#include "tst_surfacespecresolver.moc"
