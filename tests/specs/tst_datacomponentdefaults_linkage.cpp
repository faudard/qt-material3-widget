#include <QtTest>

#include "qtmaterial/specs/qtmaterialdatacomponentspecresolver.h"
#include "qtmaterial/specs/qtmaterialdatacomponentspecs.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_DataComponentDefaultsLinkage : public QObject
{
    Q_OBJECT

private slots:
    void importsDefaultFactoriesFromSpecsLibrary();
    void resolvesThemeAwareSpecs();
};

void tst_DataComponentDefaultsLinkage::importsDefaultFactoriesFromSpecsLibrary()
{
    const DatePickerSpec datePicker = defaultDatePickerSpec();
    const TableSpec table = defaultTableSpec();
    const GridListSpec gridList = defaultGridListSpec();
    const CarouselSpec carousel = defaultCarouselSpec();

    QVERIFY(datePicker.cellSize > 0);
    QVERIFY(table.rowHeight > 0);
    QVERIFY(gridList.itemSize.isValid());
    QVERIFY(carousel.preferredPageSize.isValid());
}

void tst_DataComponentDefaultsLinkage::resolvesThemeAwareSpecs()
{
    const Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    const DataComponentSpecResolver resolver;

    QCOMPARE(
        resolver.tableSpec(theme).backgroundColor,
        theme.colorScheme().color(ColorRole::Surface));
    QCOMPARE(
        resolver.carouselSpec(theme).activePageIndicatorColor,
        theme.colorScheme().color(ColorRole::Primary));
}

QTEST_MAIN(tst_DataComponentDefaultsLinkage)
#include "tst_datacomponentdefaults_linkage.moc"
