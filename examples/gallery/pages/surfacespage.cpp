#include "surfacespage.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "qtmaterial/widgets/surfaces/qtmaterialbanner.h"
#include "qtmaterial/widgets/surfaces/qtmaterialbottomappbar.h"
#include "qtmaterial/widgets/surfaces/qtmaterialbottomsheet.h"
#include "qtmaterial/widgets/surfaces/qtmaterialcard.h"
#include "qtmaterial/widgets/surfaces/qtmaterialdialog.h"
#include "qtmaterial/widgets/surfaces/qtmaterialnavigationdrawer.h"
#include "qtmaterial/widgets/surfaces/qtmaterialsnackbar.h"
#include "qtmaterial/widgets/surfaces/qtmaterialsnackbarhost.h"
#include "qtmaterial/widgets/surfaces/qtmaterialtopappbar.h"

SurfacesPage::SurfacesPage(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    auto* topBar = new QtMaterialTopAppBar(QStringLiteral("Top app bar"), this);
    topBar->setElevated(true);
    layout->addWidget(topBar);

    auto* banner = new QtMaterialBanner(
        QStringLiteral("Network available"),
        QStringLiteral("Surface components expose their production interaction and accessibility states."),
        this);
    banner->setPrimaryActionText(QStringLiteral("Review"));
    banner->setDismissible(true);
    layout->addWidget(banner);

    auto* card = new QtMaterial::QtMaterialCard(this);
    card->setTitleText(QStringLiteral("Card surface"));
    card->setBodyText(QStringLiteral("Cards support variants, focus, keyboard activation and RTL."));
    card->setInteractive(true);
    layout->addWidget(card);

    auto* actions = new QHBoxLayout;

    auto* dialog = new QtMaterial::QtMaterialDialog(this);
    dialog->setTitleText(QStringLiteral("Discard changes?"));
    dialog->setSupportingText(QStringLiteral("This demonstrates modal focus management."));
    auto* openDialog = new QPushButton(QStringLiteral("Open dialog"), this);
    connect(openDialog, &QPushButton::clicked, dialog, &QtMaterial::QtMaterialDialog::open);
    actions->addWidget(openDialog);

    auto* bottomSheet = new QtMaterial::QtMaterialBottomSheet(this);
    bottomSheet->setTitleText(QStringLiteral("Bottom sheet"));
    bottomSheet->setSupportingText(QStringLiteral("Drag, collapse, expand or dismiss with Escape."));
    auto* openBottomSheet = new QPushButton(QStringLiteral("Open bottom sheet"), this);
    connect(openBottomSheet, &QPushButton::clicked, bottomSheet, &QtMaterial::QtMaterialBottomSheet::open);
    actions->addWidget(openBottomSheet);

    auto* drawer = new QtMaterial::QtMaterialNavigationDrawer(this);
    drawer->setEdge(QtMaterial::QtMaterialNavigationDrawer::Edge::Left);
    auto* openDrawer = new QPushButton(QStringLiteral("Open navigation drawer"), this);
    connect(openDrawer, &QPushButton::clicked, drawer, &QtMaterial::QtMaterialNavigationDrawer::open);
    actions->addWidget(openDrawer);

    auto* snackbarHost = new QtMaterial::QtMaterialSnackbarHost(this, this);
    auto* snackbarButton = new QPushButton(QStringLiteral("Show snackbar"), this);
    connect(
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
        });
    actions->addWidget(snackbarButton);
    actions->addStretch(1);
    layout->addLayout(actions);

    auto* bottomBar = new QtMaterialBottomAppBar(QStringLiteral("Bottom app bar"), this);
    bottomBar->setElevated(true);
    layout->addWidget(bottomBar);

    layout->addStretch(1);
}
