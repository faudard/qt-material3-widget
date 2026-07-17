#include <QtTest/QtTest>

#include "qtmaterial/specs/qtmaterialprogressspecresolver.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_ProgressSpecResolver : public QObject
{
    Q_OBJECT

private slots:
    void resolvesThemeDefaults();
    void preservesExplicitOverrides();
    void resolvesShapeRadius();
    void returnsIndependentValues();
};

void tst_ProgressSpecResolver::resolvesThemeDefaults()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));

    const ProgressIndicatorSpec resolved =
        ProgressSpecResolver().resolve(theme);

    QCOMPARE(
        resolved.activeColor,
        theme.colorScheme().color(ColorRole::Primary));
    QCOMPARE(
        resolved.trackColor,
        theme.colorScheme().color(ColorRole::SurfaceVariant));
    QCOMPARE(resolved.stopIndicatorColor, resolved.activeColor);
    QCOMPARE(resolved.cornerRadius, -1.0);
}

void tst_ProgressSpecResolver::preservesExplicitOverrides()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildDarkFromSeed(QColor(QStringLiteral("#146C2E")));

    ProgressIndicatorSpec requested;
    requested.activeColor = QColor(QStringLiteral("#112233"));
    requested.trackColor = QColor(QStringLiteral("#445566"));
    requested.stopIndicatorColor = QColor(QStringLiteral("#778899"));
    requested.linearHeight = 9;
    requested.circularStrokeWidth = 7;
    requested.animationDurationMs = 1750;
    requested.cornerRadius = 3.5;

    const ProgressIndicatorSpec resolved =
        ProgressSpecResolver().resolve(theme, requested);

    QCOMPARE(resolved.activeColor, requested.activeColor);
    QCOMPARE(resolved.trackColor, requested.trackColor);
    QCOMPARE(
        resolved.stopIndicatorColor,
        requested.stopIndicatorColor);
    QCOMPARE(resolved.linearHeight, 9);
    QCOMPARE(resolved.circularStrokeWidth, 7);
    QCOMPARE(resolved.animationDurationMs, 1750);
    QCOMPARE(resolved.cornerRadius, 3.5);
}

void tst_ProgressSpecResolver::resolvesShapeRadius()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));

    ProgressIndicatorSpec requested;
    requested.shapeRole = ShapeRole::Small;
    requested.cornerRadius = -1.0;

    const ProgressIndicatorSpec resolved =
        ProgressSpecResolver().resolve(theme, requested);

    QCOMPARE(
        resolved.cornerRadius,
        qreal(theme.shapes().radius(ShapeRole::Small)));
}

void tst_ProgressSpecResolver::returnsIndependentValues()
{
    ThemeBuilder builder;
    const Theme firstTheme =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));
    const Theme secondTheme =
        builder.buildDarkFromSeed(QColor(QStringLiteral("#146C2E")));

    ProgressSpecResolver resolver;
    ProgressIndicatorSpec first = resolver.resolve(firstTheme);
    const QColor firstActive = first.activeColor;
    const ProgressIndicatorSpec second = resolver.resolve(secondTheme);

    QCOMPARE(first.activeColor, firstActive);
    QVERIFY(first.activeColor != second.activeColor);
}

QTEST_MAIN(tst_ProgressSpecResolver)

#include "tst_progressspecresolver.moc"
