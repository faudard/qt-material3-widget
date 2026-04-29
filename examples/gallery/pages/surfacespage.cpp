#include "surfacespage.h"
#include <QVBoxLayout>
#include "qtmaterial/widgets/surfaces/qtmaterialbanner.h"
#include "qtmaterial/widgets/surfaces/qtmaterialcard.h"
#include "qtmaterial/widgets/surfaces/qtmaterialdialog.h"
#include <QLabel>
#include <QPushButton>

#include "qtmaterial/widgets/surfaces/qtmaterialsnackbar.h"
#include "qtmaterial/widgets/surfaces/qtmaterialtopappbar.h"
#include "qtmaterial/widgets/surfaces/qtmaterialbottomappbar.h"
#include "qtmaterial/widgets/surfaces/qtmaterialbottomsheet.h"

SurfacesPage::SurfacesPage(QWidget* parent) : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    auto* dialog = new QtMaterial::QtMaterialDialog(this);
    dialog->setMinimumHeight(160);
    auto* banner = new QtMaterialBanner(this);
    // banner->setHeadlineText(QStringLiteral("Banner headline"));
    // banner->setSupportingText(QStringLiteral("Supporting text for the banner surface."));
    auto* card = new QtMaterial::QtMaterialCard(this);
    layout->addWidget(dialog);
    layout->addWidget(banner);
    layout->addWidget(card);

    auto* snackbarButton = new QPushButton(QStringLiteral("Show snackbar"), this);
    auto* snackbar = new QtMaterial::QtMaterialSnackbar(this);

    QtMaterial::SnackbarRequest request;
    request.text = QStringLiteral("Snackbar message");
    request.actionText = QStringLiteral("Undo");
    request.duration = QtMaterial::SnackbarDuration::Short;
    request.showDismissButton = true;

    QObject::connect(snackbarButton, &QPushButton::clicked, this, [snackbar, request]() {
        snackbar->setRequest(request);
        snackbar->showSnackbar();
    });


    layout->addWidget(snackbarButton);
    layout->addStretch(1);

}
