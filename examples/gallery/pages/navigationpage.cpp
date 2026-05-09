#include "navigationpage.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "qtmaterial/widgets/navigation/qtmaterialmenu.h"
#include "qtmaterial/widgets/navigation/qtmaterialnavigationrail.h"
#include "qtmaterial/widgets/surfaces/qtmaterialbanner.h"
#include "qtmaterial/widgets/surfaces/qtmaterialbottomappbar.h"
#include "qtmaterial/widgets/surfaces/qtmaterialbottomsheet.h"
#include "qtmaterial/widgets/surfaces/qtmaterialnavigationdrawer.h"
#include "qtmaterial/widgets/surfaces/qtmaterialtopappbar.h"

NavigationPage::NavigationPage(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);

    layout->addWidget(new QtMaterialTopAppBar(this));

    auto* content = new QHBoxLayout;

    auto* rail = new QtMaterial::QtMaterialNavigationRail(this);
    rail->addDestination(QStringLiteral("Home"));
    rail->addDestination(QStringLiteral("Search"));
    rail->addDestination(QStringLiteral("Settings"));
    rail->setCurrentIndex(0);

    auto* surfaces = new QVBoxLayout;
    surfaces->addWidget(new QtMaterial::QtMaterialNavigationDrawer(this));
    surfaces->addWidget(new QtMaterial::QtMaterialBottomSheet(this));

    auto* banner = new QtMaterialBanner(this);
    surfaces->addWidget(banner);

    auto* menu = new QtMaterialMenu(this);
    menu->setMinimumHeight(72);
    surfaces->addWidget(menu);

    surfaces->addWidget(new QtMaterialBottomAppBar(this));
    surfaces->addStretch(1);

    content->addWidget(rail);
    content->addLayout(surfaces, 1);

    layout->addLayout(content);
}
