#include <QApplication>
#include <QFrame>
#include <QGroupBox>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QScrollArea>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QWidget>

#include "qtmaterial/widgets/layouts/qtmaterialsplitview.h"
#include "qtmaterial/widgets/navigation/qtmaterialbreadcrumb.h"
#include "qtmaterial/widgets/navigation/qtmaterialcommandpalette.h"
#include "qtmaterial/widgets/navigation/qtmaterialmenu.h"
#include "qtmaterial/widgets/navigation/qtmaterialnavigationrail.h"
#include "qtmaterial/widgets/navigation/qtmaterialtabs.h"

namespace {

QGroupBox* section(
    const QString& title,
    QWidget* child,
    QWidget* parent)
{
    auto* box = new QGroupBox(title, parent);
    auto* layout = new QVBoxLayout(box);
    layout->addWidget(child);
    return box;
}

QLabel* paneLabel(
    const QString& text,
    QWidget* parent)
{
    auto* label = new QLabel(text, parent);
    label->setAlignment(Qt::AlignCenter);
    label->setFrameShape(QFrame::StyledPanel);
    label->setMinimumSize(120, 80);
    label->setAccessibleName(text);
    return label;
}

} // namespace

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle(
        QStringLiteral("QtMaterial3 1.5 Enterprise accessibility certification"));
    window.resize(960, 900);

    auto* scroll = new QScrollArea(&window);
    scroll->setWidgetResizable(true);
    auto* content = new QWidget(scroll);
    auto* layout = new QVBoxLayout(content);
    layout->setSpacing(16);

    auto* instructions = new QLabel(
        QStringLiteral(
            "Keyboard-only certification fixture. Review every section with NVDA, "
            "Orca and VoiceOver. Verify traversal, state announcements, activation "
            "and focus before recording a pass in "
            "docs/components/enterprise-accessibility-1.5.json."),
        content);
    instructions->setWordWrap(true);
    instructions->setAccessibleName(
        QStringLiteral("Enterprise accessibility certification instructions"));
    layout->addWidget(instructions);

    auto* rail = new QtMaterial::QtMaterialNavigationRail(content);
    rail->setAccessibleName(QStringLiteral("Enterprise Navigation Rail"));
    rail->addDestination(QStringLiteral("Home"));
    rail->addDestination(QStringLiteral("Search"));
    rail->addDestination(QStringLiteral("Disabled"));
    rail->addDestination(QStringLiteral("Settings"));
    rail->setDestinationEnabled(2, false);
    rail->setCurrentIndex(0);
    layout->addWidget(
        section(QStringLiteral("Navigation Rail"), rail, content));

    auto* tabs = new QtMaterial::QtMaterialTabs(content);
    tabs->setAccessibleName(QStringLiteral("Enterprise Tabs"));
    tabs->addTab(new QLabel(QStringLiteral("Overview content"), tabs),
                 QStringLiteral("Overview"));
    tabs->addTab(new QLabel(QStringLiteral("Disabled content"), tabs),
                 QStringLiteral("Disabled"));
    tabs->addTab(new QLabel(QStringLiteral("Settings content"), tabs),
                 QStringLiteral("Settings"));
    tabs->setTabEnabled(1, false);
    tabs->setCurrentIndex(0);
    tabs->setWrapNavigation(true);
    layout->addWidget(
        section(QStringLiteral("Tabs"), tabs, content));

    auto* menu = new QtMaterialMenu(content);
    menu->setAccessibleName(QStringLiteral("Enterprise Menu"));
    const int openIndex = menu->addItem(QStringLiteral("Open project"));
    menu->setItemShortcutText(openIndex, QStringLiteral("Ctrl+O"));
    const int favoriteIndex = menu->addItem(QStringLiteral("Favorite"));
    menu->setItemCheckable(favoriteIndex, true);
    menu->setItemChecked(favoriteIndex, true);
    menu->addSeparator();
    const int disabledIndex = menu->addItem(QStringLiteral("Disabled action"));
    menu->setItemEnabled(disabledIndex, false);
    menu->setCurrentIndex(openIndex);
    layout->addWidget(
        section(QStringLiteral("Menu"), menu, content));

    auto* breadcrumb = new QtMaterial::QtMaterialBreadcrumb(content);
    breadcrumb->setAccessibleName(QStringLiteral("Enterprise Breadcrumb"));
    breadcrumb->setItems(QStringList{
        QStringLiteral("Workspace"),
        QStringLiteral("Requirements"),
        QStringLiteral("Subsystem"),
        QStringLiteral("REQ-42")
    });
    breadcrumb->setCurrentIndex(3);
    breadcrumb->setMaximumVisibleItems(3);
    breadcrumb->setResponsiveElisionEnabled(true);
    breadcrumb->setLocationEditable(true);
    breadcrumb->setLocation(
        QStringLiteral("/Workspace/Requirements/Subsystem/REQ-42"));
    layout->addWidget(
        section(QStringLiteral("Breadcrumb"), breadcrumb, content));

    auto* paletteButton = new QPushButton(
        QStringLiteral("Open Command Palette"), content);
    paletteButton->setAccessibleName(
        QStringLiteral("Open Enterprise Command Palette"));
    layout->addWidget(
        section(QStringLiteral("Command Palette"), paletteButton, content));

    auto* palette = new QtMaterial::QtMaterialCommandPalette(&window);
    palette->setAccessibleName(QStringLiteral("Enterprise Command Palette"));
    auto* paletteModel = new QStandardItemModel(palette);

    auto* openCommand = new QStandardItem(QStringLiteral("Open project"));
    openCommand->setData(
        QStringLiteral("Browse a workspace"),
        QtMaterial::QtMaterialCommandPalette::SecondaryTextRole);
    openCommand->setData(
        QStringLiteral("Ctrl+O"),
        QtMaterial::QtMaterialCommandPalette::ShortcutRole);
    paletteModel->appendRow(openCommand);

    auto* searchCommand = new QStandardItem(QStringLiteral("Search requirements"));
    searchCommand->setData(
        QStringLiteral("Find requirement text"),
        QtMaterial::QtMaterialCommandPalette::SecondaryTextRole);
    searchCommand->setData(
        QStringLiteral("Ctrl+F"),
        QtMaterial::QtMaterialCommandPalette::ShortcutRole);
    paletteModel->appendRow(searchCommand);

    auto* disabledCommand = new QStandardItem(QStringLiteral("Disabled command"));
    disabledCommand->setEnabled(false);
    paletteModel->appendRow(disabledCommand);

    palette->setSourceModel(paletteModel);
    QObject::connect(
        paletteButton,
        &QPushButton::clicked,
        palette,
        &QtMaterial::QtMaterialCommandPalette::openPalette);

    auto* split = new QtMaterial::QtMaterialSplitView(
        Qt::Horizontal,
        content);
    split->setAccessibleName(QStringLiteral("Enterprise Split View"));
    split->setHandleWidth(8);
    split->setKeyboardResizeStep(12);
    split->addWidget(
        paneLabel(QStringLiteral("Navigation pane"), split));
    split->addWidget(
        paneLabel(QStringLiteral("Primary pane"), split));
    split->addWidget(
        paneLabel(QStringLiteral("Supporting pane"), split));
    split->setPaneMinimumExtent(0, 120);
    split->setPaneMinimumExtent(1, 160);
    split->setPaneMinimumExtent(2, 120);
    split->setDefaultPaneSizes(QList<int>{220, 420, 220});
    split->setSizes(QList<int>{220, 420, 220});
    split->setMinimumHeight(180);
    layout->addWidget(
        section(QStringLiteral("Split View"), split, content));

    layout->addStretch(1);
    scroll->setWidget(content);
    window.setCentralWidget(scroll);
    window.show();

    return app.exec();
}
