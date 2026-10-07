#include <QtTest/QtTest>

#include <QColor>

#include <memory>
#include <vector>

#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemeoptions.h"

namespace {

constexpr int kControllerCount = 1000;

QtMaterial::Theme expressiveTheme()
{
    QtMaterial::ThemeOptions options;
    options.sourceColor = QColor(QStringLiteral("#6750A4"));
    options.variant = QtMaterial::ThemeVariant::Expressive;
    options.motionScheme = QtMaterial::MotionScheme::Expressive;
    return QtMaterial::ThemeBuilder().build(options);
}

} // namespace

class benchmark_ExpressiveMotionScale : public QObject
{
    Q_OBJECT

private slots:
    void retarget1000Controllers()
    {
        const QtMaterial::Theme theme = expressiveTheme();
        std::vector<std::unique_ptr<QtMaterial::QtMaterialTransitionController>> controllers;
        controllers.reserve(kControllerCount);
        for (int i = 0; i < kControllerCount; ++i) {
            auto controller = std::make_unique<QtMaterial::QtMaterialTransitionController>();
            controller->applyMotionToken(theme, QtMaterial::MotionToken::SpatialDefault);
            controllers.push_back(std::move(controller));
        }

        QBENCHMARK_ONCE {
            for (int pass = 0; pass < 20; ++pass) {
                const qreal target = (pass % 2 == 0) ? 1.0 : 0.0;
                for (auto& controller : controllers) {
                    controller->startTo(target);
                    controller->finish();
                }
            }
        }

        for (const auto& controller : controllers) {
            QVERIFY(!controller->isRunning());
        }
    }
};

QTEST_MAIN(benchmark_ExpressiveMotionScale)
#include "benchmark_expressive_motion.moc"
