#include <QtTest>

#include "qtmaterial/specs/qtmaterialdataspecresolver.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_DataWidgetResolvedSpec : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void gridListContainsCompletePaintValues();
    void carouselKeepsAdvancedResolvedValues();
    void componentOverridesWin();
};

void tst_DataWidgetResolvedSpec::gridListContainsCompletePaintValues()
{
    const Theme theme = ThemeBuilder().buildDarkFromSeed(
        QColor(QStringLiteral("#6750A4")));
    const GridListSpec spec =
        DataSpecResolver().gridListSpec(theme);

    QVERIFY(spec.disabledTextColor.isValid());
    QVERIFY(spec.disabledSupportingTextColor.isValid());
    QVERIFY(spec.itemOutlineColor.isValid());
    QVERIFY(spec.itemSize.isValid());
    QVERIFY(spec.iconSize.isValid());
    QVERIFY(spec.itemContentMargins.left() >= 0);
    QVERIFY(spec.outlineWidth >= 0);
}

void tst_DataWidgetResolvedSpec::carouselKeepsAdvancedResolvedValues()
{
    const Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#006874")));
    const CarouselSpec spec =
        DataSpecResolver().carouselSpec(theme);

    QVERIFY(spec.itemBackgroundColor.isValid());
    QVERIFY(spec.itemSelectedColor.isValid());
    QVERIFY(spec.itemSelectedTextColor.isValid());
    QVERIFY(spec.outlineColor.isValid());
    QVERIFY(spec.selectedOutlineColor.isValid());
    QVERIFY(spec.itemSize.isValid());
    QVERIFY(spec.minimumItemSize.isValid());
    QVERIFY(!spec.supportingFont.family().isEmpty());
}

void tst_DataWidgetResolvedSpec::componentOverridesWin()
{
    Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));

    ComponentTokenOverride gridTokens;
    gridTokens.custom.insert(QStringLiteral("itemWidth"), 188);
    gridTokens.custom.insert(QStringLiteral("iconWidth"), 40);
    gridTokens.custom.insert(QStringLiteral("outlineWidth"), 3);
    gridTokens.custom.insert(
        QStringLiteral("itemOutlineColor"),
        QColor(QStringLiteral("#123456")));
    theme.componentOverrides().setOverride(
        QStringLiteral("gridList"), gridTokens);

    ComponentTokenOverride carouselTokens;
    carouselTokens.custom.insert(QStringLiteral("itemWidth"), 204);
    carouselTokens.custom.insert(QStringLiteral("pageSpacing"), 17);
    carouselTokens.custom.insert(
        QStringLiteral("itemSelectedColor"),
        QColor(QStringLiteral("#345678")));
    theme.componentOverrides().setOverride(
        QStringLiteral("carousel"), carouselTokens);

    const GridListSpec grid =
        DataSpecResolver().gridListSpec(theme);
    const CarouselSpec carousel =
        DataSpecResolver().carouselSpec(theme);

    QCOMPARE(grid.itemSize.width(), 188);
    QCOMPARE(grid.iconSize.width(), 40);
    QCOMPARE(grid.outlineWidth, 3);
    QCOMPARE(grid.itemOutlineColor, QColor(QStringLiteral("#123456")));

    QCOMPARE(carousel.itemSize.width(), 204);
    QCOMPARE(carousel.pageSpacing, 17);
    QCOMPARE(
        carousel.itemSelectedColor,
        QColor(QStringLiteral("#345678")));
}

QTEST_MAIN(tst_DataWidgetResolvedSpec)
#include "tst_datawidgetresolvedspec.moc"
