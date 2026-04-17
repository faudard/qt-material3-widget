#include <QtTest/QtTest>
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

class tst_ThemeBuilder : public QObject
{
    Q_OBJECT
private slots:
    void buildsLightTheme();
    void buildsDarkTheme();
    void fillsCoreRoles();
};

void tst_ThemeBuilder::buildsLightTheme() {
    QtMaterial::ThemeBuilder builder;
    const QtMaterial::Theme theme = builder.buildLightFromSeed(QColor("#6750A4"));
    QCOMPARE(theme.mode(), QtMaterial::ThemeMode::Light);
    QVERIFY(theme.colorScheme().contains(QtMaterial::ColorRole::Primary));
    QVERIFY(theme.colorScheme().contains(QtMaterial::ColorRole::Surface));
}

void tst_ThemeBuilder::buildsDarkTheme() {
    QtMaterial::ThemeBuilder builder;
    const QtMaterial::Theme theme = builder.buildDarkFromSeed(QColor("#6750A4"));
    QCOMPARE(theme.mode(), QtMaterial::ThemeMode::Dark);
    QVERIFY(theme.colorScheme().contains(QtMaterial::ColorRole::Primary));
    QVERIFY(theme.colorScheme().contains(QtMaterial::ColorRole::Surface));
}

void tst_ThemeBuilder::fillsCoreRoles() {
    QtMaterial::ThemeBuilder builder;
    const QtMaterial::Theme theme = builder.buildLightFromSeed(QColor("#6750A4"));
    QVERIFY(theme.colorScheme().contains(QtMaterial::ColorRole::Primary));
    QVERIFY(theme.colorScheme().contains(QtMaterial::ColorRole::OnPrimary));
    QVERIFY(theme.colorScheme().contains(QtMaterial::ColorRole::Surface));
    QVERIFY(theme.colorScheme().contains(QtMaterial::ColorRole::OnSurface));
    QVERIFY(theme.colorScheme().contains(QtMaterial::ColorRole::Outline));
}

QTEST_MAIN(tst_ThemeBuilder)
#include "tst_themebuilder.moc"
