#include <QtTest/QtTest>

#include "qtmaterial/theme/qtmaterialsystemtheme.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_SystemThemeAccessibility : public QObject {
    Q_OBJECT

private slots:
    void exposesHighContrastAvailabilitySafely();
    void appliesAccessibilityPreferencesWithoutMutatingSourceOptions();
};

void tst_SystemThemeAccessibility::exposesHighContrastAvailabilitySafely()
{
    SystemTheme& system = SystemTheme::instance();
    QVERIFY(system.isHighContrastAvailable() || !system.isHighContrastAvailable());
    QVERIFY(system.isHighContrastEnabled() || !system.isHighContrastEnabled());
    QVERIFY(system.isReducedMotionEnabled() || !system.isReducedMotionEnabled());
}

void tst_SystemThemeAccessibility::appliesAccessibilityPreferencesWithoutMutatingSourceOptions()
{
    ThemeOptions options;
    options.sourceColor = QColor(QStringLiteral("#00639B"));
    options.mode = ThemeMode::Light;
    options.contrast = ContrastMode::Standard;

    Theme theme = ThemeBuilder().build(options);
    const ThemeOptions before = theme.options();
    SystemTheme::instance().applyAccessibilityPreferencesToTheme(theme);
    QCOMPARE(theme.options(), before);
    QVERIFY(theme.accessibility().minimumTextContrastRatio >= 4.5);
    QVERIFY(theme.accessibility().minimumUiContrastRatio >= 3.0);
}

QTEST_MAIN(tst_SystemThemeAccessibility)
#include "tst_systemtheme_accessibility.moc"
