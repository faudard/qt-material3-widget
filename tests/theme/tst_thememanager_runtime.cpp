#include <QtTest/QtTest>
#include <QSignalSpy>

#include "qtmaterial/theme/qtmaterialthemeobserver.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"

using namespace QtMaterial;

class RecordingObserver : public ThemeObserver {
    Q_OBJECT
public:
    explicit RecordingObserver(QObject* parent = nullptr)
        : ThemeObserver(parent)
        , count(0)
    {
    }

    int count;

protected slots:
    void onThemeChanged(const Theme&) override
    {
        ++count;
    }
};

class tst_ThemeManagerRuntime : public QObject {
    Q_OBJECT
private slots:
    void initTestCase()
    {
        qRegisterMetaType<QtMaterial::Theme>("QtMaterial::Theme");
        qRegisterMetaType<QtMaterial::ThemeChangeReason>("QtMaterial::ThemeChangeReason");
    }

    void setThemeOptionsDoesNotEmitForIdenticalOptions()
    {
        ThemeManager& manager = ThemeManager::instance();

        ThemeOptions options;
        options.sourceColor = QColor(QStringLiteral("#6750A4"));
        options.mode = ThemeMode::Light;
        options.contrast = ContrastMode::Standard;
        manager.setThemeOptions(options);

        QSignalSpy changedSpy(&manager, &ThemeManager::themeChanged);
        QSignalSpy reasonSpy(&manager, &ThemeManager::themeChangedWithReason);
        QSignalSpy repolishSpy(&manager, &ThemeManager::themeRepolishRequested);
        const quint64 beforeRevision = manager.revision();

        QVERIFY(!manager.setThemeOptions(options));
        QCOMPARE(manager.revision(), beforeRevision);
        QCOMPARE(changedSpy.count(), 0);
        QCOMPARE(reasonSpy.count(), 0);
        QCOMPARE(repolishSpy.count(), 0);
    }

    void seedColorChangeEmitsOneRuntimeApplyCycle()
    {
        ThemeManager& manager = ThemeManager::instance();

        ThemeOptions options;
        options.sourceColor = QColor(QStringLiteral("#6750A4"));
        options.mode = ThemeMode::Light;
        options.contrast = ContrastMode::Standard;
        manager.setThemeOptions(options);

        QSignalSpy changedSpy(&manager, &ThemeManager::themeChanged);
        QSignalSpy reasonSpy(&manager, &ThemeManager::themeChangedWithReason);
        QSignalSpy repolishSpy(&manager, &ThemeManager::themeRepolishRequested);
        const quint64 beforeRevision = manager.revision();

        QVERIFY(manager.applySeedColor(QColor(QStringLiteral("#00639B"))));
        QCOMPARE(manager.revision(), beforeRevision + 1);
        QCOMPARE(changedSpy.count(), 1);
        QCOMPARE(reasonSpy.count(), 1);
        QCOMPARE(repolishSpy.count(), 1);
        QCOMPARE(manager.options().sourceColor, QColor(QStringLiteral("#00639B")));
    }

    void importFailureDoesNotEmitOrAdvanceRevision()
    {
        ThemeManager& manager = ThemeManager::instance();

        QSignalSpy changedSpy(&manager, &ThemeManager::themeChanged);
        QSignalSpy reasonSpy(&manager, &ThemeManager::themeChangedWithReason);
        QSignalSpy repolishSpy(&manager, &ThemeManager::themeRepolishRequested);
        const quint64 beforeRevision = manager.revision();

        QString error;
        QVERIFY(!manager.importThemeJson(QByteArray("{ invalid json"), &error, ThemeReadMode::Strict));
        QVERIFY(!error.isEmpty());
        QCOMPARE(manager.revision(), beforeRevision);
        QCOMPARE(changedSpy.count(), 0);
        QCOMPARE(reasonSpy.count(), 0);
        QCOMPARE(repolishSpy.count(), 0);
    }

    void observerCanDisableAndManuallySync()
    {
        ThemeManager& manager = ThemeManager::instance();

        RecordingObserver observer;
        QCOMPARE(observer.count, 0);

        observer.syncTheme();
        QCOMPARE(observer.count, 1);

        observer.setThemeObservationEnabled(false);
        QVERIFY(!observer.isThemeObservationEnabled());
        manager.applySeedColor(QColor(QStringLiteral("#FF0000")));
        QCOMPARE(observer.count, 1);

        observer.setThemeObservationEnabled(true);
        QVERIFY(observer.isThemeObservationEnabled());
        manager.applySeedColor(QColor(QStringLiteral("#00AA55")));
        QCOMPARE(observer.count, 2);
    }

    void settingSameResolvedThemeDoesNotEmit()
    {
        ThemeManager& manager = ThemeManager::instance();
        const Theme current = manager.theme();

        QSignalSpy changedSpy(&manager, &ThemeManager::themeChanged);
        QSignalSpy reasonSpy(&manager, &ThemeManager::themeChangedWithReason);
        const quint64 beforeRevision = manager.revision();

        QVERIFY(!manager.setTheme(current));
        QCOMPARE(manager.revision(), beforeRevision);
        QCOMPARE(changedSpy.count(), 0);
        QCOMPARE(reasonSpy.count(), 0);
    }
};

QTEST_MAIN(tst_ThemeManagerRuntime)
#include "tst_thememanager_runtime.moc"
