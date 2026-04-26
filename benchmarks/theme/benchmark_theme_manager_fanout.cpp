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
    void applyThemeWithManyObservers()
    {
        constexpr int kObserverCount = 250;
        QVector<ThemeSink*> sinks;
        sinks.reserve(kObserverCount);

        auto& manager = QtMaterial::ThemeManager::instance();
        for (int i = 0; i < kObserverCount; ++i) {
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

        qDeleteAll(sinks);
    }
};

QTEST_MAIN(benchmark_ThemeManagerFanout)
#include "benchmark_theme_manager_fanout.moc"
