#include <QtTest/QtTest>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"

class tst_ThemeBuilder : public QObject
{
    Q_OBJECT

private slots:
    void buildsLightTheme();
    void buildsDarkTheme();
    void buildsLightScheme();
    void buildsDarkScheme();
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

QTEST_MAIN(tst_ThemeBuilder)
#include "tst_themebuilder.moc"
