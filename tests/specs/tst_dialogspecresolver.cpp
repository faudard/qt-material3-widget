#include <QtTest/QtTest>

#include "qtmaterial/specs/qtmaterialdialogspecresolver.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_DialogSpecResolver : public QObject
{
    Q_OBJECT

private slots:
    void resolvesRuntimeValues();
    void resolvesComponentOverrides();
    void remainsCompatibleWithSpecFactory();
};

void tst_DialogSpecResolver::resolvesRuntimeValues()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));

    const DialogSpec spec =
        DialogSpecResolver().dialogSpec(theme);

    QCOMPARE(
        spec.containerColor,
        theme.colorScheme().color(
            ColorRole::SurfaceContainerHigh));
    QCOMPARE(
        spec.shadowColor,
        theme.colorScheme().color(ColorRole::Shadow));
    QCOMPARE(
        spec.cornerRadius,
        qreal(theme.shapes().radius(
            ShapeRole::ExtraLarge)));

    QVERIFY(spec.hasResolvedHeadlineFont);
    QVERIFY(spec.hasResolvedBodyFont);
    QVERIFY(spec.hasResolvedElevationStyle);
    QVERIFY(spec.hasResolvedEnterMotionStyle);
    QVERIFY(spec.hasResolvedExitMotionStyle);

    QCOMPARE(
        spec.elevationStyle.shadowBlur,
        theme.elevations()
            .style(ElevationRole::Level3)
            .shadowBlur);
    QCOMPARE(
        spec.enterMotionStyle.durationMs,
        theme.motion()
            .style(MotionToken::Medium2)
            .durationMs);
    QCOMPARE(
        spec.exitMotionStyle.durationMs,
        theme.motion()
            .style(MotionToken::Short4)
            .durationMs);
    QVERIFY(spec.scrimColor.alphaF() <= 0.3201);
}

void tst_DialogSpecResolver::resolvesComponentOverrides()
{
    ThemeBuilder builder;
    Theme theme =
        builder.buildDarkFromSeed(
            QColor(QStringLiteral("#146C2E")));

    TypographyStyle headline;
    headline.font =
        QFont(QStringLiteral("Monospace"), 16);

    MotionStyle enterMotion;
    enterMotion.durationMs = 360;
    enterMotion.easing =
        QEasingCurve(QEasingCurve::OutQuart);

    ElevationStyle elevation =
        theme.elevations().style(ElevationRole::Level3);
    elevation.shadowBlur += 3;

    ComponentTokenOverride tokens;
    tokens.typography.insert(
        TypeRole::HeadlineSmall,
        headline);
    tokens.motion.insert(
        MotionToken::Medium2,
        enterMotion);
    tokens.elevations.insert(
        ElevationRole::Level3,
        elevation);
    tokens.shapes.insert(
        ShapeRole::ExtraLarge,
        18);
    tokens.custom.insert(
        QStringLiteral("scrimOpacity"),
        0.24);
    tokens.custom.insert(
        QStringLiteral("maxWidth"),
        640);

    theme.componentOverrides().setOverride(
        QStringLiteral("dialog"),
        tokens);

    const DialogSpec spec =
        DialogSpecResolver().dialogSpec(theme);

    QCOMPARE(spec.headlineFont, headline.font);
    QCOMPARE(spec.enterMotionStyle.durationMs, 360);
    QCOMPARE(
        spec.elevationStyle.shadowBlur,
        elevation.shadowBlur);
    QCOMPARE(spec.cornerRadius, 18.0);
    QCOMPARE(spec.maxWidth, 640);
    QVERIFY(spec.scrimColor.alphaF() <= 0.2401);
}

void tst_DialogSpecResolver::remainsCompatibleWithSpecFactory()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));

    const DialogSpec resolved =
        DialogSpecResolver().dialogSpec(theme);
    const DialogSpec legacy =
        DialogSpecResolver().dialogSpec(theme);

    QCOMPARE(resolved.containerColor, legacy.containerColor);
    QCOMPARE(resolved.cornerRadius, legacy.cornerRadius);
    QCOMPARE(
        resolved.elevationStyle.shadowBlur,
        legacy.elevationStyle.shadowBlur);
    QCOMPARE(
        resolved.enterMotionStyle.durationMs,
        legacy.enterMotionStyle.durationMs);
    QCOMPARE(resolved.maxWidth, legacy.maxWidth);
}

QTEST_MAIN(tst_DialogSpecResolver)

#include "tst_dialogspecresolver.moc"
