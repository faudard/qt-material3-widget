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
    void tonalAndOutlinedRemainFlat();
};

void tst_ButtonElevationSpec::filledButtonUsesTransientElevation()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));

    const ButtonSpec spec =
        ButtonSpecResolver().filledButtonSpec(theme);

    QVERIFY(spec.hasResolvedElevationStyle);
    QCOMPARE(
        spec.elevationStyle.shadowBlur,
        theme.elevations().style(ElevationRole::Level1).shadowBlur);
    QCOMPARE(spec.restingElevationProgress, 0.0);
    QCOMPARE(spec.hoverElevationProgress, 0.65);
    QCOMPARE(spec.focusElevationProgress, 0.65);
    QCOMPARE(spec.pressElevationProgress, 1.0);
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
    QVERIFY(spec.restingElevationProgress > 0.0);
    QCOMPARE(spec.restingElevationProgress, 0.45);
    QCOMPARE(spec.hoverElevationProgress, 1.0);
    QCOMPARE(spec.focusElevationProgress, 1.0);
    QCOMPARE(spec.pressElevationProgress, 0.65);
}

void tst_ButtonElevationSpec::tonalAndOutlinedRemainFlat()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));
    ButtonSpecResolver resolver;

    const ButtonSpec tonal = resolver.filledTonalButtonSpec(theme);
    const ButtonSpec outlined = resolver.outlinedButtonSpec(theme);

    QCOMPARE(tonal.elevationStyle.shadowBlur, 0);
    QCOMPARE(tonal.hoverElevationProgress, 0.0);
    QCOMPARE(outlined.elevationStyle.shadowBlur, 0);
    QCOMPARE(outlined.pressElevationProgress, 0.0);
}

QTEST_MAIN(tst_ButtonElevationSpec)

#include "tst_buttonelevationspec.moc"
