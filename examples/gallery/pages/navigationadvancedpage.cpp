#include "navigationadvancedpage.h"

#include <QLabel>
#include <QPushButton>
#include <QShortcut>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QVBoxLayout>

#include "qtmaterial/widgets/navigation/qtmaterialbreadcrumb.h"
#include "qtmaterial/widgets/navigation/qtmaterialcommandpalette.h"
#include "qtmaterial/widgets/navigation/qtmaterialmenu.h"

NavigationAdvancedPage::NavigationAdvancedPage(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(12);

    auto* title = new QLabel(tr("0.9 — Desktop navigation"), this);
    layout->addWidget(title);

    auto* explanation = new QLabel(
        tr("Breadcrumbs navigate a hierarchy; the command palette provides keyboard-first navigation to the same destinations."),
        this);
    explanation->setWordWrap(true);
    layout->addWidget(explanation);

    auto* breadcrumb = new QtMaterial::QtMaterialBreadcrumb(this);
    breadcrumb->setItems({
        tr("Workspace"),
        tr("Requirements"),
        tr("Subsystem"),
        tr("Module"),
        tr("REQ-42")
    });
    breadcrumb->setResponsiveElisionEnabled(true);
    breadcrumb->setMaximumWidth(420);
    layout->addWidget(breadcrumb);

    auto* currentContext = new QLabel(tr("Current context: REQ-42"), this);
    layout->addWidget(currentContext);

    connect(
        breadcrumb,
        &QtMaterial::QtMaterialBreadcrumb::activated,
        this,
        [currentContext](int, const QString& text) {
            currentContext->setText(
                QObject::tr("Current context: %1").arg(text));
        });

    auto* model = new QStandardItemModel(4, 1, this);
    const QStringList commands = {
        tr("Go to Workspace"),
        tr("Go to Requirements"),
        tr("Open REQ-42"),
        tr("Show requirement search")
    };
    const QStringList shortcuts = {
        tr("Alt+1"),
        tr("Alt+2"),
        tr("Ctrl+O"),
        tr("Ctrl+F")
    };
    for (int row = 0; row < commands.size(); ++row) {
        model->setData(
            model->index(row, 0),
            commands.at(row));
        model->setData(
            model->index(row, 0),
            shortcuts.at(row),
            QtMaterial::QtMaterialCommandPalette::ShortcutRole);
    }

    auto* palette = new QtMaterial::QtMaterialCommandPalette(this);
    palette->setSourceModel(model);
    palette->setEmptyStateText(tr("No matching command"));

    connect(
        palette,
        &QtMaterial::QtMaterialCommandPalette::commandActivated,
        this,
        [breadcrumb, currentContext](const QModelIndex& index) {
            switch (index.row()) {
            case 0:
                breadcrumb->setCurrentIndex(0);
                currentContext->setText(QObject::tr("Current context: Workspace"));
                break;
            case 1:
                breadcrumb->setCurrentIndex(1);
                currentContext->setText(QObject::tr("Current context: Requirements"));
                break;
            case 2:
                breadcrumb->setCurrentIndex(2);
                currentContext->setText(QObject::tr("Current context: REQ-42"));
                break;
            case 3:
                currentContext->setText(QObject::tr("Current context: Requirement search"));
                break;
            default:
                break;
            }
        });

    const auto openCommandPalette = [palette]() {
        palette->setQuery(QString());
        palette->show();
        palette->raise();
        palette->activateWindow();
    };

    auto* openPalette =
        new QPushButton(tr("Commands (Ctrl+K)"), this);
    connect(
        openPalette,
        &QPushButton::clicked,
        this,
        openCommandPalette);
    layout->addWidget(openPalette);

    auto* shortcut =
        new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_K), this);
    connect(
        shortcut,
        &QShortcut::activated,
        this,
        openCommandPalette);

    auto* menuTitle = new QLabel(tr("Menu"), this);
    layout->addWidget(menuTitle);

    auto* menuDescription = new QLabel(
        tr("Use keyboard arrows, type-ahead, checkable actions, disabled actions and shortcuts. Toggle RTL in the gallery settings to verify mirroring."),
        this);
    menuDescription->setWordWrap(true);
    layout->addWidget(menuDescription);

    auto* menu = new QtMaterialMenu(this);
    menu->setAccessibleName(tr("Gallery action menu"));
    const int openItem = menu->addItem(tr("Open requirement"));
    menu->setItemShortcutText(openItem, tr("Ctrl+O"));

    const int copyItem = menu->addItem(tr("Copy identifier"));
    menu->setItemShortcutText(copyItem, tr("Ctrl+C"));

    menu->addSeparator();

    const int detailsItem = menu->addItem(tr("Show details"));
    menu->setItemCheckable(detailsItem, true);
    menu->setItemChecked(detailsItem, true);

    const int deleteItem = menu->addItem(tr("Delete requirement"));
    menu->setItemEnabled(deleteItem, false);
    menu->setMaximumWidth(menu->sizeHint().width());
    layout->addWidget(menu, 0, Qt::AlignLeading);

    auto* menuStatus = new QLabel(tr("Menu action: none"), this);
    layout->addWidget(menuStatus);

    connect(
        menu,
        &QtMaterialMenu::activated,
        this,
        [menu, menuStatus](int index) {
            menuStatus->setText(
                QObject::tr("Menu action: %1")
                    .arg(menu->itemText(index)));
        });

    layout->addStretch(1);
}
