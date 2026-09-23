#include <QtTest/QtTest>

#include "qtmaterial/specs/qtmaterialbuttonspecresolver.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_ButtonSpecResolver : public QObject
{
    Q_OBJECT

private slots:
    void variantOverridesDoNotLeak();
    void resolvesAllVariants();
    void resolvesDensity();
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
    QCOMPARE(
        static_cast<int>(elevated.hoverElevationRole),
        static_cast<int>(ElevationRole::Level2));

    QCOMPARE(text.containerColor, QColor(Qt::transparent));
    QCOMPARE(text.disabledContainerColor, QColor(Qt::transparent));
    QCOMPARE(text.labelColor, theme.colorScheme().color(ColorRole::Primary));
    QCOMPARE(text.iconColor, text.labelColor);
    QCOMPARE(text.stateLayerColor, text.labelColor);
    QCOMPARE(
        text.disabledLabelColor.name(QColor::HexRgb),
        theme.colorScheme()
            .color(ColorRole::OnSurfaceVariant)
            .name(QColor::HexRgb));
    QVERIFY(qAbs(text.disabledLabelColor.alphaF() - 0.38) < 0.01);
    QCOMPARE(text.elevationRole, ElevationRole::Level0);
    QCOMPARE(text.hoverElevationRole, ElevationRole::Level0);
    QCOMPARE(outlined.containerColor, QColor(Qt::transparent));
    QCOMPARE(outlined.disabledContainerColor, QColor(Qt::transparent));
    QCOMPARE(
        outlined.labelColor,
        theme.colorScheme().color(ColorRole::OnSurfaceVariant));
    QCOMPARE(outlined.iconColor, outlined.labelColor);
    QCOMPARE(outlined.stateLayerColor, outlined.labelColor);
    QCOMPARE(
        outlined.disabledLabelColor.name(QColor::HexRgb),
        theme.colorScheme()
            .color(ColorRole::OnSurfaceVariant)
            .name(QColor::HexRgb));
    QVERIFY(qAbs(outlined.disabledLabelColor.alphaF() - 0.38) < 0.01);
    QCOMPARE(
        outlined.outlineColor,
        theme.colorScheme().color(ColorRole::OutlineVariant));
    QCOMPARE(
        outlined.disabledOutlineColor.name(QColor::HexRgb),
        theme.colorScheme().color(ColorRole::OutlineVariant).name(QColor::HexRgb));
    QVERIFY(qAbs(outlined.disabledOutlineColor.alphaF() - 0.10) < 0.01);
    QCOMPARE(outlined.outlineWidth, 1.0);
    QCOMPARE(outlined.elevationRole, ElevationRole::Level0);
    QCOMPARE(outlined.hoverElevationRole, ElevationRole::Level0);
    QCOMPARE(
        filled.labelColor,
        theme.colorScheme().color(ColorRole::OnPrimary));
    QCOMPARE(
        tonal.containerColor,
        theme.colorScheme().color(ColorRole::SecondaryContainer));
    QCOMPARE(
        tonal.labelColor,
        theme.colorScheme().color(ColorRole::OnSecondaryContainer));
    QCOMPARE(tonal.iconColor, tonal.labelColor);
    QCOMPARE(tonal.stateLayerColor, tonal.labelColor);
    QCOMPARE(
        tonal.disabledContainerColor.name(QColor::HexRgb),
        theme.colorScheme().color(ColorRole::OnSurface).name(QColor::HexRgb));
    QCOMPARE(
        tonal.disabledLabelColor.name(QColor::HexRgb),
        theme.colorScheme().color(ColorRole::OnSurface).name(QColor::HexRgb));
    QVERIFY(qAbs(tonal.disabledContainerColor.alphaF() - 0.12) < 0.01);
    QVERIFY(qAbs(tonal.disabledLabelColor.alphaF() - 0.38) < 0.01);
    QCOMPARE(tonal.elevationRole, ElevationRole::Level0);
    QCOMPARE(tonal.hoverElevationRole, ElevationRole::Level1);
    QCOMPARE(
        filled.disabledContainerColor.name(QColor::HexRgb),
        theme.colorScheme().color(ColorRole::OnSurface).name(QColor::HexRgb));
    QCOMPARE(
        filled.disabledLabelColor.name(QColor::HexRgb),
        theme.colorScheme().color(ColorRole::OnSurfaceVariant).name(QColor::HexRgb));
    QVERIFY(qAbs(filled.disabledContainerColor.alphaF() - 0.10) < 0.01);
    QVERIFY(qAbs(filled.disabledLabelColor.alphaF() - 0.38) < 0.01);
    QCOMPARE(filled.hoverStateLayerOpacity, 0.08);
    QCOMPARE(filled.focusStateLayerOpacity, 0.10);
    QCOMPARE(filled.pressStateLayerOpacity, 0.10);
    QCOMPARE(
        elevated.containerColor,
        theme.colorScheme().color(ColorRole::SurfaceContainerLow));
    QCOMPARE(elevated.labelColor, theme.colorScheme().color(ColorRole::Primary));
    QCOMPARE(
        elevated.disabledContainerColor.name(QColor::HexRgb),
        theme.colorScheme().color(ColorRole::OnSurface).name(QColor::HexRgb));
    QCOMPARE(
        elevated.disabledLabelColor.name(QColor::HexRgb),
        theme.colorScheme().color(ColorRole::OnSurfaceVariant).name(QColor::HexRgb));
    QVERIFY(qAbs(elevated.disabledContainerColor.alphaF() - 0.10) < 0.01);
    QVERIFY(qAbs(elevated.disabledLabelColor.alphaF() - 0.38) < 0.01);
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
        ComponentId::ButtonText,
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
