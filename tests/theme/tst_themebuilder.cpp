#include <QtTest>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"

class tst_ThemeBuilder : public QObject
{
    Q_OBJECT

private slots:
    void buildsLightTheme();
    void buildsDarkTheme();
    void buildsLightSchemeOnly();
    void buildsDarkSchemeOnly();
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

void tst_ThemeBuilder::buildsLightSchemeOnly()
{
    QtMaterial::ThemeBuilder builder;
    const QtMaterial::ColorScheme scheme = builder.buildLightSchemeFromSeed(QColor("#6750A4"));

    QVERIFY(scheme.contains(QtMaterial::ColorRole::Primary));
    QCOMPARE(scheme.color(QtMaterial::ColorRole::Primary), QColor("#6750A4"));
}

void tst_ThemeBuilder::buildsDarkSchemeOnly()
{
    QtMaterial::ThemeBuilder builder;
    const QtMaterial::ColorScheme scheme = builder.buildDarkSchemeFromSeed(QColor("#6750A4"));

    QVERIFY(scheme.contains(QtMaterial::ColorRole::Surface));
    QVERIFY(scheme.contains(QtMaterial::ColorRole::OnSurface));
}

QTEST_MAIN(tst_ThemeBuilder)
#include "tst_themebuilder.moc"
