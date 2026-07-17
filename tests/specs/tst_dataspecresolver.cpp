#include <QtTest>

#include "qtmaterial/specs/qtmaterialdataspecresolver.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_DataSpecResolver : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void resolvesListItemTokens();
    void resolvesDividerTokens();
    void appliesComponentOverrides();
    void densityAffectsListItemMetrics();
};

void tst_DataSpecResolver::resolvesListItemTokens()
{
    const Theme theme =
        ThemeBuilder().buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));

    const ListItemSpec spec =
        DataSpecResolver().listItemSpec(theme);

    QCOMPARE(
        spec.headlineColor,
        theme.colorScheme().color(
            ColorRole::OnSurface));
    QCOMPARE(
        spec.supportingTextColor,
        theme.colorScheme().color(
            ColorRole::OnSurfaceVariant));
    QCOMPARE(
        spec.dividerColor,
        theme.colorScheme().color(
            ColorRole::OutlineVariant));

    QVERIFY(spec.headlineFont.family().isEmpty() == false);
    QVERIFY(spec.supportingFont.family().isEmpty() == false);
    QVERIFY(spec.minHeight >= 48);
    QVERIFY(spec.iconSize > 0);
    QVERIFY(spec.hoverOpacity >= 0.0);
    QVERIFY(spec.hoverOpacity <= 1.0);
}

void tst_DataSpecResolver::resolvesDividerTokens()
{
    const Theme theme =
        ThemeBuilder().buildLightFromSeed(
            QColor(QStringLiteral("#006874")));

    const DividerSpec spec =
        DataSpecResolver().dividerSpec(theme);

    QCOMPARE(
        spec.color,
        theme.colorScheme().color(
            ColorRole::OutlineVariant));
    QCOMPARE(spec.insetColor, spec.color);
    QCOMPARE(spec.thickness, 1);
    QCOMPARE(spec.leadingInset, 0);
    QCOMPARE(spec.trailingInset, 0);
}

void tst_DataSpecResolver::appliesComponentOverrides()
{
    Theme theme =
        ThemeBuilder().buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));

    ComponentTokenOverride listItemOverride;
    listItemOverride.custom.insert(
        QStringLiteral("horizontalPadding"),
        24);
    listItemOverride.custom.insert(
        QStringLiteral("headlineColor"),
        QColor(Qt::red));
    listItemOverride.custom.insert(
        QStringLiteral("showDivider"),
        true);

    theme.componentOverrides().setOverride(
        QStringLiteral("listItem"),
        listItemOverride);

    ComponentTokenOverride dividerOverride;
    dividerOverride.custom.insert(
        QStringLiteral("thickness"),
        3);
    dividerOverride.custom.insert(
        QStringLiteral("color"),
        QColor(Qt::blue));

    theme.componentOverrides().setOverride(
        QStringLiteral("divider"),
        dividerOverride);

    const DataSpecResolver resolver;
    const ListItemSpec listItem =
        resolver.listItemSpec(theme);
    const DividerSpec divider =
        resolver.dividerSpec(theme);

    QCOMPARE(listItem.horizontalPadding, 24);
    QCOMPARE(
        listItem.headlineColor,
        QColor(Qt::red));
    QVERIFY(listItem.showDivider);

    QCOMPARE(divider.thickness, 3);
    QCOMPARE(divider.color, QColor(Qt::blue));
}

void tst_DataSpecResolver::densityAffectsListItemMetrics()
{
    const Theme theme =
        ThemeBuilder().buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));

    const DataSpecResolver resolver;

    const ListItemSpec compact =
        resolver.listItemSpec(
            theme,
            Density::Compact);
    const ListItemSpec standard =
        resolver.listItemSpec(
            theme,
            Density::Default);
    const ListItemSpec comfortable =
        resolver.listItemSpec(
            theme,
            Density::Comfortable);

    QVERIFY(
        compact.horizontalPadding
        < standard.horizontalPadding);
    QVERIFY(
        comfortable.horizontalPadding
        > standard.horizontalPadding);
}

QTEST_MAIN(tst_DataSpecResolver)

#include "tst_dataspecresolver.moc"
