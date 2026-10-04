#include "datapage.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QToolButton>
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

    auto* list = new QtMaterial::QtMaterialList(this);
    list->addItem(QStringLiteral("Inbox"));
    list->addItem(QStringLiteral("Archive"));
    list->addItem(QStringLiteral("Trash"));
    list->setCurrentIndex(0);
    list->setMinimumHeight(160);
    layout->addWidget(list);

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
    for (int i = 0; i < 6; ++i) {
        gridList->addGridItem(
            QStringLiteral("Grid item %1").arg(i + 1),
            QStringLiteral("Supporting text")
            );
    }
    gridList->setMinimumHeight(180);
    gridList->setMaximumHeight(280);
    gridList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    layout->addWidget(gridList);

    auto* carousel = new QtMaterial::QtMaterialCarousel(this);
    for (int i = 0; i < 6; ++i) {
        carousel->addItem(
            QStringLiteral("Page %1").arg(i + 1),
            QStringLiteral("Carousel page %1").arg(i + 1)
            );
    }
    carousel->setVisibleItemCount(3);

    carousel->setMinimumHeight(160);
    carousel->setMaximumHeight(220);
    carousel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto* previousCarousel = new QToolButton(this);
    previousCarousel->setArrowType(Qt::LeftArrow);
    previousCarousel->setToolTip(QStringLiteral("Previous carousel item"));
    previousCarousel->setAccessibleName(QStringLiteral("Previous carousel item"));

    auto* nextCarousel = new QToolButton(this);
    nextCarousel->setArrowType(Qt::RightArrow);
    nextCarousel->setToolTip(QStringLiteral("Next carousel item"));
    nextCarousel->setAccessibleName(QStringLiteral("Next carousel item"));

    connect(
        previousCarousel,
        &QToolButton::clicked,
        carousel,
        &QtMaterial::QtMaterialCarousel::previous);
    connect(
        nextCarousel,
        &QToolButton::clicked,
        carousel,
        &QtMaterial::QtMaterialCarousel::next);

    auto* carouselRow = new QHBoxLayout;
    carouselRow->addWidget(previousCarousel, 0, Qt::AlignVCenter);
    carouselRow->addWidget(carousel, 1);
    carouselRow->addWidget(nextCarousel, 0, Qt::AlignVCenter);

    layout->addLayout(carouselRow);

    layout->addStretch(1);
}
