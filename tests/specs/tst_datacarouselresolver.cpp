#include <QtTest>

#include "qtmaterial/specs/qtmaterialdataspecresolver.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_DataCarouselResolver
    : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void resolvesThemeTokens();
    void appliesComponentOverrides();
    void densityAffectsMetrics();
};

void tst_DataCarouselResolver::
resolvesThemeTokens()
{
    const Theme theme =
        ThemeBuilder().buildLightFromSeed(
            QColor(
                QStringLiteral("#6750A4")));

    const CarouselSpec spec =
        DataSpecResolver().carouselSpec(theme);

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
    QCOMPARE(
        spec.selectedOutlineColor,
        theme.colorScheme().color(
            ColorRole::Primary));

    QVERIFY(spec.itemSize.isValid());
    QVERIFY(spec.minimumItemSize.isValid());
    QVERIFY(spec.cornerRadius >= 0.0);
    QVERIFY(!spec.labelFont.family().isEmpty());
    QVERIFY(!spec.supportingFont.family().isEmpty());
}

void tst_DataCarouselResolver::
appliesComponentOverrides()
{
    Theme theme =
        ThemeBuilder().buildLightFromSeed(
            QColor(
                QStringLiteral("#006874")));

    ComponentTokenOverride tokens;
    tokens.custom.insert(
        QStringLiteral("itemWidth"),
        214);
    tokens.custom.insert(
        QStringLiteral("itemHeight"),
        152);
    tokens.custom.insert(
        QStringLiteral("pageSpacing"),
        22);
    tokens.custom.insert(
        QStringLiteral("cornerRadius"),
        24.0);
    tokens.custom.insert(
        QStringLiteral("itemSelectedColor"),
        QColor(Qt::red));

    theme.componentOverrides().setOverride(
        QStringLiteral("carousel"),
        tokens);

    const CarouselSpec spec =
        DataSpecResolver().carouselSpec(theme);

    QCOMPARE(spec.itemSize, QSize(214, 152));
    QCOMPARE(spec.pageSpacing, 22);
    QCOMPARE(spec.cornerRadius, 24.0);
    QCOMPARE(
        spec.itemSelectedColor,
        QColor(Qt::red));
}

void tst_DataCarouselResolver::
densityAffectsMetrics()
{
    const Theme theme =
        ThemeBuilder().buildLightFromSeed(
            QColor(
                QStringLiteral("#6750A4")));

    const DataSpecResolver resolver;

    const CarouselSpec compact =
        resolver.carouselSpec(
            theme,
            Density::Compact);
    const CarouselSpec comfortable =
        resolver.carouselSpec(
            theme,
            Density::Comfortable);

    QVERIFY(
        compact.itemSize.width()
        < comfortable.itemSize.width());
    QVERIFY(
        compact.itemSize.height()
        < comfortable.itemSize.height());
    QVERIFY(
        compact.pageSpacing
        < comfortable.pageSpacing);
    QVERIFY(
        compact.iconSize
        < comfortable.iconSize);
}

QTEST_MAIN(tst_DataCarouselResolver)

#include "tst_datacarouselresolver.moc"
