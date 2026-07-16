#include <QtTest/QtTest>

#include "qtmaterial/specs/qtmaterialselectionspecresolver.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_SelectionResolvedSpec : public QObject
{
    Q_OBJECT

private slots:
    void resolvesRuntimeTokens();
    void resolvesComponentOverrides();
};

void tst_SelectionResolvedSpec::resolvesRuntimeTokens()
{
    ThemeBuilder builder;
    Theme theme =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));

    TypographyStyle typography;
    typography.font = QFont(QStringLiteral("Sans Serif"), 13);
    theme.typography().setStyle(TypeRole::BodyLarge, typography);

    MotionStyle motion;
    motion.durationMs = 215;
    motion.easing = QEasingCurve(QEasingCurve::InOutCubic);
    theme.motion().setStyle(MotionToken::Short3, motion);

    theme.stateLayer().hoverOpacity = 0.05;
    theme.stateLayer().focusOpacity = 0.09;
    theme.stateLayer().pressOpacity = 0.13;
    theme.stateLayer().dragOpacity = 0.17;

    SelectionSpecResolver resolver;
    const CheckboxSpec checkbox = resolver.checkboxSpec(theme);
    const RadioButtonSpec radio = resolver.radioButtonSpec(theme);
    const SwitchSpec switchSpec = resolver.switchSpec(theme);

    QVERIFY(checkbox.hasResolvedLabelFont);
    QCOMPARE(checkbox.labelFont, typography.font);
    QVERIFY(checkbox.hasResolvedMotionStyle);
    QCOMPARE(checkbox.motionStyle.durationMs, 215);
    QCOMPARE(checkbox.hoverStateLayerOpacity, 0.05);
    QCOMPARE(checkbox.focusStateLayerOpacity, 0.09);
    QCOMPARE(checkbox.pressStateLayerOpacity, 0.13);
    QCOMPARE(checkbox.dragStateLayerOpacity, 0.17);

    QCOMPARE(radio.labelFont, typography.font);
    QCOMPARE(radio.motionStyle.durationMs, 215);
    QCOMPARE(switchSpec.labelFont, typography.font);
    QCOMPARE(switchSpec.motionStyle.durationMs, 215);
}

void tst_SelectionResolvedSpec::resolvesComponentOverrides()
{
    ThemeBuilder builder;
    Theme theme =
        builder.buildDarkFromSeed(QColor(QStringLiteral("#6750A4")));

    TypographyStyle typography;
    typography.font = QFont(QStringLiteral("Monospace"), 12);

    MotionStyle motion;
    motion.durationMs = 320;
    motion.easing = QEasingCurve(QEasingCurve::OutQuart);

    ComponentTokenOverride tokens;
    tokens.typography.insert(TypeRole::BodyLarge, typography);
    tokens.motion.insert(MotionToken::Short3, motion);
    tokens.hasStateLayer = true;
    tokens.stateLayer.hoverOpacity = 0.03;
    tokens.stateLayer.focusOpacity = 0.06;
    tokens.stateLayer.pressOpacity = 0.10;
    tokens.stateLayer.dragOpacity = 0.14;
    tokens.custom.insert(
        QStringLiteral("pressStateLayerOpacity"),
        0.11);

    theme.componentOverrides().setOverride(
        QStringLiteral("checkbox"),
        tokens);
    theme.componentOverrides().setOverride(
        QStringLiteral("radioButton"),
        tokens);
    theme.componentOverrides().setOverride(
        QStringLiteral("switch"),
        tokens);

    SelectionSpecResolver resolver;
    const CheckboxSpec checkbox = resolver.checkboxSpec(theme);
    const RadioButtonSpec radio = resolver.radioButtonSpec(theme);
    const SwitchSpec switchSpec = resolver.switchSpec(theme);

    QCOMPARE(checkbox.labelFont, typography.font);
    QCOMPARE(checkbox.motionStyle.durationMs, 320);
    QCOMPARE(checkbox.hoverStateLayerOpacity, 0.03);
    QCOMPARE(checkbox.focusStateLayerOpacity, 0.06);
    QCOMPARE(checkbox.pressStateLayerOpacity, 0.11);
    QCOMPARE(checkbox.dragStateLayerOpacity, 0.14);

    QCOMPARE(radio.labelFont, typography.font);
    QCOMPARE(radio.motionStyle.durationMs, 320);
    QCOMPARE(radio.pressStateLayerOpacity, 0.11);

    QCOMPARE(switchSpec.labelFont, typography.font);
    QCOMPARE(switchSpec.motionStyle.durationMs, 320);
    QCOMPARE(switchSpec.pressStateLayerOpacity, 0.11);
}

QTEST_MAIN(tst_SelectionResolvedSpec)

#include "tst_selectionresolvedspec.moc"
