#include <QtTest/QtTest>

#include "qtmaterial/theme/qtmaterialsystemtheme.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_SystemTheme : public QObject {
    Q_OBJECT
private slots:
    void policyResolutionIsDeterministic_data();
    void policyResolutionIsDeterministic();
    void contrastResolutionIsDeterministic();
    void manualPreferencesResolveToExplicitModes();
    void followSystemAlwaysResolvesConcreteMode();
    void snapshotIsInternallyConsistent();
    void platformFontCanBeAppliedToTypography();
};

void tst_SystemTheme::policyResolutionIsDeterministic_data()
{
    QTest::addColumn<ThemePreference>("preference");
    QTest::addColumn<ThemeMode>("systemMode");
    QTest::addColumn<ThemeMode>("expected");

    QTest::newRow("light-over-dark")
        << ThemePreference::Light << ThemeMode::Dark << ThemeMode::Light;
    QTest::newRow("dark-over-light")
        << ThemePreference::Dark << ThemeMode::Light << ThemeMode::Dark;
    QTest::newRow("follow-light")
        << ThemePreference::FollowSystem << ThemeMode::Light << ThemeMode::Light;
    QTest::newRow("follow-dark")
        << ThemePreference::FollowSystem << ThemeMode::Dark << ThemeMode::Dark;
}

void tst_SystemTheme::policyResolutionIsDeterministic()
{
    QFETCH(ThemePreference, preference);
    QFETCH(ThemeMode, systemMode);
    QFETCH(ThemeMode, expected);
    QCOMPARE(SystemTheme::resolveMode(preference, systemMode), expected);
}

void tst_SystemTheme::contrastResolutionIsDeterministic()
{
    QCOMPARE(SystemTheme::resolveContrast(false), ContrastMode::Standard);
    QCOMPARE(SystemTheme::resolveContrast(true), ContrastMode::High);
}

void tst_SystemTheme::manualPreferencesResolveToExplicitModes()
{
    SystemTheme& system = SystemTheme::instance();

    system.setPreference(ThemePreference::Light);
    QCOMPARE(system.effectiveMode(), ThemeMode::Light);

    system.setPreference(ThemePreference::Dark);
    QCOMPARE(system.effectiveMode(), ThemeMode::Dark);
}

void tst_SystemTheme::followSystemAlwaysResolvesConcreteMode()
{
    SystemTheme& system = SystemTheme::instance();
    system.setPreference(ThemePreference::FollowSystem);
    QVERIFY(system.effectiveMode() == ThemeMode::Light
            || system.effectiveMode() == ThemeMode::Dark);
}

void tst_SystemTheme::snapshotIsInternallyConsistent()
{
    SystemTheme& system = SystemTheme::instance();
    const SystemThemeSnapshot snapshot = system.snapshot();
    QCOMPARE(snapshot.effectiveMode, system.effectiveMode());
    QCOMPARE(snapshot.highContrast, system.isHighContrastEnabled());
    QCOMPARE(snapshot.effectiveContrast, system.effectiveContrast());
    QCOMPARE(
        snapshot.effectiveContrast,
        SystemTheme::resolveContrast(snapshot.highContrast));
}

void tst_SystemTheme::platformFontCanBeAppliedToTypography()
{
    ThemeBuilder builder;
    ThemeOptions options;
    Theme theme = builder.build(options);
    SystemTheme::instance().applyPlatformFontToTheme(theme);

    QVERIFY(theme.typography().contains(TypeRole::BodyLarge));
    QVERIFY(!theme.typography().style(TypeRole::BodyLarge).font.family().isEmpty());
}

QTEST_MAIN(tst_SystemTheme)
#include "tst_systemtheme.moc"
