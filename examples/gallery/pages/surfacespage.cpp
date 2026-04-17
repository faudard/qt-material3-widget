#include "surfacespage.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/surfaces/qtmaterialbanner.h"
#include "qtmaterial/widgets/surfaces/qtmaterialcard.h"
#include "qtmaterial/widgets/surfaces/qtmaterialdialog.h"

SurfacesPage::SurfacesPage(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);

    auto* openDialogButton = new QtMaterial::QtMaterialFilledButton(this);
    openDialogButton->setText(QStringLiteral("Open dialog"));

    auto* dialog = new QtMaterial::QtMaterialDialog(this);
    auto* dialogBody = new QLabel(QStringLiteral("This is a spec-driven dialog surface placeholder."), dialog);
    dialogBody->setWordWrap(true);
    dialogBody->setMinimumSize(280, 100);
    dialog->setBodyWidget(dialogBody);

    QObject::connect(openDialogButton, &QAbstractButton::clicked, dialog, [dialog]() {
        dialog->open();
    });

    auto* banner = new QtMaterial::QtMaterialBanner(this);
    banner->setHeadlineText(QStringLiteral("Banner headline"));
    banner->setSupportingText(QStringLiteral("Supporting text for the banner surface."));

    auto* card = new QtMaterial::QtMaterialCard(this);
    card->setMinimumHeight(120);

    layout->addWidget(openDialogButton);
    layout->addWidget(banner);
    layout->addWidget(card);
    layout->addStretch(1);
}
