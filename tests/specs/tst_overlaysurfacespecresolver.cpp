#include <QtTest/QtTest>

#include "qtmaterial/specs/qtmaterialoverlaysurfacespecresolver.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_OverlaySurfaceSpecResolver : public QObject
{
    Q_OBJECT

private slots:
    void resolvesNavigationDrawer();
    void resolvesBottomSheet();
    void remainsCompatibleWithSpecFactory();
    void returnsIndependentValues();
};

void tst_OverlaySurfaceSpecResolver::resolvesNavigationDrawer()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));

    const NavigationDrawerSpec spec =
        OverlaySurfaceSpecResolver()
            .navigationDrawerSpec(theme);

    QCOMPARE(
        spec.containerColor,
        theme.colorScheme().color(
            ColorRole::SurfaceContainerLow));
    QCOMPARE(
        spec.contentColor,
        theme.colorScheme().color(
            ColorRole::OnSurface));
    QCOMPARE(
        spec.cornerRadius,
        qreal(theme.shapes().radius(ShapeRole::Large)));
    QVERIFY(spec.hasResolvedElevationStyle);
    QVERIFY(spec.hasResolvedMotionStyle);
    QCOMPARE(
        spec.elevationStyle.shadowBlur,
        theme.elevations()
            .style(ElevationRole::Level1)
            .shadowBlur);
    QCOMPARE(
        spec.motionStyle.durationMs,
        theme.motion()
            .style(MotionToken::Medium2)
            .durationMs);
    QVERIFY(spec.scrimColor.alphaF() < 1.0);
}

void tst_OverlaySurfaceSpecResolver::resolvesBottomSheet()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildDarkFromSeed(
            QColor(QStringLiteral("#146C2E")));

    const BottomSheetSpec spec =
        OverlaySurfaceSpecResolver()
            .bottomSheetSpec(theme);

    QCOMPARE(
        spec.containerColor,
        theme.colorScheme().color(
            ColorRole::SurfaceContainerHigh));
    QCOMPARE(
        spec.cornerRadius,
        qreal(
            theme.shapes().radius(
                ShapeRole::ExtraLarge)));
    QVERIFY(spec.hasResolvedElevationStyle);
    QVERIFY(spec.hasResolvedMotionStyle);
    QCOMPARE(
        spec.elevationStyle.shadowBlur,
        theme.elevations()
            .style(ElevationRole::Level3)
            .shadowBlur);
    QCOMPARE(
        spec.motionStyle.durationMs,
        theme.motion()
            .style(MotionToken::Medium2)
            .durationMs);
    QCOMPARE(
        spec.shadowColor,
        theme.colorScheme().color(ColorRole::Shadow));
}

void tst_OverlaySurfaceSpecResolver::remainsCompatibleWithSpecFactory()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));

    OverlaySurfaceSpecResolver resolver;

    const NavigationDrawerSpec resolvedDrawer =
        resolver.navigationDrawerSpec(theme);
    const NavigationDrawerSpec legacyDrawer =
        OverlaySurfaceSpecResolver().navigationDrawerSpec(theme);

    QCOMPARE(
        resolvedDrawer.containerColor,
        legacyDrawer.containerColor);
    QCOMPARE(
        resolvedDrawer.cornerRadius,
        legacyDrawer.cornerRadius);
    QCOMPARE(
        resolvedDrawer.preferredWidth,
        legacyDrawer.preferredWidth);

    const BottomSheetSpec resolvedSheet =
        resolver.bottomSheetSpec(theme);
    const BottomSheetSpec legacySheet =
        OverlaySurfaceSpecResolver().bottomSheetSpec(theme);

    QCOMPARE(
        resolvedSheet.containerColor,
        legacySheet.containerColor);
    QCOMPARE(
        resolvedSheet.cornerRadius,
        legacySheet.cornerRadius);
    QCOMPARE(
        resolvedSheet.topPadding,
        legacySheet.topPadding);
}

void tst_OverlaySurfaceSpecResolver::returnsIndependentValues()
{
    ThemeBuilder builder;
    const Theme firstTheme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));
    const Theme secondTheme =
        builder.buildDarkFromSeed(
            QColor(QStringLiteral("#146C2E")));

    OverlaySurfaceSpecResolver resolver;
    BottomSheetSpec first =
        resolver.bottomSheetSpec(firstTheme);
    const QColor firstColor = first.containerColor;
    const BottomSheetSpec second =
        resolver.bottomSheetSpec(secondTheme);

    QCOMPARE(first.containerColor, firstColor);
    QVERIFY(first.containerColor != second.containerColor);
}

QTEST_MAIN(tst_OverlaySurfaceSpecResolver)

#include "tst_overlaysurfacespecresolver.moc"
