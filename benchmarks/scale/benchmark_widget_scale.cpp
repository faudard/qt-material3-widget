#include <QtTest/QtTest>

#include <QApplication>
#include <QColor>
#include <QWidget>

#include <vector>

#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"

namespace {

constexpr int kWidgetCount = 1000;

void createButtons(QWidget* parent, std::vector<QtMaterial::QtMaterialFilledButton*>& buttons)
{
    buttons.reserve(kWidgetCount);
    for (int i = 0; i < kWidgetCount; ++i) {
        auto* button = new QtMaterial::QtMaterialFilledButton(
            QStringLiteral("Action %1").arg(i),
            parent);
        buttons.push_back(button);
    }
}

void deleteButtons(std::vector<QtMaterial::QtMaterialFilledButton*>& buttons)
{
    for (auto* button : buttons) {
        delete button;
    }
    buttons.clear();
}

} // namespace

class benchmark_WidgetScale : public QObject
{
    Q_OBJECT

private slots:
    void createAndDestroy1000()
    {
        QBENCHMARK_ONCE {
            QWidget host;
            host.setAttribute(Qt::WA_DontShowOnScreen);
            std::vector<QtMaterial::QtMaterialFilledButton*> buttons;
            createButtons(&host, buttons);
            deleteButtons(buttons);
        }
    }

    void globalThemeSwitch1000Widgets()
    {
        QWidget host;
        host.setAttribute(Qt::WA_DontShowOnScreen);
        std::vector<QtMaterial::QtMaterialFilledButton*> buttons;
        createButtons(&host, buttons);
        host.show();
        QApplication::processEvents();

        auto& manager = QtMaterial::ThemeManager::instance();
        const QtMaterial::Theme original = manager.theme();
        manager.applySeedColor(QColor(QStringLiteral("#6750A4")), QtMaterial::ThemeMode::Light);
        QApplication::processEvents();

        QBENCHMARK_ONCE {
            manager.applySeedColor(QColor(QStringLiteral("#00639B")), QtMaterial::ThemeMode::Dark);
            QApplication::processEvents();
        }

        manager.setTheme(original);
        deleteButtons(buttons);
    }

    void repeatedLifecycle10x1000()
    {
        QBENCHMARK_ONCE {
            for (int cycle = 0; cycle < 10; ++cycle) {
                auto* host = new QWidget;
                host->setAttribute(Qt::WA_DontShowOnScreen);
                std::vector<QtMaterial::QtMaterialFilledButton*> buttons;
                createButtons(host, buttons);
                delete host;
                buttons.clear();
                QApplication::processEvents();
            }
        }
    }
};

QTEST_MAIN(benchmark_WidgetScale)
#include "benchmark_widget_scale.moc"
