#include "advanceddatapage.h"

#include <QLabel>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QVBoxLayout>

#include "qtmaterial/widgets/data/qtmaterialpagination.h"
#include "qtmaterial/widgets/data/qtmaterialtable.h"
#include "qtmaterial/widgets/data/qtmaterialtreeview.h"
#include "qtmaterial/widgets/layouts/qtmaterialsplitview.h"

AdvancedDataPage::AdvancedDataPage(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(12);

    layout->addWidget(new QLabel(tr("0.9 — Desktop & productivity"), this));

    auto* tableModel = new QStandardItemModel(20, 3, this);
    tableModel->setHorizontalHeaderLabels({tr("Name"), tr("State"), tr("Value")});
    for (int row = 0; row < tableModel->rowCount(); ++row) {
        tableModel->setData(tableModel->index(row, 0), tr("Item %1").arg(row + 1));
        tableModel->setData(tableModel->index(row, 1), row % 2 ? tr("Ready") : tr("Pending"));
        tableModel->setData(tableModel->index(row, 2), row * 10);
    }

    auto* table = new QtMaterial::QtMaterialTable(this);
    table->setModel(tableModel);
    table->setDense(true);
    table->setMultiSelectionEnabled(true);
    table->setColumnReorderingEnabled(true);

    auto* treeModel = new QStandardItemModel(this);
    auto* workspace = new QStandardItem(tr("Workspace"));
    workspace->appendRow(new QStandardItem(tr("Requirements")));
    workspace->appendRow(new QStandardItem(tr("Models")));
    treeModel->appendRow(workspace);

    auto* tree = new QtMaterial::QtMaterialTreeView(this);
    tree->setModel(treeModel);
    tree->expandAll();

    auto* split = new QtMaterial::QtMaterialSplitView(Qt::Horizontal, this);
    split->addWidget(tree);
    split->addWidget(table);
    split->setStretchFactor(1, 1);
    layout->addWidget(split, 1);

    auto* pagination = new QtMaterial::QtMaterialPagination(this);
    pagination->setTotalCount(1234);
    pagination->setPageSize(25);
    layout->addWidget(pagination);
}
