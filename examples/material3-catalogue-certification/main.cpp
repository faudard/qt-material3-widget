#include <QApplication>
#include <QGroupBox>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "qtmaterial/widgets/data/qtmaterialbadge.h"
#include "qtmaterial/widgets/navigation/qtmaterialnavigationbar.h"
#include "qtmaterial/widgets/surfaces/qtmaterialsidesheet.h"
#include "qtmaterial/widgets/surfaces/qtmaterialtooltip.h"

using namespace QtMaterial;

namespace {

QGroupBox* section(
    const QString& title,
    QWidget* child,
    QWidget* parent)
{
    auto* box = new QGroupBox(title, parent);
    auto* layout = new QVBoxLayout(box);
    layout->addWidget(child);
    return box;
}

} // namespace

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle(
        QStringLiteral("QtMaterial3 1.7 catalogue certification"));
    window.resize(920, 720);

    auto* central = new QWidget(&window);
    auto* layout = new QVBoxLayout(central);
    layout->setSpacing(16);

    auto* instructions = new QLabel(
        QStringLiteral(
            "Keyboard-only 1.7 certification fixture. Review Navigation Bar, "
            "Side Sheet, Tooltip and Badge with NVDA, Orca and VoiceOver, "
            "then record only observed results in "
            "docs/components/material3-catalogue-certification-1.7.json."),
        central);
    instructions->setWordWrap(true);
    instructions->setAccessibleName(
        QStringLiteral("1.7 certification instructions"));
    layout->addWidget(instructions);

    auto* navigationBar = new QtMaterialNavigationBar(central);
    navigationBar->setAccessibleName(
        QStringLiteral("1.7 Navigation Bar"));
    navigationBar->addDestination(QStringLiteral("Home"));
    navigationBar->addDestination(QStringLiteral("Search"));
    navigationBar->addDestination(QStringLiteral("Disabled"));
    navigationBar->addDestination(QStringLiteral("Settings"));
    navigationBar->setDestinationEnabled(2, false);
    navigationBar->setCurrentIndex(0);
    layout->addWidget(
        section(QStringLiteral("Navigation Bar"), navigationBar, central));

    auto* openSheet = new QPushButton(
        QStringLiteral("Open Side Sheet"),
        central);
    openSheet->setAccessibleName(
        QStringLiteral("Open 1.7 Side Sheet"));
    layout->addWidget(
        section(QStringLiteral("Side Sheet"), openSheet, central));

    auto* sheet = new QtMaterialSideSheet(central);
    sheet->setTitleText(QStringLiteral("Account details"));
    sheet->setModal(true);
    sheet->setRestoreFocusOnClose(true);

    auto* sheetLayout = new QVBoxLayout(sheet->contentWidget());
    auto* primaryAction = new QPushButton(
        QStringLiteral("Save changes"),
        sheet->contentWidget());
    auto* secondaryAction = new QPushButton(
        QStringLiteral("Cancel"),
        sheet->contentWidget());
    sheetLayout->addWidget(primaryAction);
    sheetLayout->addWidget(secondaryAction);
    sheetLayout->addStretch(1);
    sheet->setInitialFocusWidget(primaryAction);

    QObject::connect(
        openSheet,
        &QPushButton::clicked,
        sheet,
        &QtMaterialSideSheet::open);
    QObject::connect(
        secondaryAction,
        &QPushButton::clicked,
        sheet,
        &QtMaterialSideSheet::closeSheet);

    auto* tooltipTarget = new QPushButton(
        QStringLiteral("Focus for Tooltip"),
        central);
    tooltipTarget->setAccessibleName(
        QStringLiteral("Tooltip certification target"));
    layout->addWidget(
        section(QStringLiteral("Tooltip"), tooltipTarget, central));

    auto* tooltip = new QtMaterialTooltip(&window);
    tooltip->setTargetWidget(tooltipTarget);
    tooltip->setText(
        QStringLiteral("Keyboard accessible supporting information"));
    tooltip->setShowDelay(0);
    tooltip->setPlacement(QtMaterialTooltip::Placement::Below);

    auto* badgeHost = new QWidget(central);
    auto* badgeLayout = new QVBoxLayout(badgeHost);
    auto* badge = new QtMaterialBadge(badgeHost);
    badge->setAccessibleName(QStringLiteral("Notifications badge"));
    badge->setMaximum(99);
    badge->setCount(8);

    auto* increment = new QPushButton(
        QStringLiteral("Increment badge"),
        badgeHost);
    auto* overflow = new QPushButton(
        QStringLiteral("Set overflow count"),
        badgeHost);
    auto* dot = new QPushButton(
        QStringLiteral("Toggle dot mode"),
        badgeHost);
    badgeLayout->addWidget(badge, 0, Qt::AlignLeft);
    badgeLayout->addWidget(increment);
    badgeLayout->addWidget(overflow);
    badgeLayout->addWidget(dot);

    QObject::connect(
        increment,
        &QPushButton::clicked,
        badge,
        [badge]() {
            badge->setDot(false);
            badge->setCount(badge->count() + 1);
        });
    QObject::connect(
        overflow,
        &QPushButton::clicked,
        badge,
        [badge]() {
            badge->setDot(false);
            badge->setCount(120);
        });
    QObject::connect(
        dot,
        &QPushButton::clicked,
        badge,
        [badge]() {
            badge->setDot(!badge->isDot());
        });

    layout->addWidget(
        section(QStringLiteral("Badge"), badgeHost, central));
    layout->addStretch(1);

    window.setCentralWidget(central);
    window.show();

    return app.exec();
}
