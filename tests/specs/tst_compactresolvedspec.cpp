#include <QtTest>

#include "qtmaterial/specs/qtmaterialchipspecresolver.h"
#include "qtmaterial/specs/qtmaterialsegmentedbuttonspecresolver.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_CompactResolvedSpec : public QObject
{
    Q_OBJECT

private slots:
    void chipResolvesRuntimeValues();
    void compactOverridesWin();
    void segmentedButtonResolvesRuntimeValues();
};

void tst_CompactResolvedSpec::chipResolvesRuntimeValues()
{
    Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));

    TypographyStyle typography;
    typography.font = QFont(QStringLiteral("Sans Serif"), 13);
    theme.typography().setStyle(TypeRole::LabelLarge, typography);

    MotionStyle motion;
    motion.durationMs = 245;
    motion.easing = QEasingCurve(QEasingCurve::InOutCubic);
    theme.motion().setStyle(MotionToken::Short4, motion);

    theme.shapes().setRadius(ShapeRole::Small, 11);
    theme.stateLayer().hoverOpacity = 0.07;
    theme.stateLayer().focusOpacity = 0.11;
    theme.stateLayer().pressOpacity = 0.15;
    theme.stateLayer().dragOpacity = 0.19;

    const ChipSpec spec =
        ChipSpecResolver().assistChipSpec(theme);

    QVERIFY(spec.hasResolvedLabelFont);
    QCOMPARE(spec.labelFont, typography.font);
    QCOMPARE(spec.cornerRadius, 11.0);
    QVERIFY(spec.hasResolvedMotionStyle);
    QCOMPARE(spec.motionStyle.durationMs, 245);
    QCOMPARE(spec.hoverStateLayerOpacity, 0.07);
    QCOMPARE(spec.focusStateLayerOpacity, 0.11);
    QCOMPARE(spec.pressStateLayerOpacity, 0.15);
    QCOMPARE(spec.dragStateLayerOpacity, 0.19);
}

void tst_CompactResolvedSpec::compactOverridesWin()
{
    Theme theme = ThemeBuilder().buildDarkFromSeed(
        QColor(QStringLiteral("#6750A4")));

    TypographyStyle typography;
    typography.font = QFont(QStringLiteral("Monospace"), 12);

    MotionStyle motion;
    motion.durationMs = 330;
    motion.easing = QEasingCurve(QEasingCurve::OutQuart);

    ComponentTokenOverride tokens;
    tokens.typography.insert(TypeRole::LabelLarge, typography);
    tokens.shapes.insert(ShapeRole::Small, 9);
    tokens.motion.insert(MotionToken::Short4, motion);
    tokens.custom.insert(QStringLiteral("cornerRadius"), 10.0);
    tokens.custom.insert(
        QStringLiteral("selectedContainerColor"),
        QStringLiteral("#345678"));
    tokens.hasStateLayer = true;
    tokens.stateLayer.hoverOpacity = 0.03;
    tokens.stateLayer.focusOpacity = 0.06;
    tokens.stateLayer.pressOpacity = 0.09;
    tokens.stateLayer.dragOpacity = 0.12;

    theme.componentOverrides().setOverride(
        QStringLiteral("chip.assist"),
        tokens);

    const ChipSpec spec =
        ChipSpecResolver().assistChipSpec(theme);

    QCOMPARE(spec.labelFont, typography.font);
    QCOMPARE(spec.cornerRadius, 10.0);
    QCOMPARE(spec.motionStyle.durationMs, 330);
    QCOMPARE(
        spec.selectedContainerColor,
        QColor(QStringLiteral("#345678")));
    QCOMPARE(spec.hoverStateLayerOpacity, 0.03);
    QCOMPARE(spec.focusStateLayerOpacity, 0.06);
    QCOMPARE(spec.pressStateLayerOpacity, 0.09);
    QCOMPARE(spec.dragStateLayerOpacity, 0.12);
}

void tst_CompactResolvedSpec::segmentedButtonResolvesRuntimeValues()
{
    Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#006874")));

    TypographyStyle typography;
    typography.font = QFont(QStringLiteral("Serif"), 14);
    theme.typography().setStyle(TypeRole::LabelLarge, typography);

    const SegmentedButtonSpec spec =
        SegmentedButtonSpecResolver().segmentedButtonSpec(theme);

    QVERIFY(spec.hasResolvedLabelFont);
    QCOMPARE(spec.labelFont, typography.font);
    QCOMPARE(spec.cornerRadius, -1.0);
    QVERIFY(spec.hasResolvedMotionStyle);
    QCOMPARE(
        spec.hoverStateLayerOpacity,
        theme.stateLayer().hoverOpacity);
}

QTEST_MAIN(tst_CompactResolvedSpec)
#include "tst_compactresolvedspec.moc"
