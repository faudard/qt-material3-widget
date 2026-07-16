#include <QtTest/QtTest>

#include "qtmaterial/specs/qtmaterialsurfacespecresolver.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_SurfaceResolvedSpec : public QObject
{
    Q_OBJECT

private slots:
    void resolvesCardRuntimeValues();
    void resolvesBannerRuntimeValues();
    void resolvesSnackbarRuntimeValues();
    void cardComponentOverridesWin();
};

void tst_SurfaceResolvedSpec::resolvesCardRuntimeValues()
{
    ThemeBuilder builder;
    Theme theme =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));

    TypographyStyle title;
    title.font = QFont(QStringLiteral("Sans Serif"), 14);
    TypographyStyle body;
    body.font = QFont(QStringLiteral("Sans Serif"), 11);
    theme.typography().setStyle(TypeRole::TitleMedium, title);
    theme.typography().setStyle(TypeRole::BodyMedium, body);

    const CardSpec spec = SurfaceSpecResolver().cardSpec(theme);

    QVERIFY(spec.hasResolvedTitleFont);
    QVERIFY(spec.hasResolvedBodyFont);
    QCOMPARE(spec.titleFont, title.font);
    QCOMPARE(spec.bodyFont, body.font);
    QVERIFY(spec.hasResolvedElevationStyle);
    QCOMPARE(
        spec.elevationStyle.shadowBlur,
        theme.elevations().style(ElevationRole::Level1).shadowBlur);
    QCOMPARE(
        spec.cornerRadius,
        qreal(theme.shapes().radius(ShapeRole::Large)));
    QCOMPARE(
        spec.shadowColor,
        theme.colorScheme().color(ColorRole::Shadow));
}

void tst_SurfaceResolvedSpec::resolvesBannerRuntimeValues()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildDarkFromSeed(QColor(QStringLiteral("#146C2E")));

    const BannerSpec spec =
        SurfaceSpecResolver().bannerSpec(theme);

    QVERIFY(spec.hasResolvedHeadlineFont);
    QVERIFY(spec.hasResolvedSupportingFont);
    QVERIFY(spec.hasResolvedActionFont);
    QCOMPARE(
        spec.cornerRadius,
        qreal(theme.shapes().radius(ShapeRole::Medium)));
    QCOMPARE(
        spec.focusRingColor,
        theme.colorScheme().color(ColorRole::Primary));
}

void tst_SurfaceResolvedSpec::resolvesSnackbarRuntimeValues()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));

    const SnackbarSpec spec =
        SurfaceSpecResolver().snackbarSpec(theme);

    QVERIFY(spec.hasResolvedTextFont);
    QVERIFY(spec.hasResolvedActionFont);
    QVERIFY(spec.hasResolvedEnterMotion);
    QVERIFY(spec.hasResolvedExitMotion);
    QVERIFY(spec.hasResolvedElevationStyle);
    QCOMPARE(
        spec.cornerRadius,
        qreal(theme.shapes().radius(ShapeRole::Small)));
    QCOMPARE(
        spec.elevationStyle.shadowBlur,
        theme.elevations().style(ElevationRole::Level3).shadowBlur);
    QCOMPARE(
        spec.enterMotionStyle.durationMs,
        theme.motion().style(MotionToken::Medium2).durationMs);
    QCOMPARE(
        spec.exitMotionStyle.durationMs,
        theme.motion().style(MotionToken::Short4).durationMs);
}

void tst_SurfaceResolvedSpec::cardComponentOverridesWin()
{
    ThemeBuilder builder;
    Theme theme =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));

    TypographyStyle title;
    title.font = QFont(QStringLiteral("Monospace"), 15);

    ComponentTokenOverride tokens;
    tokens.typography.insert(TypeRole::TitleMedium, title);
    tokens.shapes.insert(ShapeRole::Small, 9);
    tokens.elevations.insert(
        ElevationRole::Level2,
        ElevationStyle{10, 4, 0.08});
    tokens.custom.insert(
        QStringLiteral("shapeRole"),
        QStringLiteral("small"));
    tokens.custom.insert(
        QStringLiteral("elevationRole"),
        QStringLiteral("level2"));
    tokens.custom.insert(
        QStringLiteral("pressStateLayerOpacity"),
        0.17);

    theme.componentOverrides().setOverride(
        QStringLiteral("card"),
        tokens);

    const CardSpec spec = SurfaceSpecResolver().cardSpec(theme);

    QCOMPARE(spec.titleFont, title.font);
    QCOMPARE(spec.cornerRadius, 9.0);
    QCOMPARE(spec.elevationStyle.shadowBlur, 10);
    QCOMPARE(spec.elevationStyle.shadowYOffset, 4);
    QCOMPARE(spec.pressStateLayerOpacity, 0.17);
}

QTEST_MAIN(tst_SurfaceResolvedSpec)

#include "tst_surfaceresolvedspec.moc"
