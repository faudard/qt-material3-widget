#include <QtTest/QtTest>

#include "qtmaterial/specs/qtmaterialbuttonspecresolver.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_ButtonResolvedSpec : public QObject
{
    Q_OBJECT

private slots:
    void resolvesRuntimeTokens();
    void resolvesComponentOverrides();
};

void tst_ButtonResolvedSpec::resolvesRuntimeTokens()
{
    ThemeBuilder builder;
    Theme theme =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));

    TypographyStyle typography;
    typography.font = QFont(QStringLiteral("Sans Serif"), 13);
    theme.typography().setStyle(TypeRole::LabelLarge, typography);

    MotionStyle motion;
    motion.durationMs = 245;
    motion.easing = QEasingCurve(QEasingCurve::InOutCubic);
    theme.motion().setStyle(MotionToken::Short4, motion);

    theme.stateLayer().hoverOpacity = 0.07;
    theme.stateLayer().focusOpacity = 0.11;
    theme.stateLayer().pressOpacity = 0.15;
    theme.stateLayer().dragOpacity = 0.19;

    const QColor shadow(QStringLiteral("#123456"));
    theme.colorScheme().setColor(ColorRole::Shadow, shadow);

    const ButtonSpec spec =
        ButtonSpecResolver().filledButtonSpec(theme);

    QVERIFY(spec.hasResolvedLabelFont);
    QCOMPARE(spec.labelFont, typography.font);
    QVERIFY(spec.hasResolvedMotionStyle);
    QCOMPARE(spec.motionStyle.durationMs, 245);
    QCOMPARE(
        static_cast<int>(spec.motionStyle.easing.type()),
        static_cast<int>(QEasingCurve::InOutCubic));
    QCOMPARE(spec.cornerRadius, -1.0);
    QCOMPARE(spec.hoverStateLayerOpacity, 0.07);
    QCOMPARE(spec.focusStateLayerOpacity, 0.11);
    QCOMPARE(spec.pressStateLayerOpacity, 0.15);
    QCOMPARE(spec.dragStateLayerOpacity, 0.19);
    QCOMPARE(spec.shadowColor, shadow);
}

void tst_ButtonResolvedSpec::resolvesComponentOverrides()
{
    ThemeBuilder builder;
    Theme theme =
        builder.buildDarkFromSeed(QColor(QStringLiteral("#6750A4")));

    TypographyStyle typography;
    typography.font = QFont(QStringLiteral("Monospace"), 12);

    MotionStyle motion;
    motion.durationMs = 310;
    motion.easing = QEasingCurve(QEasingCurve::OutQuart);

    ComponentTokenOverride overrideTokens;
    overrideTokens.typography.insert(TypeRole::LabelLarge, typography);
    overrideTokens.shapes.insert(ShapeRole::Small, 9);
    overrideTokens.motion.insert(MotionToken::Medium2, motion);
    overrideTokens.custom.insert(
        QStringLiteral("shapeRole"),
        QStringLiteral("small"));
    overrideTokens.custom.insert(
        QStringLiteral("motionToken"),
        QStringLiteral("medium2"));
    overrideTokens.custom.insert(
        QStringLiteral("shadowColor"),
        QStringLiteral("#345678"));

    overrideTokens.hasStateLayer = true;
    overrideTokens.stateLayer.hoverOpacity = 0.03;
    overrideTokens.stateLayer.focusOpacity = 0.06;
    overrideTokens.stateLayer.pressOpacity = 0.09;
    overrideTokens.stateLayer.dragOpacity = 0.12;

    theme.componentOverrides().setOverride(
        QStringLiteral("button.outlined"),
        overrideTokens);

    const ButtonSpec spec =
        ButtonSpecResolver().outlinedButtonSpec(theme);

    QVERIFY(spec.hasResolvedLabelFont);
    QCOMPARE(spec.labelFont, typography.font);
    QCOMPARE(spec.cornerRadius, 9.0);
    QVERIFY(spec.hasResolvedMotionStyle);
    QCOMPARE(spec.motionStyle.durationMs, 310);
    QCOMPARE(spec.hoverStateLayerOpacity, 0.03);
    QCOMPARE(spec.focusStateLayerOpacity, 0.06);
    QCOMPARE(spec.pressStateLayerOpacity, 0.09);
    QCOMPARE(spec.dragStateLayerOpacity, 0.12);
    QCOMPARE(spec.shadowColor, QColor(QStringLiteral("#345678")));
}

QTEST_MAIN(tst_ButtonResolvedSpec)

#include "tst_buttonresolvedspec.moc"
