#include <QtTest>

#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/specs/qtmaterialdataspecresolver.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterial/widgets/data/qtmaterialcarousel.h"

using namespace QtMaterial;

class tst_ThemedCarousel : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void resolvesSpecFromParentContext();
    void sizeHintUsesResolvedMetrics();
    void followsThemeChanges();
    void explicitSpecRemainsPinned();
};

void tst_ThemedCarousel::resolvesSpecFromParentContext()
{
    const Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    ThemeContext context(theme);

    QtMaterialWidget parent;
    parent.setThemeContext(&context);
    QtMaterialCarousel carousel(&parent);

    const CarouselSpec expected =
        DataSpecResolver().carouselSpec(theme);

    QCOMPARE(carousel.effectiveThemeContext(), &context);
    QVERIFY(!carousel.hasExplicitSpec());
    QCOMPARE(
        carousel.resolvedSpec().itemBackgroundColor,
        expected.itemBackgroundColor);
    QCOMPARE(
        carousel.resolvedSpec().supportingFont,
        expected.supportingFont);
}

void tst_ThemedCarousel::sizeHintUsesResolvedMetrics()
{
    QtMaterialCarousel carousel;
    carousel.setVisibleItemCount(3);

    CarouselSpec custom = carousel.spec();
    custom.itemSize = QSize(200, 140);
    custom.outerMargins = QMargins(20, 20, 20, 20);
    custom.pageSpacing = 14;
    custom.indicatorSize = 10;
    carousel.setSpec(custom);

    QCOMPARE(
        carousel.sizeHint(),
        QSize(
            40 + 3 * 200 + 2 * 14,
            40 + 140 + 10 + 14));
}

void tst_ThemedCarousel::followsThemeChanges()
{
    Theme first = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    Theme second = ThemeBuilder().buildDarkFromSeed(
        QColor(QStringLiteral("#006874")));
    ThemeContext context(first);

    QtMaterialCarousel carousel;
    carousel.setThemeContext(&context);
    const QColor before =
        carousel.resolvedSpec().backgroundColor;

    QVERIFY(context.setTheme(second));

    const CarouselSpec expected =
        DataSpecResolver().carouselSpec(second);
    QCOMPARE(
        carousel.resolvedSpec().backgroundColor,
        expected.backgroundColor);
    QVERIFY(carousel.resolvedSpec().backgroundColor != before);
}

void tst_ThemedCarousel::explicitSpecRemainsPinned()
{
    ThemeContext context(
        ThemeBuilder().buildLightFromSeed(
            QColor(QStringLiteral("#6750A4"))));

    QtMaterialCarousel carousel;
    carousel.setThemeContext(&context);

    CarouselSpec explicitSpec = carousel.spec();
    explicitSpec.itemSelectedColor =
        QColor(QStringLiteral("#123456"));
    explicitSpec.cornerRadius = 8;
    carousel.setSpec(explicitSpec);

    QVERIFY(carousel.hasExplicitSpec());

    QVERIFY(context.setTheme(
        ThemeBuilder().buildDarkFromSeed(
            QColor(QStringLiteral("#006874")))));

    QCOMPARE(
        carousel.resolvedSpec().itemSelectedColor,
        QColor(QStringLiteral("#123456")));
    QCOMPARE(carousel.resolvedSpec().cornerRadius, 8);

    carousel.resetSpec();
    QVERIFY(!carousel.hasExplicitSpec());
    QVERIFY(
        carousel.resolvedSpec().itemSelectedColor
        != QColor(QStringLiteral("#123456")));
}

QTEST_MAIN(tst_ThemedCarousel)
#include "tst_themedcarousel.moc"
