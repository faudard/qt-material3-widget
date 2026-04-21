#include <QtTest>

#include "qtmaterial/theme/qtmaterialthememanager.h"

class tst_ThemeManager : public QObject
{
    Q_OBJECT

private slots:
    void emitsThemeChanged();
    void applySeedColorRebuildsTheme();
    void importsThemeJson();
};

void tst_ThemeManager::emitsThemeChanged()
{
    QtMaterial::ThemeManager& manager = QtMaterial::ThemeManager::instance();
    QSignalSpy spy(&manager, &QtMaterial::ThemeManager::themeChanged);
    QVERIFY(spy.isValid());

    QtMaterial::ThemeOptions options;
    options.mode = QtMaterial::ThemeMode::Dark;
    options.sourceColor = QColor("#00639B");
    manager.setThemeOptions(options);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(manager.theme().mode(), QtMaterial::ThemeMode::Dark);
}

void tst_ThemeManager::applySeedColorRebuildsTheme()
{
    QtMaterial::ThemeManager& manager = QtMaterial::ThemeManager::instance();
    QSignalSpy spy(&manager, &QtMaterial::ThemeManager::themeChanged);
    QVERIFY(spy.isValid());

    manager.applySeedColor(QColor("#B3261E"), QtMaterial::ThemeMode::Light);

    QVERIFY(spy.count() >= 1);
    QCOMPARE(manager.theme().mode(), QtMaterial::ThemeMode::Light);
    QCOMPARE(manager.theme().options().sourceColor, QColor("#B3261E"));
    QCOMPARE(manager.theme().colorScheme().color(QtMaterial::ColorRole::Primary), QColor("#B3261E"));
}

void tst_ThemeManager::importsThemeJson()
{
    QtMaterial::ThemeManager& manager = QtMaterial::ThemeManager::instance();
    QSignalSpy spy(&manager, &QtMaterial::ThemeManager::themeChanged);
    QVERIFY(spy.isValid());

    const QByteArray json = R"json(
{
  "formatVersion": 1,
  "options": {
    "sourceColor": "#ff00639b",
    "mode": "dark",
    "contrast": "standard",
    "expressive": false,
    "useMaterialColorUtilities": true
  },
  "stateLayer": {
    "hoverOpacity": 0.21
  }
}
)json";

    QString error;
    QVERIFY2(manager.importThemeJson(json, &error), qPrintable(error));

    QVERIFY(spy.count() >= 1);
    QCOMPARE(manager.theme().mode(), QtMaterial::ThemeMode::Dark);
    QCOMPARE(manager.theme().options().sourceColor, QColor("#ff00639b"));
    QCOMPARE(manager.theme().stateLayer().hoverOpacity, 0.21);
}

QTEST_MAIN(tst_ThemeManager)
#include "tst_thememanager.moc"
