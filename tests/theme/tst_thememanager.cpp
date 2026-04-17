#include <QtTest/QtTest>
#include "qtmaterial/theme/qtmaterialthememanager.h"

class tst_ThemeManager : public QObject
{
    Q_OBJECT
private slots:
    void emitsThemeChanged();
};

void tst_ThemeManager::emitsThemeChanged() {
    QtMaterial::ThemeManager& manager = QtMaterial::ThemeManager::instance();
    QSignalSpy spy(&manager, &QtMaterial::ThemeManager::themeChanged);
    QVERIFY(spy.isValid());

    QtMaterial::ThemeOptions options;
    options.sourceColor = QColor("#00639B");
    options.mode = QtMaterial::ThemeMode::Dark;

    manager.setThemeOptions(options);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(manager.theme().mode(), QtMaterial::ThemeMode::Dark);
}

QTEST_MAIN(tst_ThemeManager)
#include "tst_thememanager.moc"
