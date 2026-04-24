#include <QtTest/QtTest>
#include <QSignalSpy>

#include "qtmaterial/theme/qtmaterialtheme.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/theme/qtmaterialthemeoptions.h"

using namespace QtMaterial;

class tst_ThemeManager : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void setThemeOptions_sameOptions_noSignal();
    void applySeedColor_sameSeed_noSignal();
    void applySeedColor_sameSeedAndMode_noSignal();
    void setTheme_replacesSnapshot_andSignals();
};

void tst_ThemeManager::init()
{
    ThemeOptions options;
    ThemeManager::instance().setThemeOptions(options);
}

void tst_ThemeManager::setThemeOptions_sameOptions_noSignal()
{
    ThemeManager& manager = ThemeManager::instance();
    const ThemeOptions options = manager.options();

    QSignalSpy spy(&manager, &ThemeManager::themeChanged);
    manager.setThemeOptions(options);

    QCOMPARE(spy.count(), 0);
}

void tst_ThemeManager::applySeedColor_sameSeed_noSignal()
{
    ThemeManager& manager = ThemeManager::instance();
    const QColor current = manager.options().sourceColor;

    QSignalSpy spy(&manager, &ThemeManager::themeChanged);
    manager.applySeedColor(current);

    QCOMPARE(spy.count(), 0);
}

void tst_ThemeManager::applySeedColor_sameSeedAndMode_noSignal()
{
    ThemeManager& manager = ThemeManager::instance();
    const ThemeOptions options = manager.options();

    QSignalSpy spy(&manager, &ThemeManager::themeChanged);
    manager.applySeedColor(options.sourceColor, options.mode);

    QCOMPARE(spy.count(), 0);
}

void tst_ThemeManager::setTheme_replacesSnapshot_andSignals()
{
    Theme imported;
    ThemeOptions options;
    options.sourceColor = QColor(QStringLiteral("#146C2E"));
    options.mode = ThemeMode::Dark;
    imported.setOptions(options);
    imported.colorScheme().setColor(ColorRole::Primary, QColor(QStringLiteral("#146C2E")));

    ThemeManager& manager = ThemeManager::instance();
    QSignalSpy spy(&manager, &ThemeManager::themeChanged);

    manager.setTheme(imported);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(manager.theme().options().sourceColor, QColor(QStringLiteral("#146C2E")));
    QCOMPARE(manager.theme().options().mode, ThemeMode::Dark);
    QCOMPARE(manager.theme().colorScheme().color(ColorRole::Primary), QColor(QStringLiteral("#146C2E")));
}

QTEST_MAIN(tst_ThemeManager)
#include "tst_thememanager.moc"
