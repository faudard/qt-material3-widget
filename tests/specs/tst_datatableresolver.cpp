#include <QtTest>

#include "qtmaterial/specs/qtmaterialdataspecresolver.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_DataTableResolver
    : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void resolvesThemeTokens();
    void densityAffectsMetrics();
    void appliesComponentOverrides();
};

void tst_DataTableResolver::
resolvesThemeTokens()
{
    const Theme theme =
        ThemeBuilder().buildLightFromSeed(
            QColor(
                QStringLiteral("#6750A4")));

    const TableSpec spec =
        DataSpecResolver().tableSpec(theme);

    QCOMPARE(
        spec.backgroundColor,
        theme.colorScheme().color(
            ColorRole::Surface));
    QCOMPARE(
        spec.foregroundColor,
        theme.colorScheme().color(
            ColorRole::OnSurface));
    QCOMPARE(
        spec.headerBackgroundColor,
        theme.colorScheme().color(
            ColorRole::SurfaceContainer));
    QCOMPARE(
        spec.rowSelectedColor,
        theme.colorScheme().color(
            ColorRole::SecondaryContainer));
    QCOMPARE(
        spec.focusRingColor,
        theme.colorScheme().color(
            ColorRole::Primary));

    QVERIFY(spec.headerFont.family().isEmpty() == false);
    QVERIFY(spec.bodyFont.family().isEmpty() == false);
    QVERIFY(spec.cornerRadius >= 0.0);
}

void tst_DataTableResolver::
densityAffectsMetrics()
{
    const Theme theme =
        ThemeBuilder().buildLightFromSeed(
            QColor(
                QStringLiteral("#006874")));

    const DataSpecResolver resolver;

    const TableSpec compact =
        resolver.tableSpec(
            theme,
            Density::Compact);
    const TableSpec comfortable =
        resolver.tableSpec(
            theme,
            Density::Comfortable);

    QVERIFY(
        compact.rowHeight
        < comfortable.rowHeight);
    QVERIFY(
        compact.headerHeight
        < comfortable.headerHeight);
    QVERIFY(
        compact.cellHorizontalPadding
        < comfortable.cellHorizontalPadding);
}

void tst_DataTableResolver::
appliesComponentOverrides()
{
    Theme theme =
        ThemeBuilder().buildLightFromSeed(
            QColor(
                QStringLiteral("#6750A4")));

    ComponentTokenOverride tokens;
    tokens.custom.insert(
        QStringLiteral("rowHeight"),
        60);
    tokens.custom.insert(
        QStringLiteral("denseRowHeight"),
        40);
    tokens.custom.insert(
        QStringLiteral("headerHeight"),
        68);
    tokens.custom.insert(
        QStringLiteral("cornerRadius"),
        22.0);
    tokens.custom.insert(
        QStringLiteral("rowSelectedColor"),
        QColor(Qt::red));

    theme.componentOverrides().setOverride(
        QStringLiteral("table"),
        tokens);

    const TableSpec spec =
        DataSpecResolver().tableSpec(theme);

    QCOMPARE(spec.rowHeight, 60);
    QCOMPARE(spec.denseRowHeight, 40);
    QCOMPARE(spec.headerHeight, 68);
    QCOMPARE(spec.cornerRadius, 22.0);
    QCOMPARE(
        spec.rowSelectedColor,
        QColor(Qt::red));
}

QTEST_MAIN(tst_DataTableResolver)

#include "tst_datatableresolver.moc"
