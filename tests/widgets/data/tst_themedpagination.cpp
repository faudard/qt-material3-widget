#include <QtTest>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterial/widgets/data/qtmaterialpagination.h"

using namespace QtMaterial;

class tst_ThemedPagination : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void followsThemeChanges();
    void explicitSpecRemainsPinned();
    void resetSpecReturnsToThemeMode();
};

void tst_ThemedPagination::followsThemeChanges()
{
    const Theme first = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    const Theme second = ThemeBuilder().buildDarkFromSeed(
        QColor(QStringLiteral("#006874")));
    ThemeContext context(first);

    QtMaterialPagination pagination;
    pagination.setThemeContext(&context);
    const QColor before = pagination.spec().backgroundColor;

    QVERIFY(context.setTheme(second));
    QCOMPARE(
        pagination.spec().backgroundColor,
        second.colorScheme().color(ColorRole::Surface));
    QVERIFY(pagination.spec().backgroundColor != before);
}

void tst_ThemedPagination::explicitSpecRemainsPinned()
{
    ThemeContext context(
        ThemeBuilder().buildLightFromSeed(
            QColor(QStringLiteral("#6750A4"))));

    QtMaterialPagination pagination;
    pagination.setThemeContext(&context);

    PaginationSpec explicitSpec = pagination.spec();
    explicitSpec.backgroundColor = QColor(QStringLiteral("#123456"));
    explicitSpec.controlExtent = 48;
    pagination.setSpec(explicitSpec);

    const Theme replacement = ThemeBuilder().buildDarkFromSeed(
        QColor(QStringLiteral("#006874")));
    QVERIFY(context.setTheme(replacement));

    QVERIFY(pagination.hasExplicitSpec());
    QCOMPARE(
        pagination.spec().backgroundColor,
        QColor(QStringLiteral("#123456")));
    QCOMPARE(pagination.spec().controlExtent, 48);
}

void tst_ThemedPagination::resetSpecReturnsToThemeMode()
{
    const Theme theme = ThemeBuilder().buildDarkFromSeed(
        QColor(QStringLiteral("#006874")));
    ThemeContext context(theme);

    QtMaterialPagination pagination;
    pagination.setThemeContext(&context);
    PaginationSpec explicitSpec = pagination.spec();
    explicitSpec.backgroundColor = QColor(QStringLiteral("#123456"));
    pagination.setSpec(explicitSpec);
    pagination.resetSpec();

    QVERIFY(!pagination.hasExplicitSpec());
    QCOMPARE(
        pagination.spec().backgroundColor,
        theme.colorScheme().color(ColorRole::Surface));
}

QTEST_MAIN(tst_ThemedPagination)
#include "tst_themedpagination.moc"
