#include "surfacespage.h"
#include <QVBoxLayout>
#include "qtmaterial/widgets/surfaces/qtmaterialbanner.h"
#include "qtmaterial/widgets/surfaces/qtmaterialcard.h"
#include "qtmaterial/widgets/surfaces/qtmaterialdialog.h"
SurfacesPage::SurfacesPage(QWidget* parent) : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    auto* dialog = new QtMaterial::QtMaterialDialog(this);
    dialog->setMinimumHeight(160);
    auto* banner = new QtMaterial::QtMaterialBanner(this);
    banner->setHeadlineText(QStringLiteral("Banner headline"));
    banner->setSupportingText(QStringLiteral("Supporting text for the banner surface."));
    auto* card = new QtMaterial::QtMaterialCard(this);
    layout->addWidget(dialog);
    layout->addWidget(banner);
    layout->addWidget(card);
    layout->addStretch(1);
}
