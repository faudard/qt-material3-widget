#include <QtTest/QtTest>

#include "qtmaterial/theme/qtmaterialaccessibilitytokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "tests/helpers/qtmaterialcontrastassertions.h"

using namespace QtMaterial;

class tst_ThemeAccessibilityTokens : public QObject {
    Q_OBJECT

private slots:
    void contrastMathMatchesKnownExtremes();
    void builderResolvesStandardAccessibilityDefaults();
    void highContrastRaisesAccessibilityThresholds();
    void reducedMotionZerosDurations();
};

void tst_ThemeAccessibilityTokens::contrastMathMatchesKnownExtremes()
{
    QCOMPARE(qRound(contrastRatio(Qt::black, Qt::white) * 10.0) / 10.0, 21.0);
    QVERIFY(contrastRatio(Qt::black, Qt::black) <= 1.01);
    QVERIFY(meetsContrastRatio(Qt::black, Qt::white, 4.5));
}

void tst_ThemeAccessibilityTokens::builderResolvesStandardAccessibilityDefaults()
{
    ThemeOptions options;
    options.sourceColor = QColor(QStringLiteral("#6750A4"));
    options.mode = ThemeMode::Light;
    options.contrast = ContrastMode::Standard;

    const Theme theme = ThemeBuilder().build(options);
    QVERIFY(!theme.accessibility().highContrast);
    QVERIFY(!theme.accessibility().reducedMotion);
    QCOMPARE(theme.accessibility().focusRing.width, 2);
    QCOMPARE(theme.accessibility().minimumTextContrastRatio, 4.5);
    QVERIFY(theme.interactions().keyboardFocusVisible);
}

void tst_ThemeAccessibilityTokens::highContrastRaisesAccessibilityThresholds()
{
    ThemeOptions options;
    options.sourceColor = QColor(QStringLiteral("#6750A4"));
    options.mode = ThemeMode::Light;
    options.contrast = ContrastMode::High;

    const Theme theme = ThemeBuilder().build(options);
    QVERIFY(theme.accessibility().highContrast);
    QVERIFY(theme.accessibility().focusRing.width >= 3);
    QVERIFY(theme.accessibility().minimumTextContrastRatio >= 7.0);
    QVERIFY(theme.accessibility().minimumUiContrastRatio >= 4.5);

    QTM3_COMPARE_CONTRAST_AT_LEAST(
        theme.colorScheme().color(ColorRole::OnSurface),
        theme.colorScheme().color(ColorRole::Surface),
        theme.accessibility().minimumTextContrastRatio);
}

void tst_ThemeAccessibilityTokens::reducedMotionZerosDurations()
{
    Theme theme = ThemeBuilder().build(ThemeOptions{});
    QVERIFY(theme.motion().contains(MotionToken::Medium2));
    QVERIFY(theme.motion().style(MotionToken::Medium2).durationMs > 0);

    applyReducedMotion(&theme.motion(), true);
    QCOMPARE(theme.motion().style(MotionToken::Medium2).durationMs, 0);
    QCOMPARE(theme.motion().style(MotionToken::Long4).durationMs, 0);
}

QTEST_MAIN(tst_ThemeAccessibilityTokens)
#include "tst_theme_accessibility_tokens.moc"
