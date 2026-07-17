#include <QtTest/QtTest>

#include "qtmaterial/specs/qtmaterialbuttonspecresolver.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_ButtonSpecResolver : public QObject
{
    Q_OBJECT

private slots:
    void variantOverridesDoNotLeak();
    void resolvesAllVariants();
    void resolvesDensity();
    void remainsCompatibleWithSpecFactory();
};

void tst_ButtonSpecResolver::resolvesAllVariants()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));
    ButtonSpecResolver resolver;

    const ButtonSpec text = resolver.resolve(ButtonVariant::Text, theme);
    const ButtonSpec filled = resolver.resolve(ButtonVariant::Filled, theme);
    const ButtonSpec tonal =
        resolver.resolve(ButtonVariant::FilledTonal, theme);
    const ButtonSpec outlined =
        resolver.resolve(ButtonVariant::Outlined, theme);
    const ButtonSpec elevated =
        resolver.resolve(ButtonVariant::Elevated, theme);

    QVERIFY(text.labelColor.isValid());
    QVERIFY(filled.containerColor.isValid());
    QVERIFY(tonal.containerColor.isValid());
    QVERIFY(outlined.outlineColor.isValid());
    QCOMPARE(
        static_cast<int>(elevated.elevationRole),
        static_cast<int>(ElevationRole::Level1));

    QCOMPARE(text.containerColor, QColor(Qt::transparent));
    QCOMPARE(outlined.containerColor, QColor(Qt::transparent));
    QCOMPARE(
        filled.labelColor,
        theme.colorScheme().color(ColorRole::OnPrimary));
}

void tst_ButtonSpecResolver::resolvesDensity()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));
    ButtonSpecResolver resolver;

    QCOMPARE(
        resolver.textButtonSpec(theme, Density::Compact).containerHeight,
        36);
    QCOMPARE(
        resolver.textButtonSpec(theme, Density::Default).containerHeight,
        40);
    QCOMPARE(
        resolver.textButtonSpec(theme, Density::Comfortable).containerHeight,
        44);
}

void tst_ButtonSpecResolver::remainsCompatibleWithSpecFactory()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildDarkFromSeed(QColor(QStringLiteral("#6750A4")));
    ButtonSpecResolver resolver;
    SpecFactory factory;

    const ButtonSpec resolved =
        resolver.filledTonalButtonSpec(theme, Density::Compact);
    const ButtonSpec legacy =
        factory.filledTonalButtonSpec(theme, Density::Compact);

    QCOMPARE(resolved.containerColor, legacy.containerColor);
    QCOMPARE(resolved.labelColor, legacy.labelColor);
    QCOMPARE(resolved.stateLayerColor, legacy.stateLayerColor);
    QCOMPARE(resolved.containerHeight, legacy.containerHeight);
    QCOMPARE(
        static_cast<int>(resolved.elevationRole),
        static_cast<int>(legacy.elevationRole));
}

void tst_ButtonSpecResolver::variantOverridesDoNotLeak()
{
    ThemeBuilder builder;
    Theme theme = builder.buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));

    const QColor textOnlyColor(QStringLiteral("#B3261E"));
    ComponentTokenOverride textTokens;
    textTokens.custom.insert(
        QStringLiteral("containerColor"),
        textOnlyColor);
    theme.componentOverrides().setOverride(
        QStringLiteral("button.text"),
        textTokens);

    ButtonSpecResolver resolver;
    const ButtonSpec text = resolver.textButtonSpec(theme);
    const ButtonSpec filled = resolver.filledButtonSpec(theme);

    QCOMPARE(text.containerColor, textOnlyColor);
    QCOMPARE(
        filled.containerColor,
        theme.colorScheme().color(ColorRole::Primary));
    QVERIFY(filled.containerColor != textOnlyColor);
}

QTEST_MAIN(tst_ButtonSpecResolver)

#include "tst_buttonspecresolver.moc"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
