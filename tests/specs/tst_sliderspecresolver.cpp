#include <QtTest/QtTest>

#include "qtmaterial/specs/qtmaterialsliderspecresolver.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_SliderSpecResolver : public QObject
{
    Q_OBJECT

private slots:
    void resolvesMaterialColors();
    void resolvesDensityMetrics();
};

void tst_SliderSpecResolver::resolvesMaterialColors()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));

    const SliderSpec spec =
        SliderSpecResolver().sliderSpec(theme);

    QCOMPARE(
        spec.activeTrackColor,
        theme.colorScheme().color(
            ColorRole::Primary));
    QCOMPARE(
        spec.handleColor,
        theme.colorScheme().color(
            ColorRole::Primary));
    QCOMPARE(
        spec.inactiveTrackColor,
        theme.colorScheme().color(
            ColorRole::SurfaceContainerHighest));
    QCOMPARE(spec.trackThickness, 4);
    QCOMPARE(spec.handleDiameter, 20);
}

void tst_SliderSpecResolver::resolvesDensityMetrics()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));
    SliderSpecResolver resolver;

    const SliderSpec compact =
        resolver.sliderSpec(
            theme,
            Density::Compact);
    const SliderSpec comfortable =
        resolver.sliderSpec(
            theme,
            Density::Comfortable);
    const SliderSpec normal =
        resolver.sliderSpec(
            theme,
            Density::Default);

    QCOMPARE(compact.touchTarget, QSize(40, 40));
    QCOMPARE(compact.handleDiameter, 16);
    QCOMPARE(
        comfortable.touchTarget,
        QSize(44, 44));
    QCOMPARE(comfortable.handleDiameter, 18);
    QCOMPARE(normal.touchTarget, QSize(48, 48));
    QCOMPARE(normal.handleDiameter, 20);
}

QTEST_MAIN(tst_SliderSpecResolver)
#include "tst_sliderspecresolver.moc"
