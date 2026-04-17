#include <QtTest/QtTest>
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

class benchmark_ThemeSwitch : public QObject
{
    Q_OBJECT
private slots:
    void switchLightDark()
    {
        QtMaterial::ThemeBuilder builder;
        QBENCHMARK {
            auto light = builder.buildLightFromSeed(QColor("#6750A4"));
            auto dark = builder.buildDarkFromSeed(QColor("#6750A4"));
            Q_UNUSED(light);
            Q_UNUSED(dark);
        }
    }
};

QTEST_MAIN(benchmark_ThemeSwitch)
#include "benchmark_theme_switch.moc"
