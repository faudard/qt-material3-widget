#include "advanceddatapage.h"

#include <QHeaderView>
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

    auto* tableModel = new QStandardItemModel(0, 3, this);
    tableModel->setHorizontalHeaderLabels({tr("Name"), tr("State"), tr("Value")});

    auto* table = new QtMaterial::QtMaterialTable(this);
    table->setModel(tableModel);
    table->setDense(true);
    table->setMultiSelectionEnabled(true);
    table->setColumnReorderingEnabled(true);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

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
    split->setPaneMinimumExtent(0, 180);
    split->setPaneMinimumExtent(1, 320);
    split->setStretchFactor(1, 1);
    layout->addWidget(split, 1);

    auto* pagination = new QtMaterial::QtMaterialPagination(this);
    pagination->setPageSize(10);
    pagination->setTotalCount(1234);
    layout->addWidget(pagination);

    const auto refreshPage = [tableModel, pagination]() {
        const int firstItem =
            (pagination->page() - 1) * pagination->pageSize();
        const int remaining =
            qMax(0, pagination->totalCount() - firstItem);
        const int visibleRows =
            qMin(pagination->pageSize(), remaining);

        tableModel->setRowCount(visibleRows);
        for (int row = 0; row < visibleRows; ++row) {
            const int itemIndex = firstItem + row;
            tableModel->setData(
                tableModel->index(row, 0),
                QObject::tr("Item %1").arg(itemIndex + 1));
            tableModel->setData(
                tableModel->index(row, 1),
                itemIndex % 2
                    ? QObject::tr("Ready")
                    : QObject::tr("Pending"));
            tableModel->setData(
                tableModel->index(row, 2),
                itemIndex * 10);
        }
    };

    connect(
        pagination,
        &QtMaterial::QtMaterialPagination::pageChanged,
        this,
        [refreshPage](int) { refreshPage(); });
    connect(
        pagination,
        &QtMaterial::QtMaterialPagination::pageSizeChanged,
        this,
        [refreshPage](int) { refreshPage(); });

    refreshPage();
}
