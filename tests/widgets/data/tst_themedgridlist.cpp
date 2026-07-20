#include <QtTest>

#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/specs/qtmaterialdataspecresolver.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterial/widgets/data/qtmaterialgridlist.h"

using namespace QtMaterial;

class tst_ThemedGridList : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void resolvesSpecFromParentContext();
    void followsThemeChanges();
    void explicitSpecRemainsPinned();
    void cellExtentOverrideCanBeReset();
};

void tst_ThemedGridList::resolvesSpecFromParentContext()
{
    const Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    ThemeContext context(theme);

    QtMaterialWidget parent;
    parent.setThemeContext(&context);
    QtMaterialGridList grid(&parent);

    const GridListSpec expected =
        DataSpecResolver().gridListSpec(theme);

    QCOMPARE(grid.effectiveThemeContext(), &context);
    QVERIFY(!grid.hasExplicitSpec());
    QCOMPARE(
        grid.resolvedSpec().itemBackgroundColor,
        expected.itemBackgroundColor);
    QCOMPARE(grid.cellExtent(), expected.itemSize);
    QCOMPARE(grid.spacing(), expected.spacing);
}

void tst_ThemedGridList::followsThemeChanges()
{
    Theme first = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    Theme second = ThemeBuilder().buildDarkFromSeed(
        QColor(QStringLiteral("#006874")));
    ThemeContext context(first);

    QtMaterialGridList grid;
    grid.setThemeContext(&context);
    const QColor before = grid.resolvedSpec().backgroundColor;

    QVERIFY(context.setTheme(second));

    const GridListSpec expected =
        DataSpecResolver().gridListSpec(second);
    QCOMPARE(
        grid.resolvedSpec().backgroundColor,
        expected.backgroundColor);
    QVERIFY(grid.resolvedSpec().backgroundColor != before);
}

void tst_ThemedGridList::explicitSpecRemainsPinned()
{
    ThemeContext context(
        ThemeBuilder().buildLightFromSeed(
            QColor(QStringLiteral("#6750A4"))));

    QtMaterialGridList grid;
    grid.setThemeContext(&context);

    GridListSpec explicitSpec = grid.spec();
    explicitSpec.itemBackgroundColor =
        QColor(QStringLiteral("#123456"));
    explicitSpec.itemRadius = 7;
    grid.setSpec(explicitSpec);

    QVERIFY(grid.hasExplicitSpec());

    QVERIFY(context.setTheme(
        ThemeBuilder().buildDarkFromSeed(
            QColor(QStringLiteral("#006874")))));

    QCOMPARE(
        grid.resolvedSpec().itemBackgroundColor,
        QColor(QStringLiteral("#123456")));
    QCOMPARE(grid.resolvedSpec().itemRadius, 7);

    grid.resetSpec();
    QVERIFY(!grid.hasExplicitSpec());
    QVERIFY(
        grid.resolvedSpec().itemBackgroundColor
        != QColor(QStringLiteral("#123456")));
}

void tst_ThemedGridList::cellExtentOverrideCanBeReset()
{
    QtMaterialGridList grid;
    const QSize themedSize = grid.resolvedSpec().itemSize;

    grid.setCellExtent(QSize(210, 132));
    QVERIFY(grid.hasCellExtentOverride());
    QCOMPARE(grid.cellExtent(), QSize(210, 132));

    grid.resetCellExtent();
    QVERIFY(!grid.hasCellExtentOverride());
    QCOMPARE(grid.cellExtent(), themedSize);
}

QTEST_MAIN(tst_ThemedGridList)
#include "tst_themedgridlist.moc"
