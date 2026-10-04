#include <QtTest>

#include "qtmaterial/specs/qtmaterialdataspecresolver.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_DataPaginationResolver : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void resolvesThemeTokens();
    void densityAffectsMetrics();
    void appliesComponentOverrides();
};

void tst_DataPaginationResolver::resolvesThemeTokens()
{
    const Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    const PaginationSpec spec = DataSpecResolver().paginationSpec(theme);

    QCOMPARE(spec.backgroundColor, theme.colorScheme().color(ColorRole::Surface));
    QCOMPARE(spec.foregroundColor, theme.colorScheme().color(ColorRole::OnSurfaceVariant));
    QCOMPARE(spec.controlColor, theme.colorScheme().color(ColorRole::OnSurface));
    QCOMPARE(spec.focusRingColor, theme.colorScheme().color(ColorRole::Primary));
    QVERIFY(!spec.labelFont.family().isEmpty());
}

void tst_DataPaginationResolver::densityAffectsMetrics()
{
    const Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#006874")));
    const DataSpecResolver resolver;

    const PaginationSpec compact =
        resolver.paginationSpec(theme, Density::Compact);
    const PaginationSpec comfortable =
        resolver.paginationSpec(theme, Density::Comfortable);

    QVERIFY(compact.minimumHeight < comfortable.minimumHeight);
    QVERIFY(compact.controlExtent < comfortable.controlExtent);
    QVERIFY(compact.horizontalPadding < comfortable.horizontalPadding);
}

void tst_DataPaginationResolver::appliesComponentOverrides()
{
    Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));

    ComponentTokenOverride tokens;
    tokens.custom.insert(QStringLiteral("minimumHeight"), 52);
    tokens.custom.insert(QStringLiteral("controlExtent"), 44);
    tokens.custom.insert(QStringLiteral("spacing"), 10);
    tokens.custom.insert(QStringLiteral("horizontalPadding"), 14);
    tokens.custom.insert(QStringLiteral("controlColor"), QColor(Qt::red));

    theme.componentOverrides().setOverride(ComponentId::Data, tokens);

    const PaginationSpec spec = DataSpecResolver().paginationSpec(theme);
    QCOMPARE(spec.minimumHeight, 52);
    QCOMPARE(spec.controlExtent, 44);
    QCOMPARE(spec.spacing, 10);
    QCOMPARE(spec.horizontalPadding, 14);
    QCOMPARE(spec.controlColor, QColor(Qt::red));
}

QTEST_MAIN(tst_DataPaginationResolver)
#include "tst_datapaginationresolver.moc"
