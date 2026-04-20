#include "datapage.h"
#include <QVBoxLayout>
#include "qtmaterial/widgets/data/qtmaterialdivider.h"
#include "qtmaterial/widgets/data/qtmateriallist.h"
#include "qtmaterial/widgets/data/qtmateriallistitem.h"
#include "qtmaterial/widgets/surfaces/qtmaterialcard.h"
DataPage::DataPage(QWidget* parent) : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    auto* card = new QtMaterial::QtMaterialCard(this);
    layout->addWidget(card);
    auto* item = new QtMaterial::QtMaterialListItem(this);
    item->setHeadlineText(QStringLiteral("Item headline"));
    item->setSupportingText(QStringLiteral("Supporting text"));
    layout->addWidget(item);
    layout->addWidget(new QtMaterial::QtMaterialDivider(this));
    // layout->addWidget(new QtMaterial::QtMaterialList(this));
    layout->addStretch(1);
}
