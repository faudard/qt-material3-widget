#include "advanceddatapage.h"

#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
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

    layout->addWidget(new QLabel(tr("Desktop & productivity 2.0"), this));

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
    split->setDefaultPaneSizes({240, 640});
    split->setRememberPaneSizes(true);
    split->setAnimatedCollapseEnabled(true);
    QSettings settings;
    const QByteArray savedState = settings.value(QStringLiteral("gallery/productivity/split")).toByteArray();
    if (!savedState.isEmpty()) { split->restorePaneState(savedState); }
    connect(split, &QtMaterial::QtMaterialSplitView::paneStateChanged, this, [](const QByteArray& state) {
        QSettings settings;
        settings.setValue(QStringLiteral("gallery/productivity/split"), state);
    });
    layout->addWidget(split, 1);

    auto* splitActions = new QHBoxLayout;
    auto* collapseTree = new QPushButton(tr("Collapse / restore tree"), this);
    connect(collapseTree, &QPushButton::clicked, split, [split]() { split->setPaneCollapsed(0, !split->paneCollapsed(0)); });
    auto* resetSplit = new QPushButton(tr("Reset pane sizes"), this);
    connect(resetSplit, &QPushButton::clicked, split, &QtMaterial::QtMaterialSplitView::resetPaneSizes);
    splitActions->addWidget(collapseTree);
    splitActions->addWidget(resetSplit);
    splitActions->addStretch();
    layout->addLayout(splitActions);
    auto* splitHelp = new QLabel(tr("Focus the divider: arrows resize, Shift accelerates, Home/End reach limits, Enter collapses/restores. Double-click resets; sizes are remembered."), this);
    splitHelp->setWordWrap(true);
    layout->addWidget(splitHelp);

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
