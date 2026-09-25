#include <QtTest>

#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterial/widgets/data/qtmaterialtable.h"

using namespace QtMaterial;

class tst_ThemedTable : public QObject
{
    Q_OBJECT

private slots:
    void resolvesDefaultSpecFromParentContext();
    void followsThemeChangesInThemeMode();
    void explicitSpecRemainsPinned();
    void resetSpecReturnsToThemeMode();
};

void tst_ThemedTable::resolvesDefaultSpecFromParentContext()
{
    const Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    ThemeContext context(theme);

    QtMaterialWidget parent;
    parent.setThemeContext(&context);
    QtMaterialTable table(&parent);

    QVERIFY(!table.hasExplicitSpec());
    QCOMPARE(table.effectiveThemeContext(), &context);
    QCOMPARE(
        table.spec().backgroundColor,
        theme.colorScheme().color(ColorRole::Surface));
    QCOMPARE(
        table.spec().bodyFont,
        theme.typography().style(TypeRole::BodyMedium).font);
}

void tst_ThemedTable::followsThemeChangesInThemeMode()
{
    const Theme first = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    const Theme second = ThemeBuilder().buildDarkFromSeed(
        QColor(QStringLiteral("#006874")));
    ThemeContext context(first);

    QtMaterialTable table;
    table.setThemeContext(&context);
    const QColor before = table.spec().backgroundColor;

    QVERIFY(context.setTheme(second));

    QCOMPARE(
        table.spec().backgroundColor,
        second.colorScheme().color(ColorRole::Surface));
    QVERIFY(table.spec().backgroundColor != before);
}

void tst_ThemedTable::explicitSpecRemainsPinned()
{
    ThemeContext context(
        ThemeBuilder().buildLightFromSeed(
            QColor(QStringLiteral("#6750A4"))));

    QtMaterialTable table;
    table.setThemeContext(&context);

    TableSpec explicitSpec = table.spec();
    explicitSpec.backgroundColor = QColor(QStringLiteral("#123456"));
    explicitSpec.rowHeight = 77;
    table.setSpec(explicitSpec);

    QVERIFY(table.hasExplicitSpec());

    const Theme replacement = ThemeBuilder().buildDarkFromSeed(
        QColor(QStringLiteral("#006874")));
    QVERIFY(context.setTheme(replacement));

    QCOMPARE(
        table.spec().backgroundColor,
        QColor(QStringLiteral("#123456")));
    QCOMPARE(table.spec().rowHeight, 77);
}

void tst_ThemedTable::resetSpecReturnsToThemeMode()
{
    const Theme theme = ThemeBuilder().buildDarkFromSeed(
        QColor(QStringLiteral("#006874")));
    ThemeContext context(theme);

    QtMaterialTable table;
    table.setThemeContext(&context);

    TableSpec explicitSpec = table.spec();
    explicitSpec.backgroundColor = QColor(QStringLiteral("#123456"));
    table.setSpec(explicitSpec);
    table.resetSpec();

    QVERIFY(!table.hasExplicitSpec());
    QCOMPARE(
        table.spec().backgroundColor,
        theme.colorScheme().color(ColorRole::Surface));
}

QTEST_MAIN(tst_ThemedTable)
#include "tst_themedtable.moc"
