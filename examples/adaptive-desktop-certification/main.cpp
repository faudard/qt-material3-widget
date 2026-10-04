#include <QApplication>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/foundation/qtmaterialwindowsizeclass.h"
#include "qtmaterial/widgets/layouts/qtmaterialadaptiveshell.h"
#include "qtmaterial/widgets/navigation/qtmaterialnavigationsuite.h"

using namespace QtMaterial;

namespace {

QString widthClassName(WindowWidthSizeClass sizeClass)
{
    switch (sizeClass) {
    case WindowWidthSizeClass::Compact:
        return QStringLiteral("Compact");
    case WindowWidthSizeClass::Medium:
        return QStringLiteral("Medium");
    case WindowWidthSizeClass::Expanded:
        return QStringLiteral("Expanded");
    case WindowWidthSizeClass::Large:
        return QStringLiteral("Large");
    case WindowWidthSizeClass::ExtraLarge:
        return QStringLiteral("ExtraLarge");
    }
    return QStringLiteral("Unknown");
}

QString navigationTypeName(NavigationSuiteType type)
{
    return type == NavigationSuiteType::NavigationBar
        ? QStringLiteral("Navigation Bar")
        : QStringLiteral("Navigation Rail");
}

QString densityName(Density density)
{
    switch (density) {
    case Density::Default:
        return QStringLiteral("Default");
    case Density::Comfortable:
        return QStringLiteral("Comfortable");
    case Density::Compact:
        return QStringLiteral("Compact");
    }
    return QStringLiteral("Unknown");
}

} // namespace

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle(
        QStringLiteral("QtMaterial3 1.9 Adaptive Desktop certification"));

    auto* shell = new QtMaterialAdaptiveShell(&window);
    shell->setAccessibleName(
        QStringLiteral("1.9 Adaptive Desktop shell"));

    auto* navigation = shell->navigationSuite();
    navigation->setAccessibleName(
        QStringLiteral("1.9 responsive navigation"));
    navigation->addDestination(QStringLiteral("Home"));
    navigation->addDestination(QStringLiteral("Search"));
    navigation->addDestination(QStringLiteral("Disabled"));
    navigation->addDestination(QStringLiteral("Settings"));
    navigation->setDestinationEnabled(2, false);
    navigation->setCurrentIndex(0);

    auto* content = new QWidget;
    content->setAccessibleName(QStringLiteral("Main adaptive content"));
    auto* contentLayout = new QVBoxLayout(content);

    auto* instructions = new QLabel(
        QStringLiteral(
            "Use only the keyboard and a screen reader. Move between the five "
            "width classes, verify Navigation Bar/Rail continuity, keep focus "
            "on content controls during resize, then repeat in RTL."),
        content);
    instructions->setWordWrap(true);
    instructions->setAccessibleName(
        QStringLiteral("Adaptive Desktop certification instructions"));
    contentLayout->addWidget(instructions);

    auto* stateLabel = new QLabel(content);
    stateLabel->setAccessibleName(
        QStringLiteral("Current adaptive state"));
    contentLayout->addWidget(stateLabel);

    auto* focusField = new QLineEdit(content);
    focusField->setPlaceholderText(
        QStringLiteral("Focusable main-content field"));
    focusField->setAccessibleName(
        QStringLiteral("Main content focus preservation field"));
    contentLayout->addWidget(focusField);

    auto* sizes = new QWidget(content);
    auto* sizeLayout = new QHBoxLayout(sizes);
    sizeLayout->setContentsMargins(0, 0, 0, 0);

    struct WidthButton {
        const char* label;
        int width;
    };
    const WidthButton widthButtons[] = {
        {"Compact 520", 520},
        {"Medium 720", 720},
        {"Expanded 1000", 1000},
        {"Large 1280", 1280},
        {"ExtraLarge 1640", 1640},
    };

    for (const WidthButton& entry : widthButtons) {
        auto* button = new QPushButton(
            QString::fromLatin1(entry.label),
            sizes);
        button->setAccessibleName(
            QStringLiteral("Resize to %1").arg(
                QString::fromLatin1(entry.label)));
        QObject::connect(
            button,
            &QPushButton::clicked,
            &window,
            [&window, entry]() {
                window.resize(entry.width, 720);
            });
        sizeLayout->addWidget(button);
    }
    contentLayout->addWidget(sizes);

    auto* rtl = new QCheckBox(
        QStringLiteral("Right-to-left layout"),
        content);
    rtl->setAccessibleName(
        QStringLiteral("Toggle right-to-left Adaptive Desktop layout"));
    contentLayout->addWidget(rtl);

    auto* automaticDensity = new QCheckBox(
        QStringLiteral("Automatic desktop density"),
        content);
    automaticDensity->setChecked(true);
    automaticDensity->setAccessibleName(
        QStringLiteral("Automatic desktop density"));
    contentLayout->addWidget(automaticDensity);
    contentLayout->addStretch(1);

    auto* supporting = new QWidget;
    supporting->setAccessibleName(
        QStringLiteral("Adaptive supporting pane"));
    auto* supportingLayout = new QVBoxLayout(supporting);
    auto* supportingTitle = new QLabel(
        QStringLiteral("Supporting pane"),
        supporting);
    supportingTitle->setAccessibleName(
        QStringLiteral("Supporting pane title"));
    auto* supportingAction = new QPushButton(
        QStringLiteral("Supporting action"),
        supporting);
    supportingAction->setAccessibleName(
        QStringLiteral("Supporting pane action"));
    supportingLayout->addWidget(supportingTitle);
    supportingLayout->addWidget(supportingAction);
    supportingLayout->addStretch(1);

    shell->setContentWidget(content);
    shell->setSupportingWidget(supporting);
    shell->setSupportingPaneWidth(360);

    QObject::connect(
        rtl,
        &QCheckBox::toggled,
        shell,
        [shell](bool enabled) {
            shell->setLayoutDirection(
                enabled ? Qt::RightToLeft : Qt::LeftToRight);
        });
    QObject::connect(
        automaticDensity,
        &QCheckBox::toggled,
        shell,
        &QtMaterialAdaptiveShell::setAutomaticDensity);

    const auto refreshState = [shell, navigation, stateLabel]() {
        const QString text =
            QStringLiteral(
                "%1; %2; density %3; supporting pane %4")
                .arg(
                    widthClassName(shell->windowSizeClass().width),
                    navigationTypeName(navigation->navigationType()),
                    densityName(shell->resolvedDensity()),
                    shell->isSupportingPaneVisible()
                        ? QStringLiteral("visible")
                        : QStringLiteral("hidden"));
        stateLabel->setText(text);
        stateLabel->setAccessibleDescription(text);
    };

    QObject::connect(
        shell,
        &QtMaterialAdaptiveShell::widthSizeClassChanged,
        shell,
        [refreshState](WindowWidthSizeClass) {
            refreshState();
        });
    QObject::connect(
        shell,
        &QtMaterialAdaptiveShell::resolvedDensityChanged,
        shell,
        [refreshState](Density) {
            refreshState();
        });
    QObject::connect(
        shell,
        &QtMaterialAdaptiveShell::supportingPaneVisibleChanged,
        shell,
        [refreshState](bool) {
            refreshState();
        });
    QObject::connect(
        navigation,
        &QtMaterialNavigationSuite::navigationTypeChanged,
        shell,
        [refreshState](NavigationSuiteType) {
            refreshState();
        });

    window.setCentralWidget(shell);
    window.resize(1000, 720);
    window.show();
    refreshState();

    return app.exec();
}
