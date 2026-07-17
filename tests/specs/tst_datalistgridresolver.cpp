#include <QtTest>

#include "qtmaterial/specs/qtmaterialdataspecresolver.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_DataListGridResolver
    : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void resolvesListTokens();
    void resolvesGridListTokens();
    void appliesListOverrides();
    void appliesGridListOverrides();
    void densityAffectsMetrics();
};

void tst_DataListGridResolver::
resolvesListTokens()
{
    const Theme theme =
        ThemeBuilder().buildLightFromSeed(
            QColor(
                QStringLiteral("#6750A4")));

    const ListSpec spec =
        DataSpecResolver().listSpec(theme);

    QCOMPARE(
        spec.containerColor,
        theme.colorScheme().color(
            ColorRole::Surface));
    QCOMPARE(
        spec.dividerColor,
        theme.colorScheme().color(
            ColorRole::OutlineVariant));
    QCOMPARE(
        spec.focusRingColor,
        theme.colorScheme().color(
            ColorRole::Primary));

    QVERIFY(
        spec.minimumViewportSize.isValid());
    QVERIFY(spec.cornerRadius >= 0.0);
    QVERIFY(spec.dividerThickness > 0);
}

void tst_DataListGridResolver::
resolvesGridListTokens()
{
    const Theme theme =
        ThemeBuilder().buildLightFromSeed(
            QColor(
                QStringLiteral("#006874")));

    const GridListSpec spec =
        DataSpecResolver().gridListSpec(theme);

    QCOMPARE(
        spec.backgroundColor,
        theme.colorScheme().color(
            ColorRole::Surface));
    QCOMPARE(
        spec.foregroundColor,
        theme.colorScheme().color(
            ColorRole::OnSurface));
    QCOMPARE(
        spec.itemSelectedColor,
        theme.colorScheme().color(
            ColorRole::SecondaryContainer));

    QVERIFY(spec.itemSize.isValid());
    QVERIFY(spec.titleFont.family().isEmpty() == false);
    QVERIFY(spec.supportingFont.family().isEmpty() == false);
    QVERIFY(spec.columns > 0);
    QVERIFY(spec.minimumCellWidth >= 48);
}

void tst_DataListGridResolver::
appliesListOverrides()
{
    Theme theme =
        ThemeBuilder().buildLightFromSeed(
            QColor(
                QStringLiteral("#6750A4")));

    ComponentTokenOverride tokens;
    tokens.custom.insert(
        QStringLiteral("itemSpacing"),
        6);
    tokens.custom.insert(
        QStringLiteral("showDividers"),
        true);
    tokens.custom.insert(
        QStringLiteral("cornerRadius"),
        20.0);
    tokens.custom.insert(
        QStringLiteral("containerColor"),
        QColor(Qt::red));

    theme.componentOverrides().setOverride(
        QStringLiteral("list"),
        tokens);

    const ListSpec spec =
        DataSpecResolver().listSpec(theme);

    QCOMPARE(spec.itemSpacing, 6);
    QVERIFY(spec.showDividers);
    QCOMPARE(spec.cornerRadius, 20.0);
    QCOMPARE(spec.containerColor, QColor(Qt::red));
}

void tst_DataListGridResolver::
appliesGridListOverrides()
{
    Theme theme =
        ThemeBuilder().buildLightFromSeed(
            QColor(
                QStringLiteral("#6750A4")));

    ComponentTokenOverride tokens;
    tokens.custom.insert(
        QStringLiteral("columns"),
        4);
    tokens.custom.insert(
        QStringLiteral("itemWidth"),
        192);
    tokens.custom.insert(
        QStringLiteral("itemHeight"),
        160);
    tokens.custom.insert(
        QStringLiteral("spacing"),
        18);
    tokens.custom.insert(
        QStringLiteral("itemSelectedColor"),
        QColor(Qt::blue));

    theme.componentOverrides().setOverride(
        QStringLiteral("gridList"),
        tokens);

    const GridListSpec spec =
        DataSpecResolver().gridListSpec(theme);

    QCOMPARE(spec.columns, 4);
    QCOMPARE(spec.itemSize, QSize(192, 160));
    QCOMPARE(spec.spacing, 18);
    QCOMPARE(
        spec.itemSelectedColor,
        QColor(Qt::blue));
}

void tst_DataListGridResolver::
densityAffectsMetrics()
{
    const Theme theme =
        ThemeBuilder().buildLightFromSeed(
            QColor(
                QStringLiteral("#6750A4")));

    const DataSpecResolver resolver;

    const ListSpec compactList =
        resolver.listSpec(
            theme,
            Density::Compact);
    const ListSpec comfortableList =
        resolver.listSpec(
            theme,
            Density::Comfortable);

    QVERIFY(
        compactList.minimumViewportSize.height()
        < comfortableList.minimumViewportSize.height());

    const GridListSpec compactGrid =
        resolver.gridListSpec(
            theme,
            Density::Compact);
    const GridListSpec comfortableGrid =
        resolver.gridListSpec(
            theme,
            Density::Comfortable);

    QVERIFY(
        compactGrid.itemSize.width()
        < comfortableGrid.itemSize.width());
    QVERIFY(
        compactGrid.spacing
        < comfortableGrid.spacing);
}

QTEST_MAIN(tst_DataListGridResolver)

#include "tst_datalistgridresolver.moc"
