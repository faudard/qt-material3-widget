#include <QObject>
#include <QtTest>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_ThemeTokens : public QObject {
    Q_OBJECT

private slots:
    void builder_producesCompleteFrozenTokenGroups();
    void componentOverrides_areStableAndSorted();
};

void tst_ThemeTokens::builder_producesCompleteFrozenTokenGroups()
{
    ThemeBuilder builder;
    const Theme theme = builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));

    QVERIFY(theme.colorScheme().contains(ColorRole::Primary));
    QVERIFY(theme.colorScheme().contains(ColorRole::PrimaryFixed));
    QVERIFY(theme.colorScheme().contains(ColorRole::PrimaryFixedDim));
    QVERIFY(theme.colorScheme().contains(ColorRole::OnPrimaryFixed));
    QVERIFY(theme.colorScheme().contains(ColorRole::OnPrimaryFixedVariant));
    QVERIFY(theme.colorScheme().contains(ColorRole::SecondaryFixed));
    QVERIFY(theme.colorScheme().contains(ColorRole::TertiaryFixed));
    QVERIFY(theme.colorScheme().contains(ColorRole::SurfaceTint));

    QVERIFY(theme.typography().contains(TypeRole::BodyLarge));
    QVERIFY(theme.shapes().contains(ShapeRole::Full));
    QVERIFY(theme.elevations().contains(ElevationRole::Level5));
    QVERIFY(theme.motion().contains(MotionToken::Long4));
    QVERIFY(theme.density().contains(DensityRole::Compact));
    QVERIFY(theme.density().contains(DensityRole::Default));
    QVERIFY(theme.density().contains(DensityRole::Comfortable));
    QVERIFY(theme.iconSizes().contains(IconSizeRole::ExtraSmall));
    QVERIFY(theme.iconSizes().contains(IconSizeRole::Medium));
    QVERIFY(theme.iconSizes().contains(IconSizeRole::ExtraLarge));
}

void tst_ThemeTokens::componentOverrides_areStableAndSorted()
{
    Theme theme;

    ComponentTokenOverride button;
    button.shapes.insert(ShapeRole::Full, 999);

    ComponentTokenOverride card;
    card.elevations.insert(ElevationRole::Level1, ElevationStyle{ 4, 1, 0.05 });

    theme.componentOverrides().setOverride(ComponentId::Card, card);
    theme.componentOverrides().setOverride(ComponentId::Button, button);

    const QList<ComponentId> ids = theme.componentOverrides().componentIds();
    QCOMPARE(ids.size(), 2);
    QCOMPARE(ids.at(0), ComponentId::Button);
    QCOMPARE(ids.at(1), ComponentId::Card);
    QVERIFY(theme.componentOverrides().contains(ComponentId::Button));
    QCOMPARE(theme.componentOverrides().overrideFor(ComponentId::Button).shapes.value(ShapeRole::Full), 999);
}

QTEST_MAIN(tst_ThemeTokens)
#include "tst_theme_tokens.moc"
