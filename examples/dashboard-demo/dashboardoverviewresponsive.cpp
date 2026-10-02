#include "dashboardoverviewresponsive.h"

#include <QBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QWidget>

#include "qtmaterial/widgets/data/qtmaterialtable.h"
#include "qtmaterial/widgets/inputs/qtmaterialcombobox.h"
#include "qtmaterial/widgets/selection/qtmaterialchip.h"
#include "qtmaterial/widgets/surfaces/qtmaterialcard.h"

namespace {

void clearGridPosition(QGridLayout* layout, QWidget* widget)
{
    if (layout && widget) {
        layout->removeWidget(widget);
    }
}

} // namespace

namespace DashboardOverviewResponsive {

void apply(
    QWidget* contentHost,
    QWidget* revenueSummary,
    QtMaterial::QtMaterialCard* statisticsCard,
    QtMaterial::QtMaterialComboBox* yearCombo,
    QtMaterial::QtMaterialComboBox* monthCombo,
    QtMaterial::QtMaterialTable* orders,
    int availableWidth)
{
    if (!contentHost) {
        return;
    }

    const bool stackedQuickSummary = availableWidth < 900;
    const bool stackedHighlights = availableWidth < 980;
    const int popularAppsColumns =
        availableWidth < 620 ? 1 : (availableWidth < 980 ? 2 : 3);
    const bool compactProjectRows = availableWidth < 760;
    const bool veryCompactProjectRows = availableWidth < 560;
    const bool compactStatisticsHeader = availableWidth < 620;
    const bool stackedTasksContent = availableWidth < 620;

    auto* quickOuter =
        contentHost->findChild<QGridLayout*>(
            QStringLiteral("dashboardQuickOuterGrid"));
    auto* metricHost =
        contentHost->findChild<QWidget*>(
            QStringLiteral("dashboardMetricHost"));
    if (quickOuter && metricHost && revenueSummary) {
        clearGridPosition(quickOuter, metricHost);
        clearGridPosition(quickOuter, revenueSummary);

        if (stackedQuickSummary) {
            quickOuter->addWidget(metricHost, 0, 0);
            quickOuter->addWidget(revenueSummary, 1, 0);
            quickOuter->setColumnStretch(0, 1);
            revenueSummary->setMinimumHeight(150);
        } else {
            quickOuter->addWidget(metricHost, 0, 0, 1, 2);
            quickOuter->addWidget(revenueSummary, 0, 2);
            quickOuter->setColumnStretch(0, 1);
            quickOuter->setColumnStretch(1, 1);
            quickOuter->setColumnStretch(2, 1);
            revenueSummary->setMinimumHeight(140);
        }
    }

    auto* lowerGrid =
        contentHost->findChild<QGridLayout*>(
            QStringLiteral("dashboardLowerHighlightsGrid"));
    auto* social =
        contentHost->findChild<QWidget*>(
            QStringLiteral("dashboardSocialCard"));
    auto* tasks =
        contentHost->findChild<QWidget*>(
            QStringLiteral("dashboardTasksCard"));
    if (lowerGrid && social && tasks) {
        clearGridPosition(lowerGrid, social);
        clearGridPosition(lowerGrid, tasks);

        if (stackedHighlights) {
            lowerGrid->addWidget(social, 0, 0);
            lowerGrid->addWidget(tasks, 1, 0);
            lowerGrid->setColumnStretch(0, 1);
            lowerGrid->setColumnStretch(1, 0);
        } else {
            lowerGrid->addWidget(social, 0, 0);
            lowerGrid->addWidget(tasks, 0, 1);
            lowerGrid->setColumnStretch(0, 1);
            lowerGrid->setColumnStretch(1, 2);
        }
    }

    auto* appsGrid =
        contentHost->findChild<QGridLayout*>(
            QStringLiteral("dashboardPopularAppsGrid"));
    const auto appItems =
        contentHost->findChildren<QWidget*>(
            QStringLiteral("dashboardPopularApp"));
    if (appsGrid && !appItems.isEmpty()) {
        for (QWidget* item : appItems) {
            clearGridPosition(appsGrid, item);
        }

        for (int i = 0; i < appItems.size(); ++i) {
            appsGrid->addWidget(
                appItems.at(i),
                i / popularAppsColumns,
                i % popularAppsColumns);
        }

        for (int column = 0; column < 3; ++column) {
            appsGrid->setColumnStretch(
                column,
                column < popularAppsColumns ? 1 : 0);
        }
    }

    const auto projectNames =
        contentHost->findChildren<QLabel*>(
            QStringLiteral("dashboardProjectOverviewName"));
    for (QLabel* name : projectNames) {
        name->setMinimumWidth(
            veryCompactProjectRows
                ? 100
                : (compactProjectRows ? 130 : 180));
    }

    const auto projectDue =
        contentHost->findChildren<QLabel*>(
            QStringLiteral("dashboardProjectOverviewDue"));
    for (QLabel* due : projectDue) {
        due->setVisible(!compactProjectRows);
    }

    const auto projectStatus =
        contentHost->findChildren<QtMaterial::QtMaterialChip*>(
            QStringLiteral("dashboardProjectOverviewStatus"));
    for (QtMaterial::QtMaterialChip* status : projectStatus) {
        status->setVisible(!veryCompactProjectRows);
    }

    if (statisticsCard) {
        auto* statisticsHeader =
            statisticsCard->findChild<QGridLayout*>(
                QStringLiteral("dashboardStatisticsHeaderGrid"));
        auto* statisticsTitle =
            statisticsCard->findChild<QLabel*>(
                QStringLiteral("dashboardStatisticsTitle"));
        const auto tabs =
            statisticsCard->findChildren<QToolButton*>(
                QStringLiteral("chartTab"));

        if (statisticsHeader
            && statisticsTitle
            && yearCombo
            && monthCombo) {
            clearGridPosition(statisticsHeader, statisticsTitle);
            for (QToolButton* tab : tabs) {
                clearGridPosition(statisticsHeader, tab);
            }
            clearGridPosition(statisticsHeader, yearCombo);
            clearGridPosition(statisticsHeader, monthCombo);

            for (int column = 0; column < 8; ++column) {
                statisticsHeader->setColumnStretch(column, 0);
            }

            if (compactStatisticsHeader) {
                statisticsHeader->addWidget(
                    statisticsTitle,
                    0,
                    0,
                    1,
                    3);
                for (int i = 0; i < tabs.size(); ++i) {
                    statisticsHeader->addWidget(
                        tabs.at(i),
                        1,
                        i);
                }
                statisticsHeader->addWidget(yearCombo, 2, 0);
                statisticsHeader->addWidget(monthCombo, 2, 1, 1, 2);
                statisticsHeader->setColumnStretch(0, 1);
                statisticsHeader->setColumnStretch(1, 1);
                statisticsHeader->setColumnStretch(2, 1);
            } else {
                statisticsHeader->addWidget(statisticsTitle, 0, 0);
                for (int i = 0; i < tabs.size(); ++i) {
                    statisticsHeader->addWidget(
                        tabs.at(i),
                        0,
                        i + 2);
                }
                statisticsHeader->setColumnStretch(5, 1);
                statisticsHeader->addWidget(yearCombo, 0, 6);
                statisticsHeader->addWidget(monthCombo, 0, 7);
            }
        }
    }

    auto* tasksLayout =
        contentHost->findChild<QHBoxLayout*>(
            QStringLiteral("dashboardTasksContentLayout"));
    if (tasksLayout) {
        tasksLayout->setDirection(
            stackedTasksContent
                ? QBoxLayout::TopToBottom
                : QBoxLayout::LeftToRight);
    }

    auto* exportButton =
        contentHost->findChild<QWidget*>(
            QStringLiteral("dashboardOrdersExport"));
    if (exportButton) {
        exportButton->setVisible(availableWidth >= 520);
    }

    if (orders) {
        orders->setColumnHidden(2, availableWidth < 760);
        orders->setColumnHidden(1, availableWidth < 520);
        orders->horizontalHeader()->setStretchLastSection(true);
    }
}

} // namespace DashboardOverviewResponsive
