#include <QtTest/QtTest>

#include "qtmaterial/theme/qtmaterialsystemtheme.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_SystemTheme : public QObject {
    Q_OBJECT
private slots:
    void manualPreferencesResolveToExplicitModes() {
        SystemTheme& system = SystemTheme::instance();

        system.setPreference(ThemeModePreference::Light);
        QCOMPARE(system.effectiveMode(), ThemeMode::Light);

        system.setPreference(ThemeModePreference::Dark);
        QCOMPARE(system.effectiveMode(), ThemeMode::Dark);

        system.setPreference(ThemeModePreference::FollowSystem);
        QVERIFY(system.effectiveMode() == ThemeMode::Light || system.effectiveMode() == ThemeMode::Dark);
    }

    void snapshotIsInternallyConsistent() {
        SystemTheme& system = SystemTheme::instance();
        const SystemThemeSnapshot snapshot = system.snapshot();
        QCOMPARE(snapshot.effectiveMode, system.effectiveMode());
        QCOMPARE(snapshot.highContrast, system.isHighContrastEnabled());
        QCOMPARE(snapshot.effectiveContrast, system.effectiveContrast());
    }

    void platformFontCanBeAppliedToTypography() {
        ThemeBuilder builder;
        ThemeOptions options;
        Theme theme = builder.build(options);
        SystemTheme::instance().applyPlatformFontToTheme(theme);

        QVERIFY(theme.typography().contains(TypeRole::BodyLarge));
        QVERIFY(!theme.typography().style(TypeRole::BodyLarge).font.family().isEmpty());
    }
};

QTEST_MAIN(tst_SystemTheme)
#include "tst_systemtheme.moc"
