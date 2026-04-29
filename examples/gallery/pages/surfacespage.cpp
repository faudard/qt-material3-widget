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
#include "qtmaterial/widgets/surfaces/qtmaterialsnackbarhost.h"


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

    auto* snackbarHost = new QtMaterial::QtMaterialSnackbarHost(this, this);

    auto* snackbarButton = new QPushButton(QStringLiteral("Show snackbar"), this);

    QObject::connect(
        snackbarButton,
        &QPushButton::clicked,
        this,
        [snackbarHost]() {
            QtMaterial::SnackbarRequest request;
            request.text = QStringLiteral("Snackbar message");
            request.actionText = QStringLiteral("Undo");
            request.duration = QtMaterial::SnackbarDuration::Short;
            request.showDismissButton = true;

            snackbarHost->showMessage(request, true);
        }
        );

    layout->addWidget(snackbarButton);
    layout->addStretch(1);

}
