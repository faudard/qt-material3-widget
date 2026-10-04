#include "navigationadvancedpage.h"

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QStyle>
#include <QTimer>
#include <functional>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QVBoxLayout>

#include "qtmaterial/widgets/navigation/qtmaterialbreadcrumb.h"
#include "qtmaterial/widgets/navigation/qtmaterialcommandpalette.h"
#include "qtmaterial/widgets/navigation/qtmaterialmenu.h"
#include "qtmaterial/widgets/navigation/qtmaterialnavigationsuite.h"
#include "qtmaterial/widgets/layouts/qtmaterialadaptiveshell.h"

namespace {

class GalleryCommandProvider final : public QtMaterial::QtMaterialCommandProvider
{
public:
    GalleryCommandProvider(QList<QtMaterial::QtMaterialCommand> commands,
        std::function<void(const QString&)> action, int delay, QObject* parent)
        : QtMaterialCommandProvider(parent), m_commands(std::move(commands)),
          m_action(std::move(action)), m_delay(delay) {}

    void requestCommands(const QString&, quint64 id) override
    {
        m_request = id;
        if (m_delay == 0) { Q_EMIT commandsReady(id, m_commands); return; }
        QTimer::singleShot(m_delay, this, [this, id]() {
            if (m_request == id) { Q_EMIT commandsReady(id, m_commands); }
        });
    }
    void cancelRequest(quint64 id) override { if (m_request == id) { m_request = 0; } }
    void activateCommand(const QString& id) override { m_action(id); }

private:
    QList<QtMaterial::QtMaterialCommand> m_commands;
    std::function<void(const QString&)> m_action;
    int m_delay;
    quint64 m_request = 0;
};

} // namespace

NavigationAdvancedPage::NavigationAdvancedPage(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(12);

    auto* title = new QLabel(tr("Desktop navigation 2.0"), this);
    layout->addWidget(title);

    auto* explanation = new QLabel(
        tr("Breadcrumbs navigate a hierarchy; the command palette provides keyboard-first navigation to the same destinations."),
        this);
    explanation->setWordWrap(true);
    layout->addWidget(explanation);


    auto* adaptiveTitle = new QLabel(tr("Adaptive / Desktop shell"), this);
    layout->addWidget(adaptiveTitle);

    auto* adaptiveHelp = new QLabel(
        tr("Resize the gallery window: Compact uses a bottom Navigation Bar, Medium+ uses a Navigation Rail, "
           "and Expanded+ reveals the supporting pane when enough content width remains."),
        this);
    adaptiveHelp->setWordWrap(true);
    layout->addWidget(adaptiveHelp);

    auto* adaptiveShell =
        new QtMaterial::QtMaterialAdaptiveShell(this);
    adaptiveShell->setMinimumHeight(320);

    auto* adaptiveContent = new QFrame(adaptiveShell);
    auto* adaptiveContentLayout = new QVBoxLayout(adaptiveContent);
    auto* adaptivePageLabel =
        new QLabel(tr("Home — main adaptive content"), adaptiveContent);
    adaptivePageLabel->setAlignment(Qt::AlignCenter);
    adaptiveContentLayout->addWidget(adaptivePageLabel, 1);

    auto* adaptiveSupporting = new QFrame(adaptiveShell);
    auto* adaptiveSupportingLayout =
        new QVBoxLayout(adaptiveSupporting);
    auto* supportingTitle =
        new QLabel(tr("Supporting pane"), adaptiveSupporting);
    supportingTitle->setWordWrap(true);
    adaptiveSupportingLayout->addWidget(supportingTitle);
    adaptiveSupportingLayout->addStretch(1);

    adaptiveShell->setContentWidget(adaptiveContent);
    adaptiveShell->setSupportingWidget(adaptiveSupporting);

    auto* adaptiveNavigation = adaptiveShell->navigationSuite();
    adaptiveNavigation->addDestination(
        tr("Home"),
        style()->standardIcon(QStyle::SP_DirHomeIcon));
    adaptiveNavigation->addDestination(
        tr("Search"),
        style()->standardIcon(QStyle::SP_FileDialogContentsView));
    adaptiveNavigation->addDestination(
        tr("Settings"),
        style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    adaptiveNavigation->setCurrentIndex(0);

    connect(
        adaptiveNavigation,
        &QtMaterial::QtMaterialNavigationSuite::currentIndexChanged,
        this,
        [adaptiveNavigation, adaptivePageLabel](int index) {
            adaptivePageLabel->setText(
                QObject::tr("%1 — main adaptive content")
                    .arg(adaptiveNavigation->destinationText(index)));
        });

    auto* adaptiveStatus = new QLabel(this);
    adaptiveStatus->setWordWrap(true);
    const auto updateAdaptiveStatus =
        [adaptiveShell, adaptiveStatus](QtMaterial::WindowWidthSizeClass) {
            QString widthClass;
            switch (adaptiveShell->windowSizeClass().width) {
            case QtMaterial::WindowWidthSizeClass::Compact:
                widthClass = QObject::tr("Compact");
                break;
            case QtMaterial::WindowWidthSizeClass::Medium:
                widthClass = QObject::tr("Medium");
                break;
            case QtMaterial::WindowWidthSizeClass::Expanded:
                widthClass = QObject::tr("Expanded");
                break;
            case QtMaterial::WindowWidthSizeClass::Large:
                widthClass = QObject::tr("Large");
                break;
            case QtMaterial::WindowWidthSizeClass::ExtraLarge:
                widthClass = QObject::tr("ExtraLarge");
                break;
            }
            adaptiveStatus->setText(
                QObject::tr("Window class: %1 · navigation: %2 · desktop density: %3")
                    .arg(widthClass)
                    .arg(
                        adaptiveShell->navigationSuite()->navigationType()
                                == QtMaterial::NavigationSuiteType::NavigationBar
                            ? QObject::tr("Bar")
                            : QObject::tr("Rail"))
                    .arg(
                        adaptiveShell->resolvedDensity() == QtMaterial::Density::Compact
                            ? QObject::tr("Compact")
                            : adaptiveShell->resolvedDensity() == QtMaterial::Density::Comfortable
                                ? QObject::tr("Comfortable")
                                : QObject::tr("Default")));
        };
    connect(
        adaptiveShell,
        &QtMaterial::QtMaterialAdaptiveShell::widthSizeClassChanged,
        this,
        updateAdaptiveStatus);
    updateAdaptiveStatus(adaptiveShell->windowSizeClass().width);

    layout->addWidget(adaptiveStatus);
    layout->addWidget(adaptiveShell);


    auto* breadcrumb = new QtMaterial::QtMaterialBreadcrumb(this);
    breadcrumb->setItems({
        tr("Workspace"),
        tr("Requirements"),
        tr("Subsystem"),
        tr("Module"),
        tr("REQ-42")
    });
    breadcrumb->setResponsiveElisionEnabled(true);
    breadcrumb->setLocationEditable(true);
    breadcrumb->setItemIcon(0, style()->standardIcon(QStyle::SP_DirHomeIcon));
    breadcrumb->setItemIcon(1, style()->standardIcon(QStyle::SP_DirIcon));
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

    connect(breadcrumb, &QtMaterial::QtMaterialBreadcrumb::locationSubmitted, this,
        [breadcrumb, currentContext](const QString& location) {
            const QStringList path = location.split(QLatin1Char('/'), Qt::SkipEmptyParts);
            if (!path.isEmpty()) {
                breadcrumb->setItems(path);
                breadcrumb->setLocation(location);
                currentContext->setText(QObject::tr("Current context: %1").arg(location));
            }
        });
    auto* editLocation = new QPushButton(tr("Edit location (Ctrl+L while focused)"), this);
    connect(editLocation, &QPushButton::clicked, breadcrumb, [breadcrumb]() { breadcrumb->setEditingLocation(true); });
    layout->addWidget(editLocation);

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
        model->setData(model->index(row, 0), QStringLiteral("navigation.%1").arg(row),
            QtMaterial::QtMaterialCommandPalette::IdRole);
        model->setData(model->index(row, 0), tr("Navigation"), QtMaterial::QtMaterialCommandPalette::SectionRole);
        model->setData(model->index(row, 0), tr("Navigate to a workspace destination"), QtMaterial::QtMaterialCommandPalette::SecondaryTextRole);
        model->setData(model->index(row, 0), style()->standardIcon(QStyle::SP_DirIcon), Qt::DecorationRole);
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
                breadcrumb->setCurrentIndex(breadcrumb->items().size() - 1);
                currentContext->setText(QObject::tr("Current context: REQ-42"));
                break;
            case 3:
                currentContext->setText(QObject::tr("Current context: Requirement search"));
                break;
            default:
                break;
            }
        });

    const auto command = [](const QString& id, const QString& title, const QString& section,
                            const QString& description, const QIcon& icon, const QKeySequence& shortcut) {
        QtMaterial::QtMaterialCommand result;
        result.id = id; result.text = title; result.section = section;
        result.secondaryText = description; result.icon = icon; result.shortcut = shortcut;
        return result;
    };
    const auto action = [currentContext](const QString& id) {
        currentContext->setText(QObject::tr("Executed command: %1").arg(id));
    };
    auto* commandProvider = new GalleryCommandProvider({
        command(QStringLiteral("project.build"), tr("Build project"), tr("Project"),
            tr("Compile the current workspace"), style()->standardIcon(QStyle::SP_ComputerIcon), QKeySequence(QStringLiteral("Ctrl+B"))),
        command(QStringLiteral("project.tests"), tr("Run tests"), tr("Project"),
            tr("Run the workspace test suite"), style()->standardIcon(QStyle::SP_MediaPlay), QKeySequence(QStringLiteral("Ctrl+T")))
    }, action, 0, this);
    auto* recentFilesProvider = new GalleryCommandProvider({
        command(QStringLiteral("file.req42"), tr("Open REQ-42.xml"), tr("Recent files"),
            tr("Workspace / Requirements / Module"), style()->standardIcon(QStyle::SP_FileIcon), QKeySequence()),
        command(QStringLiteral("file.model"), tr("Open model.xml"), tr("Recent files"),
            tr("Workspace / Models"), style()->standardIcon(QStyle::SP_FileIcon), QKeySequence())
    }, action, 180, this);
    auto* settingsProvider = new GalleryCommandProvider({
        command(QStringLiteral("settings.preferences"), tr("Open preferences"), tr("Settings"),
            tr("Editor, shortcuts and workspace preferences"), style()->standardIcon(QStyle::SP_FileDialogDetailedView), QKeySequence(QStringLiteral("Ctrl+,")))
    }, action, 0, this);
    palette->addProvider(commandProvider);
    palette->addProvider(recentFilesProvider);
    palette->addProvider(settingsProvider);
    palette->setCommandFavorite(QStringLiteral("project.build"), true);

    auto* openPalette = new QPushButton(tr("Commands (Ctrl+K / Ctrl+P)"), this);
    connect(openPalette, &QPushButton::clicked, palette, &QtMaterial::QtMaterialCommandPalette::openPalette);
    layout->addWidget(openPalette);
    auto* paletteHelp = new QLabel(tr("Try fuzzy search such as 'bld'. Ctrl+D toggles a favorite; activated commands appear in history. Recent files load asynchronously."), this);
    paletteHelp->setWordWrap(true);
    layout->addWidget(paletteHelp);

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
