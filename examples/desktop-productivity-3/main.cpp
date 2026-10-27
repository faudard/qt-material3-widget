#include <QApplication>
#include <QCheckBox>
#include <QCoreApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QSettings>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTabWidget>
#include <QVBoxLayout>

#include "qtmaterial/widgets/data/qtmaterialmultisortfilterproxy.h"
#include "qtmaterial/widgets/data/qtmaterialselectionstate.h"
#include "qtmaterial/widgets/data/qtmaterialtable.h"
#include "qtmaterial/widgets/data/qtmaterialtreeview.h"
#include "qtmaterial/widgets/layouts/qtmaterialsplitview.h"
#include "qtmaterial/widgets/navigation/qtmaterialcommandpalette.h"

namespace {

constexpr int kStableIdRole = Qt::UserRole + 42;

QWidget* createTextPage(const QString& heading, const QString& description)
{
    auto* page = new QWidget;
    auto* layout = new QVBoxLayout(page);
    layout->addWidget(new QLabel(heading));
    auto* text = new QLabel(description);
    text->setWordWrap(true);
    layout->addWidget(text);
    layout->addStretch();
    return page;
}

} // namespace

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("QtMaterial3"));
    QCoreApplication::setApplicationName(QStringLiteral("DesktopProductivity3"));

    QMainWindow window;
    window.setWindowTitle(QStringLiteral("Material 3 - Desktop Productivity 3.0"));
    window.resize(1100, 680);

    auto* content = new QWidget(&window);
    auto* mainLayout = new QVBoxLayout(content);
    auto* toolbar = new QHBoxLayout;
    auto* openCommands = new QPushButton(QStringLiteral("Commands (Ctrl+K)"));
    toolbar->addWidget(new QLabel(QStringLiteral("Productivity workspace")));
    toolbar->addStretch();
    toolbar->addWidget(openCommands);
    mainLayout->addLayout(toolbar);

    auto* tabs = new QTabWidget(content);
    mainLayout->addWidget(tabs, 1);
    window.setCentralWidget(content);

    auto* explorer = new QWidget(tabs);
    auto* explorerLayout = new QVBoxLayout(explorer);
    auto* search = new QLineEdit(explorer);
    search->setPlaceholderText(QStringLiteral("Filter across project, priority and status"));
    explorerLayout->addWidget(search);

    auto* split = new QtMaterial::QtMaterialSplitView(Qt::Horizontal, explorer);
    explorerLayout->addWidget(split, 1);

    auto* tree = new QtMaterial::QtMaterialTreeView(split);
    auto* treeModel = new QStandardItemModel(&window);
    auto* workspaceItem = new QStandardItem(QStringLiteral("Workspace"));
    workspaceItem->setData(QStringLiteral("workspace"), kStableIdRole);
    for (int i = 0; i < 6; ++i) {
        auto* child = new QStandardItem(QStringLiteral("Project %1").arg(i + 1));
        child->setData(QStringLiteral("project-%1").arg(i), kStableIdRole);
        workspaceItem->appendRow(child);
    }
    treeModel->appendRow(workspaceItem);
    tree->setModel(treeModel);
    tree->expand(treeModel->index(0, 0));
    split->addWidget(tree);

    auto* table = new QtMaterial::QtMaterialTable(split);
    table->setSortingEnabled(false);
    table->setMultiSelectionEnabled(true);
    table->setColumnReorderingEnabled(true);
    auto* data = new QStandardItemModel(0, 3, &window);
    data->setHorizontalHeaderLabels({QStringLiteral("Project"),
                                     QStringLiteral("Priority"),
                                     QStringLiteral("Status")});
    for (int row = 0; row < 200; ++row) {
        data->insertRow(row);
        data->setData(data->index(row, 0), QStringLiteral("Project %1").arg(row % 6 + 1));
        data->setData(data->index(row, 0),
                      QStringLiteral("entry-%1").arg(row), kStableIdRole);
        data->setData(data->index(row, 1), row % 10);
        data->setData(data->index(row, 2),
                      row % 3 ? QStringLiteral("Open") : QStringLiteral("Done"));
    }

    // The application owns both proxies and the source data model.
    auto* multi = new QtMaterial::QtMaterialMultiSortFilterProxy(&window);
    multi->setSourceModel(data);
    multi->setSortKeys({{0, Qt::AscendingOrder}, {1, Qt::DescendingOrder}});
    auto* filter = new QSortFilterProxyModel(&window);
    filter->setSourceModel(multi);
    table->setModel(filter);
    split->addWidget(table);
    split->setDefaultPaneSizes({260, 800});
    QObject::connect(search, &QLineEdit::textChanged,
                     multi, &QtMaterial::QtMaterialMultiSortFilterProxy::setSearchQuery);

    tabs->addTab(explorer, QStringLiteral("Explorer"));
    tabs->addTab(createTextPage(
        QStringLiteral("Dashboard"),
        QStringLiteral("Application-owned dashboard content and models.")),
        QStringLiteral("Dashboard"));

    auto* settingsPage = new QWidget(tabs);
    auto* settingsLayout = new QVBoxLayout(settingsPage);
    settingsLayout->addWidget(new QLabel(QStringLiteral("Workspace preferences")));
    auto* compact = new QCheckBox(QStringLiteral("Dense table rows"), settingsPage);
    settingsLayout->addWidget(compact);
    settingsLayout->addStretch();
    QObject::connect(compact, &QCheckBox::toggled,
                     table, &QtMaterial::QtMaterialTable::setDense);
    tabs->addTab(settingsPage, QStringLiteral("Settings"));

    auto* commands = new QStandardItemModel(3, 1, &window);
    const QStringList titles = {QStringLiteral("Go to Explorer"),
                                QStringLiteral("Go to Dashboard"),
                                QStringLiteral("Go to Settings")};
    for (int row = 0; row < titles.size(); ++row) {
        commands->setData(commands->index(row, 0), titles.at(row));
        commands->setData(commands->index(row, 0),
                          QStringLiteral("workspace.%1").arg(row),
                          QtMaterial::QtMaterialCommandPalette::IdRole);
        commands->setData(commands->index(row, 0),
                          QStringLiteral("Workspace"),
                          QtMaterial::QtMaterialCommandPalette::SectionRole);
    }
    auto* palette = new QtMaterial::QtMaterialCommandPalette(&window);
    palette->setSourceModel(commands);
    QObject::connect(openCommands, &QPushButton::clicked,
                     palette, &QtMaterial::QtMaterialCommandPalette::openPalette);
    QObject::connect(palette, &QtMaterial::QtMaterialCommandPalette::commandActivated,
                     tabs, [tabs](const QModelIndex& index) {
        if (index.isValid()) {
            tabs->setCurrentIndex(index.row());
        }
    });

    // Restore only after creating models, proxy chains and pane order.
    QSettings settings;
    table->restoreWorkspaceState(settings.value(QStringLiteral("table")).toByteArray());
    tree->restoreWorkspaceState(settings.value(QStringLiteral("tree")).toByteArray());
    palette->restoreWorkspaceState(settings.value(QStringLiteral("palette")).toByteArray());
    split->restoreWorkspaceState(settings.value(QStringLiteral("split")).toByteArray());
    QtMaterial::QtMaterialSelectionState::restore(
        table, settings.value(QStringLiteral("selection")).toByteArray(),
        kStableIdRole);
    tabs->setCurrentIndex(settings.value(QStringLiteral("page"), 0).toInt());

    QObject::connect(&app, &QCoreApplication::aboutToQuit, &window, [&]() {
        QSettings output;
        output.setValue(QStringLiteral("table"), table->saveWorkspaceState());
        output.setValue(QStringLiteral("tree"), tree->saveWorkspaceState());
        output.setValue(QStringLiteral("palette"), palette->saveWorkspaceState());
        output.setValue(QStringLiteral("split"), split->saveWorkspaceState());
        output.setValue(QStringLiteral("selection"),
                        QtMaterial::QtMaterialSelectionState::save(table, kStableIdRole));
        output.setValue(QStringLiteral("page"), tabs->currentIndex());
    });

    window.show();
    return app.exec();
}
