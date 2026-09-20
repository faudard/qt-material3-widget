#include "advanceddatapage.h"

#include <QLabel>
#include <QStandardItemModel>
#include <QVBoxLayout>

#include "qtmaterial/widgets/data/qtmaterialtable.h"

AdvancedDataPage::AdvancedDataPage(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(12);

    layout->addWidget(new QLabel(tr("0.8 — Desktop data"), this));

    auto* model = new QStandardItemModel(20, 3, this);
    model->setHorizontalHeaderLabels({tr("Name"), tr("State"), tr("Value")});
    for (int row = 0; row < model->rowCount(); ++row) {
        model->setData(model->index(row, 0), tr("Item %1").arg(row + 1));
        model->setData(model->index(row, 1), row % 2 ? tr("Ready") : tr("Pending"));
        model->setData(model->index(row, 2), row * 10);
    }

    auto* table = new QtMaterial::QtMaterialTable(this);
    table->setModel(model);
    table->setDense(true);
    table->setMultiSelectionEnabled(true);
    layout->addWidget(table, 1);
}
