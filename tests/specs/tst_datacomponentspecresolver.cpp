#include <QtTest>

#include "qtmaterial/specs/qtmaterialdatacomponentspecresolver.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_DataComponentSpecResolver : public QObject
{
    Q_OBJECT

private slots:
    void resolvesDatePicker();
    void resolvesTable();
    void resolvesGridList();
    void resolvesCarousel();
};

void tst_DataComponentSpecResolver::resolvesDatePicker()
{
    const Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    const DatePickerSpec spec =
        DataComponentSpecResolver().datePickerSpec(theme);

    QCOMPARE(
        spec.backgroundColor,
        theme.colorScheme().color(ColorRole::SurfaceContainerHigh));
    QCOMPARE(
        spec.selectedDateColor,
        theme.colorScheme().color(ColorRole::Primary));
    QCOMPARE(spec.hoverColor.alpha(), 20);
    QCOMPARE(spec.headlineFont, theme.typography().headlineSmall);
}

void tst_DataComponentSpecResolver::resolvesTable()
{
    const Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    const TableSpec spec =
        DataComponentSpecResolver().tableSpec(theme);

    QCOMPARE(
        spec.headerBackgroundColor,
        theme.colorScheme().color(ColorRole::SurfaceContainer));
    QCOMPARE(
        spec.rowSelectedColor,
        theme.colorScheme().color(ColorRole::SecondaryContainer));
    QCOMPARE(spec.rowHoverColor.alpha(), 15);
    QCOMPARE(spec.bodyFont, theme.typography().bodyMedium);
}

void tst_DataComponentSpecResolver::resolvesGridList()
{
    const Theme theme = ThemeBuilder().buildDarkFromSeed(
        QColor(QStringLiteral("#6750A4")));
    const GridListSpec spec =
        DataComponentSpecResolver().gridListSpec(theme);

    QCOMPARE(
        spec.itemBackgroundColor,
        theme.colorScheme().color(ColorRole::SurfaceContainer));
    QCOMPARE(
        spec.supportingTextColor,
        theme.colorScheme().color(ColorRole::OnSurfaceVariant));
    QCOMPARE(spec.itemHoverColor.alpha(), 15);
}

void tst_DataComponentSpecResolver::resolvesCarousel()
{
    const Theme theme = ThemeBuilder().buildDarkFromSeed(
        QColor(QStringLiteral("#6750A4")));
    const CarouselSpec spec =
        DataComponentSpecResolver().carouselSpec(theme);

    QCOMPARE(
        spec.backgroundColor,
        theme.colorScheme().color(ColorRole::SurfaceContainerLow));
    QCOMPARE(
        spec.activePageIndicatorColor,
        theme.colorScheme().color(ColorRole::Primary));
    QCOMPARE(spec.labelFont, theme.typography().labelLarge);
}

QTEST_MAIN(tst_DataComponentSpecResolver)
#include "tst_datacomponentspecresolver.moc"
