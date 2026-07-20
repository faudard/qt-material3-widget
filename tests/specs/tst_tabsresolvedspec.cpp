#include <QtTest>

#include "qtmaterial/specs/qtmaterialtabsspecresolver.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_TabsResolvedSpec : public QObject
{
    Q_OBJECT

private slots:
    void resolvesRuntimeValues();
    void componentOverridesWin();
    void clampsRenderingMetrics();
};

void tst_TabsResolvedSpec::resolvesRuntimeValues()
{
    const Theme theme = ThemeBuilder().buildDarkFromSeed(
        QColor(QStringLiteral("#6750A4")));

    const TabsSpec spec =
        TabsSpecResolver().resolve(theme);

    QVERIFY(spec.hasResolvedLabelFont);
    QVERIFY(spec.hasResolvedBadgeFont);
    QVERIFY(spec.hasResolvedIndicatorMotion);
    QCOMPARE(
        spec.labelFont,
        theme.typography().style(
            TypeRole::LabelLarge).font);
    QCOMPARE(
        spec.badgeFont,
        theme.typography().style(
            TypeRole::LabelSmall).font);
    QVERIFY(spec.stateLayerRadius >= 0.0);
    QVERIFY(spec.focusRingWidth >= 1.0);
    QCOMPARE(spec.indicatorHorizontalInset, 10);
}

void tst_TabsResolvedSpec::componentOverridesWin()
{
    Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#006874")));

    MotionStyle motion;
    motion.durationMs = 275;
    motion.easing =
        QEasingCurve(QEasingCurve::OutQuart);

    ComponentTokenOverride tokens;
    tokens.motion.insert(MotionToken::Short4, motion);
    tokens.custom.insert(
        QStringLiteral("stateLayerInset"), 7);
    tokens.custom.insert(
        QStringLiteral("stateLayerRadius"), 19.0);
    tokens.custom.insert(
        QStringLiteral("focusRingWidth"), 3.0);
    tokens.custom.insert(
        QStringLiteral("indicatorHorizontalInset"), 14);
    tokens.custom.insert(
        QStringLiteral("overflowButtonPadding"), 9);
    tokens.custom.insert(
        QStringLiteral("badgeEndInset"), 11);
    tokens.custom.insert(
        QStringLiteral("containerColor"),
        QStringLiteral("#123456"));

    theme.componentOverrides().setOverride(
        QStringLiteral("tabs"),
        tokens);

    const TabsSpec spec =
        TabsSpecResolver().resolve(theme);

    QCOMPARE(spec.stateLayerInset, 7);
    QCOMPARE(spec.stateLayerRadius, 19.0);
    QCOMPARE(spec.focusRingWidth, 3.0);
    QCOMPARE(spec.indicatorHorizontalInset, 14);
    QCOMPARE(spec.overflowButtonPadding, 9);
    QCOMPARE(spec.badgeEndInset, 11);
    QCOMPARE(spec.animationDuration, 275);
    QCOMPARE(
        spec.indicatorMotionStyle.easing.type(),
        QEasingCurve::OutQuart);
    QCOMPARE(
        spec.containerColor,
        QColor(QStringLiteral("#123456")));
}

void tst_TabsResolvedSpec::clampsRenderingMetrics()
{
    Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#146C2E")));

    ComponentTokenOverride tokens;
    tokens.custom.insert(
        QStringLiteral("containerHeight"), 12);
    tokens.custom.insert(
        QStringLiteral("compactContainerHeight"), 80);
    tokens.custom.insert(
        QStringLiteral("minimumTabWidth"), 100);
    tokens.custom.insert(
        QStringLiteral("maximumTabWidth"), 20);
    tokens.custom.insert(
        QStringLiteral("focusRingWidth"), 0.1);
    tokens.custom.insert(
        QStringLiteral("animationDuration"), -2);
    theme.componentOverrides().setOverride(
        QStringLiteral("tabs"),
        tokens);

    const TabsSpec spec =
        TabsSpecResolver().resolve(theme);

    QVERIFY(spec.containerHeight >= 32);
    QVERIFY(
        spec.compactContainerHeight
        <= spec.containerHeight);
    QVERIFY(
        spec.maximumTabWidth
        >= spec.minimumTabWidth);
    QVERIFY(spec.focusRingWidth >= 1.0);
    QVERIFY(spec.animationDuration >= 0);
}

QTEST_MAIN(tst_TabsResolvedSpec)
#include "tst_tabsresolvedspec.moc"
