#include <QtTest/QtTest>
#include <QColor>

#include "qtmaterial/core/qtmaterialdensity.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

class benchmark_SpecFactoryPipeline : public QObject
{
    Q_OBJECT

private slots:
    void buttonSpecsHotLoop()
    {
        QtMaterial::ThemeBuilder builder;
        const auto theme = builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));
        QtMaterial::SpecFactory factory;

        QBENCHMARK {
            auto text = factory.textButtonSpec(theme, QtMaterial::Density::Default);
            auto filled = factory.filledButtonSpec(theme, QtMaterial::Density::Default);
            auto tonal = factory.filledTonalButtonSpec(theme, QtMaterial::Density::Default);
            auto outlined = factory.outlinedButtonSpec(theme, QtMaterial::Density::Default);
            auto elevated = factory.elevatedButtonSpec(theme, QtMaterial::Density::Default);
            Q_UNUSED(text);
            Q_UNUSED(filled);
            Q_UNUSED(tonal);
            Q_UNUSED(outlined);
            Q_UNUSED(elevated);
        }
    }

    void inputAndSelectionSpecsHotLoop()
    {
        QtMaterial::ThemeBuilder builder;
        const auto theme = builder.buildDarkFromSeed(QColor(QStringLiteral("#00639B")));
        QtMaterial::SpecFactory factory;

        QBENCHMARK {
            auto checkbox = factory.checkboxSpec(theme, QtMaterial::Density::Default);
            auto radio = factory.radioButtonSpec(theme, QtMaterial::Density::Compact);
            auto sw = factory.switchSpec(theme, QtMaterial::Density::Comfortable);
            auto outlined = factory.outlinedTextFieldSpec(theme, QtMaterial::Density::Default);
            auto filled = factory.filledTextFieldSpec(theme, QtMaterial::Density::Default);
            Q_UNUSED(checkbox);
            Q_UNUSED(radio);
            Q_UNUSED(sw);
            Q_UNUSED(outlined);
            Q_UNUSED(filled);
        }
    }

    void surfaceSpecsHotLoop()
    {
        QtMaterial::ThemeBuilder builder;
        const auto theme = builder.buildLightFromSeed(QColor(QStringLiteral("#FF0000")));
        QtMaterial::SpecFactory factory;

        QBENCHMARK {
            auto card = factory.cardSpec(theme);
            auto dialog = factory.dialogSpec(theme);
            auto drawer = factory.navigationDrawerSpec(theme);
            auto sheet = factory.bottomSheetSpec(theme);
            auto banner = factory.bannerSpec(theme);
            Q_UNUSED(card);
            Q_UNUSED(dialog);
            Q_UNUSED(drawer);
            Q_UNUSED(sheet);
            Q_UNUSED(banner);
        }
    }
};

QTEST_MAIN(benchmark_SpecFactoryPipeline)
#include "benchmark_spec_factory_pipeline.moc"
