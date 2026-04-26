#include <QtTest/QtTest>
#include <QByteArray>
#include <QColor>
#include <QJsonDocument>
#include <QVector>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemeserializer.h"

class benchmark_ThemePipeline : public QObject
{
    Q_OBJECT

private slots:
    void buildLightThemeFromSeed()
    {
        QtMaterial::ThemeBuilder builder;
        QtMaterial::ThemeOptions options;
        options.sourceColor = QColor(QStringLiteral("#6750A4"));
        options.mode = QtMaterial::ThemeMode::Light;
        options.contrast = QtMaterial::ContrastMode::Standard;

        QBENCHMARK {
            const auto theme = builder.build(options);
            Q_UNUSED(theme);
        }
    }

    void buildDarkHighContrastThemeFromSeed()
    {
        QtMaterial::ThemeBuilder builder;
        QtMaterial::ThemeOptions options;
        options.sourceColor = QColor(QStringLiteral("#00639B"));
        options.mode = QtMaterial::ThemeMode::Dark;
        options.contrast = QtMaterial::ContrastMode::High;

        QBENCHMARK {
            const auto theme = builder.build(options);
            Q_UNUSED(theme);
        }
    }

    void buildSeedMatrix()
    {
        QtMaterial::ThemeBuilder builder;
        const QVector<QColor> seeds {
            QColor(QStringLiteral("#6750A4")),
            QColor(QStringLiteral("#00639B")),
            QColor(QStringLiteral("#FF0000")),
            QColor(QStringLiteral("#0B8043")),
            QColor(QStringLiteral("#F9AB00"))
        };

        QBENCHMARK {
            for (const QColor& seed : seeds) {
                auto light = builder.buildLightFromSeed(seed);
                auto dark = builder.buildDarkFromSeed(seed);
                Q_UNUSED(light);
                Q_UNUSED(dark);
            }
        }
    }

    void serializeResolvedTheme()
    {
        QtMaterial::ThemeBuilder builder;
        const auto theme = builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));

        QBENCHMARK {
            const QByteArray json = QtMaterial::ThemeSerializer::toJson(
                theme,
                QJsonDocument::Compact);
            Q_UNUSED(json);
        }
    }

    void deserializeResolvedTheme()
    {
        QtMaterial::ThemeBuilder builder;
        const auto theme = builder.buildDarkFromSeed(QColor(QStringLiteral("#6750A4")));
        const QByteArray json = QtMaterial::ThemeSerializer::toJson(
            theme,
            QJsonDocument::Compact);

        QBENCHMARK {
            bool ok = false;
            QString error;
            const auto parsed = QtMaterial::ThemeSerializer::fromJson(json, &ok, &error);
            QVERIFY2(ok, qPrintable(error));
            Q_UNUSED(parsed);
        }
    }
};

QTEST_MAIN(benchmark_ThemePipeline)
#include "benchmark_theme_pipeline.moc"
