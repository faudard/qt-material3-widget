#include <QtTest>

#include <QHeaderView>
#include <QStandardItemModel>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterial/widgets/data/qtmaterialtable.h"

using namespace QtMaterial;

class tst_TableResolvedSpec
    : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void exposesResolvedSpec();
    void themeContextRefreshesSpec();
    void explicitSpecRemainsStable();
    void resetSpecRestoresThemeResolution();
    void denseUsesCompactMetrics();
};

void tst_TableResolvedSpec::
exposesResolvedSpec()
{
    QtMaterialTable table;

    const TableSpec& spec =
        table.spec();

    QVERIFY(spec.backgroundColor.isValid());
    QVERIFY(spec.foregroundColor.isValid());
    QVERIFY(spec.rowHeight > 0);
    QVERIFY(spec.headerHeight > 0);

    QCOMPARE(
        table.verticalHeader()
            ->defaultSectionSize(),
        spec.rowHeight);
}

void tst_TableResolvedSpec::
themeContextRefreshesSpec()
{
    ThemeContext context(
        ThemeBuilder().buildLightFromSeed(
            QColor(
                QStringLiteral("#6750A4"))));

    QtMaterialTable table;
    table.setThemeContext(&context);

    const QColor first =
        table.spec().focusRingColor;

    context.setTheme(
        ThemeBuilder().buildLightFromSeed(
            QColor(
                QStringLiteral("#006874"))));

    const QColor second =
        table.spec().focusRingColor;

    QVERIFY(first != second);
    QCOMPARE(
        second,
        context.theme().colorScheme().color(
            ColorRole::Primary));
}

void tst_TableResolvedSpec::
explicitSpecRemainsStable()
{
    ThemeContext context(
        ThemeBuilder().buildLightFromSeed(
            QColor(
                QStringLiteral("#6750A4"))));

    QtMaterialTable table;
    table.setThemeContext(&context);

    TableSpec explicitSpec =
        table.spec();
    explicitSpec.rowHeight = 71;
    explicitSpec.backgroundColor =
        QColor(Qt::yellow);

    table.setSpec(explicitSpec);

    QVERIFY(table.hasExplicitSpec());
    QCOMPARE(
        table.spec().rowHeight,
        71);
    QCOMPARE(
        table.spec().backgroundColor,
        QColor(Qt::yellow));

    context.setTheme(
        ThemeBuilder().buildLightFromSeed(
            QColor(
                QStringLiteral("#006874"))));

    QCOMPARE(
        table.spec().rowHeight,
        71);
    QCOMPARE(
        table.spec().backgroundColor,
        QColor(Qt::yellow));
}

void tst_TableResolvedSpec::
resetSpecRestoresThemeResolution()
{
    ThemeContext context(
        ThemeBuilder().buildLightFromSeed(
            QColor(
                QStringLiteral("#006874"))));

    QtMaterialTable table;
    table.setThemeContext(&context);

    TableSpec explicitSpec =
        table.spec();
    explicitSpec.backgroundColor =
        QColor(Qt::yellow);
    table.setSpec(explicitSpec);

    table.resetSpec();

    QVERIFY(!table.hasExplicitSpec());
    QCOMPARE(
        table.spec().backgroundColor,
        context.theme().colorScheme().color(
            ColorRole::Surface));
}

void tst_TableResolvedSpec::
denseUsesCompactMetrics()
{
    QtMaterialTable table;

    const int regularHeight =
        table.verticalHeader()
            ->defaultSectionSize();

    table.setDense(true);

    QCOMPARE(
        table.verticalHeader()
            ->defaultSectionSize(),
        table.spec().denseRowHeight);
    QVERIFY(
        table.verticalHeader()
            ->defaultSectionSize()
        < regularHeight);
}

QTEST_MAIN(tst_TableResolvedSpec)

#include "tst_table_resolvedspec.moc"
