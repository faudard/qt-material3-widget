#include <QtTest>

#include "qtmaterial/specs/qtmaterialchipspecresolver.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_ChipSpecResolver : public QObject
{
    Q_OBJECT

private slots:
    void resolvesAllVariants();
    void resolvesDensity();
};

void tst_ChipSpecResolver::resolvesAllVariants()
{
    const Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    const ChipSpecResolver resolver;

    const ChipSpec assist = resolver.assistChipSpec(theme);
    const ChipSpec filter = resolver.filterChipSpec(theme);
    const ChipSpec input = resolver.inputChipSpec(theme);
    const ChipSpec suggestion = resolver.suggestionChipSpec(theme);

    QCOMPARE(static_cast<int>(assist.variant),
             static_cast<int>(ChipVariant::Assist));
    QCOMPARE(static_cast<int>(filter.variant),
             static_cast<int>(ChipVariant::Filter));
    QCOMPARE(static_cast<int>(input.variant),
             static_cast<int>(ChipVariant::Input));
    QCOMPARE(static_cast<int>(suggestion.variant),
             static_cast<int>(ChipVariant::Suggestion));
    QCOMPARE(filter.containerColor, QColor(Qt::transparent));
    QCOMPARE(input.containerColor, QColor(Qt::transparent));
    QCOMPARE(suggestion.containerColor, QColor(Qt::transparent));
    QCOMPARE(
        assist.labelColor,
        theme.colorScheme().color(ColorRole::OnSurface));
}

void tst_ChipSpecResolver::resolvesDensity()
{
    const Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    const ChipSpecResolver resolver;

    QCOMPARE(
        resolver.assistChipSpec(theme, Density::Compact).containerHeight,
        28);
    QCOMPARE(
        resolver.assistChipSpec(theme, Density::Default).containerHeight,
        32);
    QCOMPARE(
        resolver.assistChipSpec(theme, Density::Comfortable).containerHeight,
        36);
}

QTEST_MAIN(tst_ChipSpecResolver)
#include "tst_chipspecresolver.moc"
