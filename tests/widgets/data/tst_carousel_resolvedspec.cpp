#include <QtTest>

#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterial/widgets/data/qtmaterialcarousel.h"

using namespace QtMaterial;

class tst_CarouselResolvedSpec
    : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void exposesResolvedSpec();
    void refreshesAfterThemeChange();
    void densityUpdatesSizeHint();
    void componentMetricsDriveSizeHint();
};

void tst_CarouselResolvedSpec::
exposesResolvedSpec()
{
    Theme theme =
        ThemeBuilder().buildLightFromSeed(
            QColor(
                QStringLiteral("#6750A4")));
    theme.colorScheme().setColor(
        ColorRole::SecondaryContainer,
        QColor(Qt::green));

    ThemeContext context(theme);
    QtMaterialCarousel carousel;
    carousel.setThemeContext(&context);

    QCOMPARE(
        carousel.resolvedSpec().itemSelectedColor,
        QColor(Qt::green));
    QVERIFY(
        qobject_cast<QtMaterialControl*>(
            &carousel) != nullptr);
}

void tst_CarouselResolvedSpec::
refreshesAfterThemeChange()
{
    Theme first =
        ThemeBuilder().buildLightFromSeed(
            QColor(
                QStringLiteral("#6750A4")));
    first.colorScheme().setColor(
        ColorRole::Surface,
        QColor(Qt::red));

    Theme second = first;
    second.colorScheme().setColor(
        ColorRole::Surface,
        QColor(Qt::blue));

    ThemeContext context(first);
    QtMaterialCarousel carousel;
    carousel.setThemeContext(&context);

    QCOMPARE(
        carousel.resolvedSpec().backgroundColor,
        QColor(Qt::red));

    QVERIFY(context.setTheme(second));

    QCOMPARE(
        carousel.resolvedSpec().backgroundColor,
        QColor(Qt::blue));
}

void tst_CarouselResolvedSpec::
densityUpdatesSizeHint()
{
    QtMaterialCarousel carousel;
    carousel.setVisibleItemCount(3);

    carousel.setDensity(Density::Compact);
    const QSize compact = carousel.sizeHint();

    carousel.setDensity(Density::Comfortable);
    const QSize comfortable = carousel.sizeHint();

    QVERIFY(compact.width() < comfortable.width());
    QVERIFY(compact.height() < comfortable.height());
}

void tst_CarouselResolvedSpec::
componentMetricsDriveSizeHint()
{
    Theme theme =
        ThemeBuilder().buildLightFromSeed(
            QColor(
                QStringLiteral("#6750A4")));

    ComponentTokenOverride tokens;
    tokens.custom.insert(
        QStringLiteral("itemWidth"),
        200);
    tokens.custom.insert(
        QStringLiteral("itemHeight"),
        140);
    tokens.custom.insert(
        QStringLiteral("pageSpacing"),
        20);
    tokens.custom.insert(
        QStringLiteral("outerMarginLeft"),
        10);
    tokens.custom.insert(
        QStringLiteral("outerMarginRight"),
        14);
    tokens.custom.insert(
        QStringLiteral("outerMarginTop"),
        8);
    tokens.custom.insert(
        QStringLiteral("outerMarginBottom"),
        12);

    theme.componentOverrides().setOverride(
        QStringLiteral("carousel"),
        tokens);

    ThemeContext context(theme);
    QtMaterialCarousel carousel;
    carousel.setThemeContext(&context);
    carousel.setVisibleItemCount(2);

    QCOMPARE(
        carousel.sizeHint(),
        QSize(
            10 + 14 + 2 * 200 + 20,
            8 + 12 + 140));
}

QTEST_MAIN(tst_CarouselResolvedSpec)

#include "tst_carousel_resolvedspec.moc"
