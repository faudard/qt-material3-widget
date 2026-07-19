#include <QtTest>

#include <QColor>

#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/specs/qtmaterialbuttonspecresolver.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class benchmark_ResolverPipeline : public QObject
{
    Q_OBJECT

private slots:
    void resolvesFilledButtonSpec();
};

void benchmark_ResolverPipeline::resolvesFilledButtonSpec()
{
    const Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    const ButtonSpecResolver resolver;
    ButtonSpec result;

    QBENCHMARK {
        result = resolver.filledButtonSpec(
            theme,
            Density::Default);
    }

    QVERIFY(result.containerHeight > 0);
}

QTEST_MAIN(benchmark_ResolverPipeline)
#include "benchmark_spec_factory_pipeline.moc"
