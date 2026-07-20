#include <QtTest>

#include "qtmaterial/specs/qtmaterialsurfacespecresolver.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_SnackbarResolvedSpec : public QObject
{
    Q_OBJECT

private slots:
    void resolvesRuntimeValues();
    void componentOverridesWin();
    void clampsBehaviorMetrics();
};

void tst_SnackbarResolvedSpec::resolvesRuntimeValues()
{
    const Theme theme = ThemeBuilder().buildDarkFromSeed(
        QColor(QStringLiteral("#6750A4")));

    const SnackbarSpec spec =
        SurfaceSpecResolver().snackbarSpec(theme);

    QVERIFY(spec.hasResolvedTextFont);
    QVERIFY(spec.hasResolvedActionFont);
    QVERIFY(spec.hasResolvedElevationStyle);
    QVERIFY(spec.hasResolvedEnterMotion);
    QVERIFY(spec.hasResolvedExitMotion);
    QCOMPARE(spec.shortDurationMs, 4000);
    QCOMPARE(spec.longDurationMs, 10000);
    QCOMPARE(spec.minWidth, 200);
    QCOMPARE(spec.slideDistance, 16);
}

void tst_SnackbarResolvedSpec::componentOverridesWin()
{
    Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#006874")));

    ComponentTokenOverride tokens;
    tokens.custom.insert(QStringLiteral("minWidth"), 280);
    tokens.custom.insert(QStringLiteral("labelMinWidth"), 144);
    tokens.custom.insert(QStringLiteral("slideDistance"), 28);
    tokens.custom.insert(QStringLiteral("shortDurationMs"), 650);
    tokens.custom.insert(QStringLiteral("longDurationMs"), 1200);
    tokens.custom.insert(
        QStringLiteral("actionPaddingHorizontal"),
        18);
    tokens.custom.insert(
        QStringLiteral("containerColor"),
        QStringLiteral("#123456"));

    theme.componentOverrides().setOverride(
        QStringLiteral("snackbar"),
        tokens);

    const SnackbarSpec spec =
        SurfaceSpecResolver().snackbarSpec(theme);

    QCOMPARE(spec.minWidth, 280);
    QCOMPARE(spec.labelMinWidth, 144);
    QCOMPARE(spec.slideDistance, 28);
    QCOMPARE(spec.shortDurationMs, 650);
    QCOMPARE(spec.longDurationMs, 1200);
    QCOMPARE(spec.actionPaddingHorizontal, 18);
    QCOMPARE(
        spec.containerColor,
        QColor(QStringLiteral("#123456")));
}

void tst_SnackbarResolvedSpec::clampsBehaviorMetrics()
{
    Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#146C2E")));

    ComponentTokenOverride tokens;
    tokens.custom.insert(QStringLiteral("minWidth"), 20);
    tokens.custom.insert(QStringLiteral("maxWidth"), 10);
    tokens.custom.insert(QStringLiteral("shortDurationMs"), -1);
    tokens.custom.insert(QStringLiteral("longDurationMs"), 0);
    tokens.custom.insert(QStringLiteral("dismissButtonSize"), 4);
    theme.componentOverrides().setOverride(
        QStringLiteral("snackbar"),
        tokens);

    const SnackbarSpec spec =
        SurfaceSpecResolver().snackbarSpec(theme);

    QVERIFY(spec.minWidth >= 80);
    QVERIFY(spec.maxWidth >= spec.minWidth);
    QVERIFY(spec.shortDurationMs >= 1);
    QVERIFY(spec.longDurationMs >= spec.shortDurationMs);
    QVERIFY(spec.dismissButtonSize >= 24);
}

QTEST_MAIN(tst_SnackbarResolvedSpec)
#include "tst_snackbarresolvedspec.moc"
