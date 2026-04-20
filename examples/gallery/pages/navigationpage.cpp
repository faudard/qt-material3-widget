#include "navigationpage.h"
#include <QVBoxLayout>
#include "qtmaterial/widgets/surfaces/qtmaterialbanner.h"
#include "qtmaterial/widgets/surfaces/qtmaterialbottomappbar.h"
#include "qtmaterial/widgets/surfaces/qtmaterialbottomsheet.h"
#include "qtmaterial/widgets/surfaces/qtmaterialnavigationdrawer.h"
#include "qtmaterial/widgets/surfaces/qtmaterialtopappbar.h"
NavigationPage::NavigationPage(QWidget* parent) : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QtMaterialTopAppBar(this));
    layout->addWidget(new QtMaterial::QtMaterialNavigationDrawer(this));
    layout->addWidget(new QtMaterial::QtMaterialBottomSheet(this));
    auto* banner = new QtMaterialBanner(this);
    // banner->setHeadlineText(QStringLiteral("Banner headline"));
    // banner->setSupportingText(QStringLiteral("Navigation and supporting surfaces placeholder."));
    layout->addWidget(banner);
    layout->addWidget(new QtMaterialBottomAppBar(this));
    layout->addStretch(1);
}
