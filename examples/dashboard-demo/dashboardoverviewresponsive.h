#pragma once

class QWidget;

namespace QtMaterial {
class QtMaterialCard;
class QtMaterialComboBox;
class QtMaterialTable;
}

namespace DashboardOverviewResponsive {

void apply(
    QWidget* contentHost,
    QWidget* revenueSummary,
    QtMaterial::QtMaterialCard* statisticsCard,
    QtMaterial::QtMaterialComboBox* yearCombo,
    QtMaterial::QtMaterialComboBox* monthCombo,
    QtMaterial::QtMaterialTable* orders,
    int availableWidth);

} // namespace DashboardOverviewResponsive
