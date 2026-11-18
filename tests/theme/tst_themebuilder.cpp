#include <QtTest/QtTest>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemevalidation.h"

class tst_ThemeBuilder : public QObject
{
    Q_OBJECT

private slots:
    void buildsLightTheme();
    void buildsDarkTheme();
    void buildsLightScheme();
    void buildsDarkScheme();
    void usesKeyboardOnlyFocusIndicatorsByDefault();
    void fallbackAccentContrastDoesNotRegress();
};

void tst_ThemeBuilder::buildsLightTheme()
{
    QtMaterial::ThemeBuilder builder;
    const QtMaterial::Theme theme = builder.buildLightFromSeed(QColor("#6750A4"));

    QCOMPARE(theme.mode(), QtMaterial::ThemeMode::Light);
    QVERIFY(theme.colorScheme().contains(QtMaterial::ColorRole::Primary));
    QVERIFY(theme.typography().contains(QtMaterial::TypeRole::BodyLarge));
}

void tst_ThemeBuilder::buildsDarkTheme()
{
    QtMaterial::ThemeBuilder builder;
    const QtMaterial::Theme theme = builder.buildDarkFromSeed(QColor("#6750A4"));

    QCOMPARE(theme.mode(), QtMaterial::ThemeMode::Dark);
    QVERIFY(theme.colorScheme().contains(QtMaterial::ColorRole::Surface));
}

void tst_ThemeBuilder::buildsLightScheme()
{
    QtMaterial::ThemeBuilder builder;
    const QColor seed("#6750A4");

    const QtMaterial::ColorScheme scheme = builder.buildLightSchemeFromSeed(seed);
    const QtMaterial::Theme theme = builder.buildLightFromSeed(seed);

    QVERIFY(scheme.contains(QtMaterial::ColorRole::Primary));
    QVERIFY(scheme.contains(QtMaterial::ColorRole::Surface));
    QCOMPARE(scheme.color(QtMaterial::ColorRole::Primary), seed);
    QCOMPARE(scheme.color(QtMaterial::ColorRole::Primary), theme.colorScheme().color(QtMaterial::ColorRole::Primary));
    QCOMPARE(scheme.color(QtMaterial::ColorRole::Surface), theme.colorScheme().color(QtMaterial::ColorRole::Surface));
}

void tst_ThemeBuilder::buildsDarkScheme()
{
    QtMaterial::ThemeBuilder builder;
    const QColor seed("#6750A4");

    const QtMaterial::ColorScheme scheme = builder.buildDarkSchemeFromSeed(seed);
    const QtMaterial::Theme theme = builder.buildDarkFromSeed(seed);

    QVERIFY(scheme.contains(QtMaterial::ColorRole::Primary));
    QVERIFY(scheme.contains(QtMaterial::ColorRole::InversePrimary));
    QCOMPARE(scheme.color(QtMaterial::ColorRole::InversePrimary), seed);
    QCOMPARE(scheme.color(QtMaterial::ColorRole::Primary), theme.colorScheme().color(QtMaterial::ColorRole::Primary));
    QCOMPARE(scheme.color(QtMaterial::ColorRole::Surface), theme.colorScheme().color(QtMaterial::ColorRole::Surface));
}

void tst_ThemeBuilder::usesKeyboardOnlyFocusIndicatorsByDefault()
{
    QtMaterial::ThemeBuilder builder;
    const QtMaterial::Theme theme =
        builder.buildLightFromSeed(QColor("#6750A4"));

    QVERIFY(theme.interactions().keyboardFocusVisible);
    QVERIFY(!theme.interactions().strongFocusIndicators);
}

void tst_ThemeBuilder::fallbackAccentContrastDoesNotRegress()
{
    using namespace QtMaterial;
    const ColorRole pairs[][2] = {
        {ColorRole::OnPrimary, ColorRole::Primary},
        {ColorRole::OnSecondary, ColorRole::Secondary},
        {ColorRole::OnTertiary, ColorRole::Tertiary}
    };
    for (const QColor& seed : {QColor("#6750A4"), QColor("#FF0000"),
                               QColor("#FFFFFF"), QColor("#000000"), QColor("#00639B")}) {
        for (ThemeMode mode : {ThemeMode::Light, ThemeMode::Dark}) {
            ThemeOptions options;
            options.sourceColor = seed;
            options.mode = mode;
            options.backendPolicy = ColorBackendPolicy::ForceFallback;
            options.contrast = ContrastMode::Standard;
            const Theme standard = ThemeBuilder().build(options);
            options.contrast = ContrastMode::High;
            const Theme high = ThemeBuilder().build(options);
            for (const auto& pair : pairs) {
                const auto& a = standard.colorScheme();
                const auto& b = high.colorScheme();
                const double before = colorContrastRatio(a.color(pair[0]), a.color(pair[1]));
                const double after = colorContrastRatio(b.color(pair[0]), b.color(pair[1]));
                QVERIFY(after >= before);
                QVERIFY(after >= 7.0);
            }
        }
    }
}

QTEST_MAIN(tst_ThemeBuilder)
#include "tst_themebuilder.moc"
