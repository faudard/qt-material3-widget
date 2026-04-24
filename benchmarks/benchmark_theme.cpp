#include <QtTest/QtTest>

#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemeoptions.h"

using namespace QtMaterial;

class benchmark_Theme : public QObject
{
    Q_OBJECT

private slots:
    void build_defaultTheme();
    void manager_applySeedColor();
    void manager_toggleMode();
};

void benchmark_Theme::build_defaultTheme()
{
    ThemeBuilder builder;
    ThemeOptions options;

    QBENCHMARK {
        const Theme theme = builder.build(options);
        Q_UNUSED(theme);
    }
}

void benchmark_Theme::manager_applySeedColor()
{
    ThemeManager& manager = ThemeManager::instance();
    manager.setThemeOptions(ThemeOptions {});

    int i = 0;
    QBENCHMARK {
        const QColor color = QColor::fromHsv((i * 17) % 360, 180, 180);
        manager.applySeedColor(color);
        ++i;
    }
}

void benchmark_Theme::manager_toggleMode()
{
    ThemeManager& manager = ThemeManager::instance();
    ThemeOptions options = manager.options();

    int i = 0;
    QBENCHMARK {
        options.mode = (i % 2 == 0) ? ThemeMode::Light : ThemeMode::Dark;
        manager.setThemeOptions(options);
        ++i;
    }
}

QTEST_MAIN(benchmark_Theme)
#include "benchmark_theme.moc"
