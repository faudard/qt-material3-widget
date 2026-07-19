#include <QtTest/QtTest>

#include "qtmaterial/specs/qtmaterialdatefieldspecresolver.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_DateFieldSpecResolver : public QObject
{
    Q_OBJECT

private slots:
    void resolvesRuntimeValues();
    void resolvesComponentOverrides();
    void remainsCompatibleWithSpecFactory();
};

void tst_DateFieldSpecResolver::resolvesRuntimeValues()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));

    const DateFieldSpec spec =
        DateFieldSpecResolver().dateFieldSpec(theme);

    QCOMPARE(
        spec.trailingIconColor,
        theme.colorScheme().color(
            ColorRole::OnSurfaceVariant));
    QCOMPARE(
        spec.placeholderColor,
        theme.colorScheme().color(
            ColorRole::OnSurfaceVariant));
    QCOMPARE(spec.actionIconSize, 20);
    QCOMPARE(spec.actionButtonExtent, 40);
    QVERIFY(spec.calendarVisibleWhenEmpty);
    QVERIFY(spec.preferClearAction);
    QVERIFY(spec.hasResolvedMotionStyle);
    QCOMPARE(
        spec.motionStyle.durationMs,
        theme.motion()
            .style(MotionToken::Short4)
            .durationMs);
    QVERIFY(spec.disabledIconColor.alphaF() < 1.0);
}

void tst_DateFieldSpecResolver::resolvesComponentOverrides()
{
    ThemeBuilder builder;
    Theme theme =
        builder.buildDarkFromSeed(
            QColor(QStringLiteral("#146C2E")));

    MotionStyle motion;
    motion.durationMs = 280;
    motion.easing =
        QEasingCurve(QEasingCurve::OutCubic);

    ComponentTokenOverride tokens;
    tokens.motion.insert(
        MotionToken::Short4,
        motion);
    tokens.custom.insert(
        QStringLiteral("actionIconSize"),
        24);
    tokens.custom.insert(
        QStringLiteral("actionButtonExtent"),
        44);
    tokens.custom.insert(
        QStringLiteral("calendarVisibleWhenEmpty"),
        false);
    tokens.custom.insert(
        QStringLiteral("preferClearAction"),
        false);
    tokens.custom.insert(
        QStringLiteral("clearIconColor"),
        QStringLiteral("#ff0000"));

    theme.componentOverrides().setOverride(
        QStringLiteral("dateField"),
        tokens);

    const DateFieldSpec spec =
        DateFieldSpecResolver().dateFieldSpec(theme);

    QCOMPARE(spec.actionIconSize, 24);
    QCOMPARE(spec.actionButtonExtent, 44);
    QVERIFY(!spec.calendarVisibleWhenEmpty);
    QVERIFY(!spec.preferClearAction);
    QCOMPARE(
        spec.clearIconColor,
        QColor(QStringLiteral("#ff0000")));
    QCOMPARE(spec.motionStyle.durationMs, 280);
}

void tst_DateFieldSpecResolver::remainsCompatibleWithSpecFactory()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));

    const DateFieldSpec resolved =
        DateFieldSpecResolver().dateFieldSpec(theme);
    const DateFieldSpec legacy =
        DateFieldSpecResolver().dateFieldSpec(theme);

    QCOMPARE(
        resolved.trailingIconColor,
        legacy.trailingIconColor);
    QCOMPARE(
        resolved.placeholderColor,
        legacy.placeholderColor);
    QCOMPARE(
        resolved.actionIconSize,
        legacy.actionIconSize);
    QCOMPARE(
        resolved.motionStyle.durationMs,
        legacy.motionStyle.durationMs);
}

QTEST_MAIN(tst_DateFieldSpecResolver)

#include "tst_datefieldspecresolver.moc"
