#include "datapage.h"

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
    table->setMinimumHeight(72);
    layout->addWidget(table);

    auto* gridList = new QtMaterial::QtMaterialGridList(this);
    gridList->setMinimumHeight(72);
    layout->addWidget(gridList);

    auto* carousel = new QtMaterial::QtMaterialCarousel(this);
    carousel->setMinimumHeight(72);
    layout->addWidget(carousel);

    layout->addStretch(1);
}
