#include <QtTest>

#include "qtmaterial/specs/qtmaterialcompactspecresolver.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/specs/qtmaterialchipspecresolver.h"

using namespace QtMaterial;

class tst_CompactSpecResolver : public QObject
{
    Q_OBJECT

private slots:
    void resolvesVariants();
    void resolvesDensity();
    void variantOverridesDoNotLeak();
    void remainsCompatibleWithSpecFactory();
};

void tst_CompactSpecResolver::resolvesVariants()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));

    CompactSpecResolver resolver;

    const ChipSpec assist =
        resolver.assistChipSpec(theme);
    const ChipSpec filter =
        resolver.filterChipSpec(theme);
    const ChipSpec input =
        resolver.inputChipSpec(theme);
    const ChipSpec suggestion =
        resolver.suggestionChipSpec(theme);

    QCOMPARE(
        assist.variant,
        ChipVariant::Assist);
    QCOMPARE(
        filter.variant,
        ChipVariant::Filter);
    QCOMPARE(
        input.variant,
        ChipVariant::Input);
    QCOMPARE(
        suggestion.variant,
        ChipVariant::Suggestion);

    QCOMPARE(
        assist.containerColor,
        theme.colorScheme().color(
            ColorRole::SurfaceContainerLow));
    QCOMPARE(
        filter.containerColor,
        QColor(Qt::transparent));
    QCOMPARE(
        suggestion.selectedContainerColor,
        theme.colorScheme().color(
            ColorRole::PrimaryContainer));

    QVERIFY(assist.hasResolvedLabelFont);
    QVERIFY(assist.cornerRadius >= 0.0);
    QVERIFY(assist.focusRingWidth >= 0.0);
}

void tst_CompactSpecResolver::resolvesDensity()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));

    CompactSpecResolver resolver;

    QCOMPARE(
        resolver.assistChipSpec(
            theme,
            Density::Compact)
            .containerHeight,
        28);

    QCOMPARE(
        resolver.assistChipSpec(
            theme,
            Density::Default)
            .containerHeight,
        32);

    QCOMPARE(
        resolver.assistChipSpec(
            theme,
            Density::Comfortable)
            .containerHeight,
        36);
}

void tst_CompactSpecResolver::variantOverridesDoNotLeak()
{
    ThemeBuilder builder;
    Theme theme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));

    const QColor filterOnly(
        QStringLiteral("#B3261E"));

    ComponentTokenOverride tokens;
    tokens.custom.insert(
        QStringLiteral("containerColor"),
        filterOnly);

    theme.componentOverrides().setOverride(
        QStringLiteral("chip.filter"),
        tokens);

    CompactSpecResolver resolver;

    const ChipSpec assist =
        resolver.assistChipSpec(theme);
    const ChipSpec filter =
        resolver.filterChipSpec(theme);

    QCOMPARE(
        filter.containerColor,
        filterOnly);

    QCOMPARE(
        assist.containerColor,
        theme.colorScheme().color(
            ColorRole::SurfaceContainerLow));

    QVERIFY(
        assist.containerColor
        != filterOnly);
}

void tst_CompactSpecResolver::remainsCompatibleWithSpecFactory()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildDarkFromSeed(
            QColor(QStringLiteral("#6750A4")));

    CompactSpecResolver resolver;

    const ChipSpec resolved =
        resolver.inputChipSpec(
            theme,
            Density::Compact);

    const ChipSpec legacy =
        ChipSpecResolver().inputChipSpec(
            theme,
            Density::Compact);

    QCOMPARE(
        resolved.containerColor,
        legacy.containerColor);
    QCOMPARE(
        resolved.labelColor,
        legacy.labelColor);
    QCOMPARE(
        resolved.containerHeight,
        legacy.containerHeight);
    QCOMPARE(
        resolved.cornerRadius,
        legacy.cornerRadius);
}

QTEST_MAIN(tst_CompactSpecResolver)
#include "tst_compactspecresolver.moc"
