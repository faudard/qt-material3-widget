#include "datapage.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QVBoxLayout>

#include "qtmaterial/widgets/data/qtmaterialcarousel.h"
#include "qtmaterial/widgets/data/qtmaterialdivider.h"
#include "qtmaterial/widgets/data/qtmaterialgridlist.h"
#include "qtmaterial/widgets/data/qtmateriallist.h"
#include "qtmaterial/widgets/data/qtmateriallistitem.h"
#include "qtmaterial/widgets/data/qtmaterialtable.h"
#include "qtmaterial/widgets/surfaces/qtmaterialcard.h"

DataPage::DataPage(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);

    auto* card = new QtMaterial::QtMaterialCard(this);
    layout->addWidget(card);

    auto* item = new QtMaterial::QtMaterialListItem(this);
    item->setHeadlineText(QStringLiteral("Item headline"));
    item->setSupportingText(QStringLiteral("Supporting text"));
    layout->addWidget(item);

    layout->addWidget(new QtMaterial::QtMaterialDivider(this));

    auto* table = new QtMaterial::QtMaterialTable(this);

    auto* tableModel = new QStandardItemModel(4, 3, table);
    tableModel->setHorizontalHeaderLabels({
        QStringLiteral("Name"),
        QStringLiteral("Role"),
        QStringLiteral("Status")
    });

    tableModel->setItem(0, 0, new QStandardItem(QStringLiteral("Ada")));
    tableModel->setItem(0, 1, new QStandardItem(QStringLiteral("Engineer")));
    tableModel->setItem(0, 2, new QStandardItem(QStringLiteral("Active")));

    tableModel->setItem(1, 0, new QStandardItem(QStringLiteral("Linus")));
    tableModel->setItem(1, 1, new QStandardItem(QStringLiteral("Reviewer")));
    tableModel->setItem(1, 2, new QStandardItem(QStringLiteral("Pending")));

    tableModel->setItem(2, 0, new QStandardItem(QStringLiteral("Grace")));
    tableModel->setItem(2, 1, new QStandardItem(QStringLiteral("Designer")));
    tableModel->setItem(2, 2, new QStandardItem(QStringLiteral("Active")));

    tableModel->setItem(3, 0, new QStandardItem(QStringLiteral("Margaret")));
    tableModel->setItem(3, 1, new QStandardItem(QStringLiteral("Lead")));
    tableModel->setItem(3, 2, new QStandardItem(QStringLiteral("Blocked")));

    table->setModel(tableModel);
    table->setMinimumHeight(180);
    table->setMaximumHeight(280);
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    layout->addWidget(table);

    auto* gridList = new QtMaterial::QtMaterialGridList(this);

    auto* gridModel = new QStandardItemModel(gridList);

    for (int i = 0; i < 6; ++i) {
        auto* item = new QStandardItem(
            QStringLiteral("Grid item %1").arg(i + 1)
            );

        item->setData(
            QStringLiteral("Supporting text"),
            Qt::UserRole + 1
            );

        gridModel->appendRow(item);
    }

    gridList->setModel(gridModel);
    gridList->setMinimumHeight(180);
    gridList->setMaximumHeight(280);
    gridList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    layout->addWidget(gridList);

    auto* carousel = new QtMaterial::QtMaterialCarousel(this);

    for (int i = 0; i < 3; ++i) {
        auto* page = new QWidget(carousel);
        auto* pageLayout = new QVBoxLayout(page);

        auto* label = new QLabel(
            QStringLiteral("Carousel page %1").arg(i + 1),
            page
            );

        label->setAlignment(Qt::AlignCenter);

        pageLayout->addWidget(label);

        carousel->addPage(
            page,
            QStringLiteral("Page %1").arg(i + 1)
            );
    }

    carousel->setMinimumHeight(160);
    carousel->setMaximumHeight(220);
    carousel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    layout->addWidget(carousel);

    layout->addStretch(1);
}
