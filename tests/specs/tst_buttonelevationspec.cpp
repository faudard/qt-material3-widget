#include <QtTest/QtTest>

#include "qtmaterial/specs/qtmaterialbuttonspecresolver.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_ButtonElevationSpec : public QObject
{
    Q_OBJECT

private slots:
    void filledButtonUsesTransientElevation();
    void elevatedButtonUsesPersistentElevation();
    void tonalUsesHoverElevationAndOutlinedRemainsFlat();
};

void tst_ButtonElevationSpec::filledButtonUsesTransientElevation()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));

    const ButtonSpec spec =
        ButtonSpecResolver().filledButtonSpec(theme);

    QVERIFY(spec.hasResolvedElevationStyle);
    QVERIFY(spec.hasResolvedHoverElevationStyle);
    QCOMPARE(spec.elevationRole, ElevationRole::Level0);
    QCOMPARE(spec.hoverElevationRole, ElevationRole::Level1);
    QCOMPARE(
        spec.elevationStyle.shadowBlur,
        theme.elevations().style(ElevationRole::Level0).shadowBlur);
    QCOMPARE(
        spec.hoverElevationStyle.shadowBlur,
        theme.elevations().style(ElevationRole::Level1).shadowBlur);
    QCOMPARE(spec.restingElevationProgress, 0.0);
    QCOMPARE(spec.hoverElevationProgress, 1.0);
    QCOMPARE(spec.focusElevationProgress, 0.0);
    QCOMPARE(spec.pressElevationProgress, 0.0);
    QCOMPARE(spec.disabledElevationProgress, 0.0);
}

void tst_ButtonElevationSpec::elevatedButtonUsesPersistentElevation()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildDarkFromSeed(QColor(QStringLiteral("#6750A4")));

    const ButtonSpec spec =
        ButtonSpecResolver().elevatedButtonSpec(theme);

    QVERIFY(spec.hasResolvedElevationStyle);
    QVERIFY(spec.hasResolvedHoverElevationStyle);
    QCOMPARE(spec.elevationRole, ElevationRole::Level1);
    QCOMPARE(spec.hoverElevationRole, ElevationRole::Level2);
    QCOMPARE(
        spec.elevationStyle.shadowBlur,
        theme.elevations().style(ElevationRole::Level1).shadowBlur);
    QCOMPARE(
        spec.hoverElevationStyle.shadowBlur,
        theme.elevations().style(ElevationRole::Level2).shadowBlur);
    QCOMPARE(spec.restingElevationProgress, 0.0);
    QCOMPARE(spec.hoverElevationProgress, 1.0);
    QCOMPARE(spec.focusElevationProgress, 0.0);
    QCOMPARE(spec.pressElevationProgress, 0.0);
    QCOMPARE(spec.disabledElevationProgress, 0.0);
}

void tst_ButtonElevationSpec::tonalUsesHoverElevationAndOutlinedRemainsFlat()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));
    ButtonSpecResolver resolver;

    const ButtonSpec tonal = resolver.filledTonalButtonSpec(theme);
    const ButtonSpec outlined = resolver.outlinedButtonSpec(theme);

    QVERIFY(tonal.hasResolvedElevationStyle);
    QVERIFY(tonal.hasResolvedHoverElevationStyle);
    QCOMPARE(tonal.elevationRole, ElevationRole::Level0);
    QCOMPARE(tonal.hoverElevationRole, ElevationRole::Level1);
    QCOMPARE(
        tonal.elevationStyle.shadowBlur,
        theme.elevations().style(ElevationRole::Level0).shadowBlur);
    QCOMPARE(
        tonal.hoverElevationStyle.shadowBlur,
        theme.elevations().style(ElevationRole::Level1).shadowBlur);
    QCOMPARE(tonal.restingElevationProgress, 0.0);
    QCOMPARE(tonal.hoverElevationProgress, 1.0);
    QCOMPARE(tonal.focusElevationProgress, 0.0);
    QCOMPARE(tonal.pressElevationProgress, 0.0);
    QCOMPARE(tonal.disabledElevationProgress, 0.0);
    QVERIFY(outlined.hasResolvedElevationStyle);
    QVERIFY(outlined.hasResolvedHoverElevationStyle);
    QCOMPARE(outlined.elevationRole, ElevationRole::Level0);
    QCOMPARE(outlined.hoverElevationRole, ElevationRole::Level0);
    QCOMPARE(outlined.elevationStyle.shadowBlur, 0);
    QCOMPARE(outlined.hoverElevationStyle.shadowBlur, 0);
    QCOMPARE(outlined.restingElevationProgress, 0.0);
    QCOMPARE(outlined.hoverElevationProgress, 0.0);
    QCOMPARE(outlined.focusElevationProgress, 0.0);
    QCOMPARE(outlined.pressElevationProgress, 0.0);
    QCOMPARE(outlined.disabledElevationProgress, 0.0);
}

QTEST_MAIN(tst_ButtonElevationSpec)

#include "tst_buttonelevationspec.moc"
