#include "navigationpage.h"

#include <QHBoxLayout>
#include <QCheckBox>
#include "qtmaterial/widgets/buttons/qtmaterialiconbutton.h"
#include <QLabel>
#include <QStackedWidget>
#include <QStyle>
#include <QVBoxLayout>

#include "qtmaterial/widgets/navigation/qtmaterialfloatingtoolbar.h"
#include "qtmaterial/widgets/navigation/qtmaterialmenu.h"
#include "qtmaterial/widgets/navigation/qtmaterialnavigationbar.h"
#include "qtmaterial/widgets/navigation/qtmaterialnavigationrail.h"
#include "qtmaterial/widgets/navigation/qtmaterialtabs.h"

namespace {

QWidget* contentPage(
    const QString& title,
    const QString& description,
    QWidget* parent)
{
    auto* page = new QWidget(parent);
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(8);

    auto* heading = new QLabel(title, page);
    QFont headingFont = heading->font();
    headingFont.setBold(true);
    headingFont.setPointSize(headingFont.pointSize() + 2);
    heading->setFont(headingFont);

    auto* body = new QLabel(description, page);
    body->setWordWrap(true);

    layout->addWidget(heading);
    layout->addWidget(body);
    layout->addStretch(1);
    return page;
}

} // namespace

NavigationPage::NavigationPage(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(16);

    auto* tabs = new QtMaterial::QtMaterialTabs(this);
    tabs->addTab(
        contentPage(
            QStringLiteral("Overview"),
            QStringLiteral("Overview content. Click Activity or Settings to verify pointer navigation."),
            tabs),
        QStringLiteral("Overview"));
    tabs->addTab(
        contentPage(
            QStringLiteral("Activity"),
            QStringLiteral("Three recent activities are available. The badge belongs to this tab."),
            tabs),
        QStringLiteral("Activity"));
    tabs->addTab(
        contentPage(
            QStringLiteral("Settings"),
            QStringLiteral("Settings content. Tabs now expose visibly different pages in the gallery."),
            tabs),
        QStringLiteral("Settings"));
    tabs->setTabId(0, QStringLiteral("gallery.navigation.overview"));
    tabs->setTabId(1, QStringLiteral("gallery.navigation.activity"));
    tabs->setTabId(2, QStringLiteral("gallery.navigation.settings"));
    tabs->setRoute(0, QStringLiteral("navigation/overview"));
    tabs->setRoute(1, QStringLiteral("navigation/activity"));
    tabs->setRoute(2, QStringLiteral("navigation/settings"));
    tabs->setBadge(1, QStringLiteral("3"));
    tabs->setBadgeVisible(1, true);
    tabs->setMinimumHeight(170);
    layout->addWidget(tabs);

    auto* navigationBar = new QtMaterial::QtMaterialNavigationBar(this);
    navigationBar->addDestination(QStringLiteral("Home"));
    navigationBar->addDestination(QStringLiteral("Explore"));
    navigationBar->addDestination(QStringLiteral("Saved"));
    navigationBar->addDestination(QStringLiteral("Profile"));
    navigationBar->setCurrentIndex(0);
    navigationBar->setMaterialTestId(QStringLiteral("gallery.navigation.bar"));
    layout->addWidget(navigationBar);

    auto* expressiveRow = new QHBoxLayout;
    expressiveRow->setSpacing(16);

    auto* floatingToolbar =
        new QtMaterial::QtMaterialFloatingToolbar(this);
    floatingToolbar->addAction(
        style()->standardIcon(QStyle::SP_FileDialogDetailedView),
        QStringLiteral("Details"));
    floatingToolbar->addAction(
        style()->standardIcon(QStyle::SP_DialogSaveButton),
        QStringLiteral("Save"));
    floatingToolbar->addAction(
        style()->standardIcon(QStyle::SP_DialogCloseButton),
        QStringLiteral("Close"));

    auto* expressiveMenu =
        new QtMaterialMenu(this);
    expressiveMenu->setExpressive(true);
    expressiveMenu->addItem(QStringLiteral("Open"));
    const int renameIndex = expressiveMenu->addItem(QStringLiteral("Rename"));
    expressiveMenu->setItemCheckable(renameIndex, true);
    expressiveMenu->setItemChecked(renameIndex, true);
    const int shareIndex = expressiveMenu->addItem(QStringLiteral("Share"));
    expressiveMenu->setItemEnabled(shareIndex, false);
    expressiveMenu->setMinimumWidth(220);

    expressiveRow->addWidget(floatingToolbar, 0, Qt::AlignTop);
    expressiveRow->addWidget(expressiveMenu, 0, Qt::AlignTop);
    expressiveRow->addStretch(1);
    layout->addLayout(expressiveRow);
    auto* collapseToolbar = new QCheckBox(tr("Collapse floating toolbar"), this);
    auto* verticalToolbar = new QCheckBox(tr("Vertical floating toolbar"), this);
    auto* disableToolbar = new QCheckBox(tr("Disable floating toolbar"), this);
    auto* standardMenu = new QCheckBox(tr("Use standard menu styling"), this);
    auto* expressiveStatus = new QLabel(tr("No navigation action yet"), this);
    expressiveStatus->setAccessibleName(tr("Last expressive navigation action"));
    for (auto* control : {collapseToolbar, verticalToolbar, disableToolbar, standardMenu}) {
        layout->addWidget(control);
    }
    layout->addWidget(expressiveStatus);
    connect(collapseToolbar, &QCheckBox::toggled, floatingToolbar, [floatingToolbar](bool collapsed) {
        floatingToolbar->setExpanded(!collapsed);
    });
    connect(verticalToolbar, &QCheckBox::toggled, floatingToolbar, [floatingToolbar](bool vertical) {
        floatingToolbar->setOrientation(vertical ? Qt::Vertical : Qt::Horizontal);
    });
    connect(disableToolbar, &QCheckBox::toggled, floatingToolbar, &QWidget::setDisabled);
    connect(standardMenu, &QCheckBox::toggled, expressiveMenu, [expressiveMenu](bool standard) {
        expressiveMenu->setExpressive(!standard);
    });
    connect(floatingToolbar, &QtMaterial::QtMaterialFloatingToolbar::actionTriggered,
            expressiveStatus, [expressiveStatus, floatingToolbar](int index) {
                expressiveStatus->setText(QObject::tr("Toolbar action: %1").arg(floatingToolbar->itemAt(index)->accessibleName()));
            });
    connect(expressiveMenu, &QtMaterialMenu::activated, expressiveStatus, [expressiveStatus](int index) {
        expressiveStatus->setText(QObject::tr("Menu action %1").arg(index + 1));
    });

    auto* railRow = new QHBoxLayout;
    railRow->setSpacing(16);

    auto* rail = new QtMaterial::QtMaterialNavigationRail(this);
    rail->addDestination(QStringLiteral("Home"));
    rail->addDestination(QStringLiteral("Search"));
    rail->addDestination(QStringLiteral("Settings"));
    rail->setCurrentIndex(0);

    auto* railContent = new QStackedWidget(this);
    railContent->addWidget(
        contentPage(
            QStringLiteral("Home"),
            QStringLiteral("Home destination selected."),
            railContent));
    railContent->addWidget(
        contentPage(
            QStringLiteral("Search"),
            QStringLiteral("Search destination selected. This page changes when the rail is clicked."),
            railContent));
    railContent->addWidget(
        contentPage(
            QStringLiteral("Settings"),
            QStringLiteral("Navigation-rail settings destination selected."),
            railContent));

    connect(
        rail,
        &QtMaterial::QtMaterialNavigationRail::currentIndexChanged,
        railContent,
        &QStackedWidget::setCurrentIndex);

    railRow->addWidget(rail);
    railRow->addWidget(railContent, 1);

    layout->addLayout(railRow, 1);
}
