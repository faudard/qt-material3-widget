#include <QtTest>

#include "qtmaterial/specs/qtmaterialsegmentedbuttonspecresolver.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_SegmentedButtonSpecResolver : public QObject
{
    Q_OBJECT

private slots:
    void resolvesThemeRoles();
    void resolvesDensity();
};

void tst_SegmentedButtonSpecResolver::resolvesThemeRoles()
{
    const Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    const SegmentedButtonSpec spec =
        SegmentedButtonSpecResolver().segmentedButtonSpec(theme);

    QCOMPARE(spec.containerColor, QColor(Qt::transparent));
    QCOMPARE(
        spec.selectedContainerColor,
        theme.colorScheme().color(ColorRole::SecondaryContainer));
    QCOMPARE(
        spec.selectedLabelColor,
        theme.colorScheme().color(ColorRole::OnSecondaryContainer));
    QVERIFY(spec.touchTarget.height() >= 48);
}

void tst_SegmentedButtonSpecResolver::resolvesDensity()
{
    const Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    const SegmentedButtonSpecResolver resolver;

    QCOMPARE(
        resolver.segmentedButtonSpec(theme, Density::Compact).segmentHeight,
        36);
    QCOMPARE(
        resolver.segmentedButtonSpec(theme, Density::Default).segmentHeight,
        40);
    QCOMPARE(
        resolver.segmentedButtonSpec(theme, Density::Comfortable).segmentHeight,
        44);
}

QTEST_MAIN(tst_SegmentedButtonSpecResolver)
#include "tst_segmentedbuttonspecresolver.moc"
