#include <QtTest/QtTest>
#include <QColor>
#include <QObject>
#include <QVector>

#include "qtmaterial/theme/qtmaterialthememanager.h"

class ThemeSink : public QObject
{
    Q_OBJECT
public slots:
    void onThemeChanged(const QtMaterial::Theme& theme)
    {
        Q_UNUSED(theme);
        ++count;
    }

public:
    int count = 0;
};

class benchmark_ThemeManagerFanout : public QObject
{
    Q_OBJECT

private slots:
    void applyThemeWithManyObservers_data()
    {
        QTest::addColumn<int>("observerCount");
        QTest::newRow("500-observers") << 500;
        QTest::newRow("1000-observers") << 1000;
    }

    void applyThemeWithManyObservers()
    {
        QFETCH(int, observerCount);

        QVector<ThemeSink*> sinks;
        sinks.reserve(observerCount);

        auto& manager = QtMaterial::ThemeManager::instance();
        for (int i = 0; i < observerCount; ++i) {
            auto* sink = new ThemeSink;
            sinks.append(sink);
            QObject::connect(&manager,
                             &QtMaterial::ThemeManager::themeChanged,
                             sink,
                             &ThemeSink::onThemeChanged);
        }

        int step = 0;
        QBENCHMARK {
            const QColor seed = (step++ % 2 == 0)
                ? QColor(QStringLiteral("#6750A4"))
                : QColor(QStringLiteral("#00639B"));
            manager.applySeedColor(seed, QtMaterial::ThemeMode::Light);
        }

        for (ThemeSink* sink : sinks) {
            QVERIFY(sink->count > 0);
        }
        qDeleteAll(sinks);
    }
};

QTEST_MAIN(benchmark_ThemeManagerFanout)
#include "benchmark_theme_manager_fanout.moc"
