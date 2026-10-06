#include <QtTest/QtTest>

#include <memory>

#include <QApplication>
#include <QComboBox>
#include <QCursor>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSplitterHandle>
#include <QTabBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QPalette>
#include <QSlider>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QWidget>

#include "qtmaterial/integration/qtmaterialpaletteadapter.h"
#include "qtmaterial/theme/qtmaterialaccessibilitytokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/widgets/buttons/qtmaterialelevatedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialbuttongroup.h"
#include "qtmaterial/widgets/buttons/qtmaterialsplitbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"
#include "qtmaterial/widgets/data/qtmaterialbadge.h"
#include "qtmaterial/widgets/data/qtmaterialcarousel.h"
#include "qtmaterial/widgets/data/qtmaterialdivider.h"
#include "qtmaterial/widgets/data/qtmaterialgridlist.h"
#include "qtmaterial/widgets/data/qtmateriallist.h"
#include "qtmaterial/widgets/data/qtmaterialsegmentedlist.h"
#include "qtmaterial/widgets/progress/qtmaterialcircularprogressindicator.h"
#include "qtmaterial/widgets/progress/qtmaterialloadingindicator.h"
#include "qtmaterial/widgets/progress/qtmateriallinearprogressindicator.h"
#include "qtmaterial/widgets/selection/qtmaterialchip.h"
#include "qtmaterial/widgets/surfaces/qtmaterialbanner.h"
#include "qtmaterial/widgets/surfaces/qtmaterialbottomappbar.h"
#include "qtmaterial/widgets/surfaces/qtmaterialtopappbar.h"
#include "qtmaterial/widgets/data/qtmaterialpagination.h"
#include "qtmaterial/widgets/data/qtmaterialtable.h"
#include "qtmaterial/widgets/data/qtmaterialtreeview.h"
#include "qtmaterial/widgets/layouts/qtmaterialadaptiveshell.h"
#include "qtmaterial/widgets/layouts/qtmaterialsplitview.h"
#include "qtmaterial/widgets/inputs/qtmaterialautocomplete.h"
#include "qtmaterial/widgets/inputs/qtmaterialcombobox.h"
#include "qtmaterial/widgets/inputs/qtmaterialdatefield.h"
#include "qtmaterial/widgets/inputs/qtmaterialdaterangepicker.h"
#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"
#include "qtmaterial/widgets/inputs/qtmaterialsearchbar.h"
#include "qtmaterial/widgets/inputs/qtmaterialsearchview.h"
#include "qtmaterial/widgets/inputs/qtmaterialslider.h"
#include "qtmaterial/widgets/inputs/qtmaterialrangeslider.h"
#include "qtmaterial/widgets/inputs/qtmaterialtimefield.h"
#include "qtmaterial/widgets/inputs/qtmaterialtimepicker.h"
#include "qtmaterial/widgets/qtmaterialdatepicker.h"
#include "qtmaterial/widgets/navigation/qtmaterialbreadcrumb.h"
#include "qtmaterial/widgets/navigation/qtmaterialcommandpalette.h"
#include "qtmaterial/widgets/navigation/qtmaterialfloatingtoolbar.h"
#include "qtmaterial/widgets/navigation/qtmaterialmenu.h"
#include "qtmaterial/widgets/navigation/qtmaterialnavigationbar.h"
#include "qtmaterial/widgets/navigation/qtmaterialnavigationsuite.h"
#include "qtmaterial/widgets/navigation/qtmaterialnavigationrail.h"
#include "qtmaterial/widgets/navigation/qtmaterialtabs.h"
#include "qtmaterial/widgets/selection/qtmaterialcheckbox.h"
#include "qtmaterial/widgets/selection/qtmaterialradiobutton.h"
#include "qtmaterial/widgets/selection/qtmaterialsegmentedbutton.h"
#include "qtmaterial/widgets/selection/qtmaterialswitch.h"
#include "qtmaterial/widgets/surfaces/qtmaterialcard.h"
#include "qtmaterial/widgets/surfaces/qtmaterialdialog.h"
#include "qtmaterial/widgets/surfaces/qtmaterialnavigationdrawer.h"
#include "qtmaterial/widgets/surfaces/qtmaterialbottomsheet.h"
#include "qtmaterial/widgets/surfaces/qtmaterialsidesheet.h"
#include "qtmaterial/widgets/surfaces/qtmaterialsnackbar.h"
#include "qtmaterial/widgets/surfaces/qtmaterialtooltip.h"

#include "qtmaterialvisualtesthelpers.h"

using namespace QtMaterial;

namespace {

Theme makeTheme(ThemeMode mode, ContrastMode contrast, const QColor& seed = QColor(QStringLiteral("#6750A4")))
{
    ThemeOptions options;
    options.sourceColor = seed;
    options.mode = mode;
    options.contrast = contrast;
    options.variant = QtMaterial::ThemeVariant::TonalSpot;
    options.backendPolicy = QtMaterial::ColorBackendPolicy::ForceFallback;
    return ThemeBuilder().build(options);
}

Theme makeStaticComponentTheme(ThemeMode mode, ContrastMode contrast)
{
    Theme theme = makeTheme(mode, contrast);
    // Pixel goldens must capture a stable end state, never an animation frame.
    theme.accessibility().reducedMotion = true;
    applyReducedMotion(&theme.motion(), true);
    return theme;
}

QWidget* buildComponentGrid(const Theme& theme)
{
    ThemeManager::instance().setTheme(theme);

    auto* root = new QWidget;
    root->setObjectName(QStringLiteral("visualComponentGrid"));
    root->setAutoFillBackground(true);

    root->setPalette(
        QtMaterialPaletteAdapter::toPalette(
            theme,
            root->palette()));

    auto* layout = new QGridLayout(root);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setHorizontalSpacing(16);
    layout->setVerticalSpacing(16);

    auto* title = new QLabel(QStringLiteral("Qt Material 3 component grid"), root);
    QFont titleFont = title->font();
    titleFont.setPixelSize(22);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setAccessibleName(QStringLiteral("Component grid title"));
    layout->addWidget(title, 0, 0, 1, 4);

    auto* filled = new QtMaterialFilledButton(root);
    filled->setText(QStringLiteral("Filled"));
    filled->setAccessibleName(QStringLiteral("Filled button visual sample"));
    layout->addWidget(filled, 1, 0);

    auto* outlined = new QtMaterialOutlinedButton(root);
    outlined->setText(QStringLiteral("Outlined"));
    outlined->setAccessibleName(QStringLiteral("Outlined button visual sample"));
    layout->addWidget(outlined, 1, 1);

    auto* elevated = new QtMaterialElevatedButton(root);
    elevated->setText(QStringLiteral("Elevated"));
    elevated->setAccessibleName(QStringLiteral("Elevated button visual sample"));
    layout->addWidget(elevated, 1, 2);

    auto* text = new QtMaterialTextButton(root);
    text->setText(QStringLiteral("Text"));
    text->setAccessibleName(QStringLiteral("Text button visual sample"));
    layout->addWidget(text, 1, 3);

    auto* checkbox = new QtMaterialCheckbox(root);
    checkbox->setText(QStringLiteral("Checkbox"));
    checkbox->setChecked(true);
    checkbox->setAccessibleName(QStringLiteral("Checkbox visual sample"));
    layout->addWidget(checkbox, 2, 0);

    auto* radio = new QtMaterialRadioButton(root);
    radio->setText(QStringLiteral("Radio"));
    radio->setChecked(true);
    radio->setAccessibleName(QStringLiteral("Radio visual sample"));
    layout->addWidget(radio, 2, 1);

    auto* sw = new QtMaterialSwitch(QStringLiteral("Switch"), root);
    sw->setChecked(true);
    sw->setAccessibleName(QStringLiteral("Switch visual sample"));
    layout->addWidget(sw, 2, 2);

    auto* slider = new QSlider(Qt::Horizontal, root);
    slider->setRange(0, 100);
    slider->setValue(64);
    slider->setAccessibleName(QStringLiteral("Slider visual sample"));
    layout->addWidget(slider, 2, 3);

    auto* field = new QtMaterialOutlinedTextField(root);
    field->setText(QStringLiteral("Outlined text field"));
    field->setAccessibleName(QStringLiteral("Text field visual sample"));
    layout->addWidget(field, 3, 0, 1, 2);

    auto* combo = new QComboBox(root);
    combo->addItems({QStringLiteral("Default density"), QStringLiteral("Compact"), QStringLiteral("Comfortable")});
    combo->setAccessibleName(QStringLiteral("Combo visual sample"));
    layout->addWidget(combo, 3, 2, 1, 2);

    auto* card = new QtMaterialCard(root);
    card->setTitleText(QStringLiteral("Card surface"));
    card->setBodyText(QStringLiteral("Surface, shape, elevation, text, and outline are captured together."));
    card->setAccessibleName(QStringLiteral("Card visual sample"));
    layout->addWidget(card, 4, 0, 1, 4);

    return root;
}

void configureMatrixRoot(QWidget* root, const Theme& theme, const QString& objectName)
{
    ThemeManager::instance().setTheme(theme);
    root->setObjectName(objectName);
    root->setAutoFillBackground(true);

    root->setPalette(
        QtMaterialPaletteAdapter::toPalette(
            theme,
            root->palette()));
}

QLabel* matrixLabel(const QString& text, QWidget* parent, bool strong = false)
{
    auto* label = new QLabel(text, parent);
    if (strong) {
        QFont font = label->font();
        font.setBold(true);
        label->setFont(font);
    }
    return label;
}

void addMatrixHeaders(QGridLayout* layout, QWidget* root, const QStringList& headers)
{
    for (int column = 0; column < headers.size(); ++column) {
        layout->addWidget(matrixLabel(headers.at(column), root, true), 0, column + 1);
    }
}

void addStateLabel(QGridLayout* layout, QWidget* root, int row, const QString& label)
{
    layout->addWidget(matrixLabel(label, root, true), row, 0);
}

QWidget* buildSelectionStateMatrix(const Theme& theme)
{
    auto* root = new QWidget;
    configureMatrixRoot(root, theme, QStringLiteral("selectionStateMatrix"));

    auto* layout = new QGridLayout(root);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setHorizontalSpacing(18);
    layout->setVerticalSpacing(12);
    addMatrixHeaders(layout, root, {
        QStringLiteral("Checkbox"), QStringLiteral("Radio"),
        QStringLiteral("Switch"), QStringLiteral("Segmented")});

    const QStringList states = {
        QStringLiteral("Default"), QStringLiteral("Selected"),
        QStringLiteral("Disabled"), QStringLiteral("RTL")};
    for (int i = 0; i < states.size(); ++i) {
        addStateLabel(layout, root, i + 1, states.at(i));
    }

    for (int row = 1; row <= 4; ++row) {
        const bool selected = row != 1;
        const bool enabled = row != 3;
        const Qt::LayoutDirection direction = row == 4 ? Qt::RightToLeft : Qt::LeftToRight;

        auto* checkbox = new QtMaterialCheckbox(root);
        checkbox->setText(QStringLiteral("Option"));
        checkbox->setChecked(selected);
        checkbox->setEnabled(enabled);
        checkbox->setLayoutDirection(direction);
        layout->addWidget(checkbox, row, 1);

        auto* radio = new QtMaterialRadioButton(QStringLiteral("Option"), root);
        radio->setChecked(selected);
        radio->setEnabled(enabled);
        radio->setLayoutDirection(direction);
        layout->addWidget(radio, row, 2);

        auto* sw = new QtMaterialSwitch(QStringLiteral("Setting"), root);
        sw->setChecked(selected);
        sw->setEnabled(enabled);
        sw->setLayoutDirection(direction);
        layout->addWidget(sw, row, 3);

        auto* segmented = new QtMaterialSegmentedButton(root);
        segmented->addSegment(QStringLiteral("Day"));
        segmented->addSegment(QStringLiteral("Week"));
        segmented->addSegment(QStringLiteral("Month"));
        if (selected) {
            segmented->setCurrentIndex(1);
        }
        segmented->setEnabled(enabled);
        segmented->setLayoutDirection(direction);
        layout->addWidget(segmented, row, 4);
    }

    return root;
}

QWidget* buildInputFieldStateMatrix(const Theme& theme)
{
    auto* root = new QWidget;
    configureMatrixRoot(root, theme, QStringLiteral("inputFieldStateMatrix"));

    auto* layout = new QGridLayout(root);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setHorizontalSpacing(14);
    layout->setVerticalSpacing(12);
    addMatrixHeaders(layout, root, {
        QStringLiteral("Outlined"), QStringLiteral("Filled"), QStringLiteral("Combo"),
        QStringLiteral("Autocomplete"), QStringLiteral("Search"), QStringLiteral("Date"),
        QStringLiteral("Time")});

    const QStringList states = {
        QStringLiteral("Empty"), QStringLiteral("Value"),
        QStringLiteral("Disabled"), QStringLiteral("RTL")};
    for (int i = 0; i < states.size(); ++i) {
        addStateLabel(layout, root, i + 1, states.at(i));
    }

    for (int row = 1; row <= 4; ++row) {
        const bool hasValue = row != 1;
        const bool enabled = row != 3;
        const Qt::LayoutDirection direction = row == 4 ? Qt::RightToLeft : Qt::LeftToRight;

        auto* outlined = new QtMaterialOutlinedTextField(root);
        outlined->setLabelText(QStringLiteral("Email"));
        if (hasValue) outlined->setText(QStringLiteral("dev@example.com"));
        outlined->setEnabled(enabled);
        outlined->setLayoutDirection(direction);
        layout->addWidget(outlined, row, 1);

        auto* filled = new QtMaterialFilledTextField(root);
        filled->setLabelText(QStringLiteral("Name"));
        if (hasValue) filled->setText(QStringLiteral("Ada"));
        filled->setEnabled(enabled);
        filled->setLayoutDirection(direction);
        layout->addWidget(filled, row, 2);

        auto* combo = new QtMaterialComboBox(root);
        combo->setLabelText(QStringLiteral("Country"));
        combo->addItems({QStringLiteral("France"), QStringLiteral("Germany"), QStringLiteral("Spain")});
        combo->setCurrentIndex(hasValue ? 1 : -1);
        combo->setEnabled(enabled);
        combo->setLayoutDirection(direction);
        layout->addWidget(combo, row, 3);

        auto* autocomplete = new QtMaterialAutocomplete(root);
        autocomplete->setPlaceholderText(QStringLiteral("Project"));
        autocomplete->setSuggestions({QStringLiteral("Material"), QStringLiteral("Dashboard"), QStringLiteral("Gallery")});
        if (hasValue) autocomplete->setText(QStringLiteral("Material"));
        autocomplete->setEnabled(enabled);
        autocomplete->setLayoutDirection(direction);
        layout->addWidget(autocomplete, row, 4);

        auto* search = new QtMaterialSearchBar(root);
        search->setPlaceholderText(QStringLiteral("Search"));
        if (hasValue) search->setText(QStringLiteral("material"));
        search->setEnabled(enabled);
        search->setLayoutDirection(direction);
        layout->addWidget(search, row, 5);

        auto* date = new QtMaterialDateField(root);
        date->setLabelText(QStringLiteral("Due date"));
        date->setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
        if (hasValue) date->setDate(QDate(2026, 10, 2));
        date->setEnabled(enabled);
        date->setLayoutDirection(direction);
        layout->addWidget(date, row, 6);

        auto* time = new QtMaterialTimeField(root);
        time->setTime(hasValue ? QTime(10, 30) : QTime(0, 0));
        time->setEnabled(enabled);
        time->setLayoutDirection(direction);
        layout->addWidget(time, row, 7);
    }

    return root;
}

QWidget* buildInputCompositeStateMatrix(const Theme& theme)
{
    auto* root = new QWidget;
    configureMatrixRoot(root, theme, QStringLiteral("inputCompositeStateMatrix"));

    auto* layout = new QGridLayout(root);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setHorizontalSpacing(18);
    layout->setVerticalSpacing(12);
    addMatrixHeaders(layout, root, {
        QStringLiteral("Search View"), QStringLiteral("Date Picker"), QStringLiteral("Date Range"), QStringLiteral("Time Picker")});

    const QStringList states = {
        QStringLiteral("Value"), QStringLiteral("Disabled"), QStringLiteral("RTL")};
    for (int i = 0; i < states.size(); ++i) {
        addStateLabel(layout, root, i + 1, states.at(i));
    }

    for (int row = 1; row <= 3; ++row) {
        const bool enabled = row != 2;
        const Qt::LayoutDirection direction = row == 3 ? Qt::RightToLeft : Qt::LeftToRight;

        auto* model = new QStringListModel({
            QStringLiteral("Alpha"), QStringLiteral("Beta"), QStringLiteral("Gamma")}, root);
        auto* searchView = new QtMaterialSearchView(root);
        searchView->setSourceModel(model);
        searchView->searchBar()->setText(QStringLiteral("a"));
        searchView->setEnabled(enabled);
        searchView->setLayoutDirection(direction);
        searchView->setMinimumSize(QSize(260, 220));
        layout->addWidget(searchView, row, 1);

        auto* datePicker = new QtMaterialDatePicker(root);
        datePicker->setSelectedDate(QDate(2026, 10, 2));
        datePicker->setEnabled(enabled);
        datePicker->setLayoutDirection(direction);
        layout->addWidget(datePicker, row, 2);

        auto* dateRange = new QtMaterialDateRangePicker(root);
        dateRange->setDateRange(QDate(2026, 10, 2), QDate(2026, 10, 9));
        dateRange->setEnabled(enabled);
        dateRange->setLayoutDirection(direction);
        layout->addWidget(dateRange, row, 3);

        auto* timePicker = new QtMaterialTimePicker(root);
        timePicker->setWindowFlags(Qt::Widget);
        timePicker->setModal(false);
        timePicker->setSelectedTime(QTime(14, 35));
        timePicker->setEnabled(enabled);
        timePicker->setLayoutDirection(direction);
        timePicker->setMinimumSize(QSize(220, 140));
        layout->addWidget(timePicker, row, 4);
    }

    return root;
}

QWidget* buildNavigationPrimaryStateMatrix(const Theme& theme)
{
    auto* root = new QWidget;
    configureMatrixRoot(root, theme, QStringLiteral("navigationPrimaryStateMatrix"));

    auto* layout = new QGridLayout(root);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setHorizontalSpacing(18);
    layout->setVerticalSpacing(12);
    addMatrixHeaders(layout, root, {
        QStringLiteral("Tabs"),
        QStringLiteral("Navigation Rail"),
        QStringLiteral("Menu")});

    const QStringList states = {
        QStringLiteral("Selected"),
        QStringLiteral("Disabled"),
        QStringLiteral("RTL")};
    for (int i = 0; i < states.size(); ++i) {
        addStateLabel(layout, root, i + 1, states.at(i));
    }

    for (int row = 1; row <= 3; ++row) {
        const bool enabled = row != 2;
        const Qt::LayoutDirection direction =
            row == 3 ? Qt::RightToLeft : Qt::LeftToRight;

        auto* tabs = new QtMaterialTabs(root);
        tabs->addTab(new QWidget(tabs), QStringLiteral("Overview"));
        tabs->addTab(new QWidget(tabs), QStringLiteral("Activity"));
        tabs->addTab(new QWidget(tabs), QStringLiteral("Settings"));
        tabs->setCurrentIndex(1);
        tabs->setBadge(1, QStringLiteral("3"));
        tabs->setBadgeVisible(1, true);
        tabs->setEnabled(enabled);
        tabs->setLayoutDirection(direction);
        tabs->setMinimumSize(QSize(340, 150));
        layout->addWidget(tabs, row, 1);

        auto* rail = new QtMaterialNavigationRail(root);
        rail->addDestination(QStringLiteral("Home"));
        rail->addDestination(QStringLiteral("Search"));
        rail->addDestination(QStringLiteral("Settings"));
        rail->setCurrentIndex(1);
        rail->setEnabled(enabled);
        rail->setLayoutDirection(direction);
        layout->addWidget(rail, row, 2);

        auto* menu = new QtMaterialMenu(root);
        const int open = menu->addItem(QStringLiteral("Open"));
        menu->setItemShortcutText(open, QStringLiteral("Ctrl+O"));
        const int details = menu->addItem(QStringLiteral("Show details"));
        menu->setItemCheckable(details, true);
        menu->setItemChecked(details, true);
        menu->addSeparator();
        const int remove = menu->addItem(QStringLiteral("Delete"));
        menu->setItemEnabled(remove, false);
        menu->setCurrentIndex(details);
        menu->setEnabled(enabled);
        menu->setLayoutDirection(direction);
        layout->addWidget(menu, row, 3);
    }

    return root;
}

QWidget* buildNavigationDesktopStateMatrix(const Theme& theme)
{
    auto* root = new QWidget;
    configureMatrixRoot(root, theme, QStringLiteral("navigationDesktopStateMatrix"));

    auto* layout = new QGridLayout(root);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setHorizontalSpacing(18);
    layout->setVerticalSpacing(12);
    addMatrixHeaders(layout, root, {
        QStringLiteral("Breadcrumb"),
        QStringLiteral("Command Palette")});

    const QStringList states = {
        QStringLiteral("Default"),
        QStringLiteral("Disabled"),
        QStringLiteral("RTL")};
    for (int i = 0; i < states.size(); ++i) {
        addStateLabel(layout, root, i + 1, states.at(i));
    }

    for (int row = 1; row <= 3; ++row) {
        const bool enabled = row != 2;
        const Qt::LayoutDirection direction =
            row == 3 ? Qt::RightToLeft : Qt::LeftToRight;

        auto* breadcrumb = new QtMaterialBreadcrumb(root);
        breadcrumb->setItems({
            QStringLiteral("Workspace"),
            QStringLiteral("Requirements"),
            QStringLiteral("Subsystem"),
            QStringLiteral("Module"),
            QStringLiteral("REQ-42")});
        breadcrumb->setMaximumVisibleItems(3);
        breadcrumb->setEnabled(enabled);
        breadcrumb->setLayoutDirection(direction);
        breadcrumb->setMinimumWidth(420);
        layout->addWidget(breadcrumb, row, 1, Qt::AlignTop);

        auto* model =
            new QStandardItemModel(4, 1, root);
        const QStringList commands = {
            QStringLiteral("Open file"),
            QStringLiteral("Build project"),
            QStringLiteral("Run tests"),
            QStringLiteral("Show settings")};
        const QStringList shortcuts = {
            QStringLiteral("Ctrl+O"),
            QStringLiteral("Ctrl+B"),
            QStringLiteral("Ctrl+R"),
            QStringLiteral("Ctrl+,")};
        for (int commandRow = 0;
             commandRow < commands.size();
             ++commandRow) {
            model->setData(
                model->index(commandRow, 0),
                commands.at(commandRow));
            model->setData(
                model->index(commandRow, 0),
                shortcuts.at(commandRow),
                QtMaterialCommandPalette::ShortcutRole);
            model->setData(model->index(commandRow, 0), QStringLiteral("command.%1").arg(commandRow), QtMaterialCommandPalette::IdRole);
            model->setData(model->index(commandRow, 0), commandRow < 2 ? QStringLiteral("Workspace") : QStringLiteral("Tools"), QtMaterialCommandPalette::SectionRole);
            model->setData(model->index(commandRow, 0), QStringLiteral("Workspace action %1").arg(commandRow + 1), QtMaterialCommandPalette::SecondaryTextRole);
        }

        auto* palette =
            new QtMaterialCommandPalette(root);
        palette->setWindowFlags(Qt::Widget);
        palette->setModal(false);
        palette->setSourceModel(model);
        palette->setActivationShortcuts({});
        palette->setFavoriteCommandIds({QStringLiteral("command.0")});
        palette->setRecentCommandIds({QStringLiteral("command.1")});
        palette->setQuery(
            row == 2
                ? QStringLiteral("No match")
                : QString());
        palette->setEnabled(enabled);
        palette->setLayoutDirection(direction);
        palette->setMinimumSize(QSize(420, 220));
        layout->addWidget(palette, row, 2);
    }

    return root;
}

QWidget* buildNavigationSplitStateMatrix(const Theme& theme)
{
    auto* root = new QWidget;
    configureMatrixRoot(root, theme, QStringLiteral("navigationSplitStateMatrix"));
    auto* layout = new QGridLayout(root);
    layout->setContentsMargins(24, 24, 24, 24);
    addMatrixHeaders(layout, root, {QStringLiteral("Horizontal SplitView"), QStringLiteral("Vertical SplitView")});
    const QStringList states = {QStringLiteral("Expanded"), QStringLiteral("Collapsed"), QStringLiteral("RTL")};
    for (int row = 0; row < states.size(); ++row) {
        addStateLabel(layout, root, row + 1, states.at(row));
        for (int column = 0; column < 2; ++column) {
            auto* split = new QtMaterialSplitView(column == 0 ? Qt::Horizontal : Qt::Vertical, root);
            split->setLayoutDirection(row == 2 ? Qt::RightToLeft : Qt::LeftToRight);
            split->addWidget(new QLabel(QStringLiteral("Explorer")));
            split->addWidget(new QLabel(QStringLiteral("Editor")));
            split->addWidget(new QLabel(QStringLiteral("Inspector")));
            split->setSizes({100, 200, 100}); split->setMinimumSize(420, 160);
            split->setPaneCollapsed(0, row == 1);
            layout->addWidget(split, row + 1, column + 1);
        }
    }
    return root;
}

struct NavigationFocusFixture { QWidget* root; QWidget* target; };

NavigationFocusFixture buildNavigationFocusFixture(const Theme& theme, const QString& component)
{
    auto* root = new QWidget;
    configureMatrixRoot(root, theme, QStringLiteral("navigationFocusFixture"));
    auto* layout = new QVBoxLayout(root); layout->setContentsMargins(24, 24, 24, 24);
    QWidget* target = nullptr;
    if (component == QStringLiteral("tabs")) {
        auto* tabs = new QtMaterialTabs(root);
        tabs->addTab(new QWidget(tabs), QStringLiteral("Overview"));
        tabs->addTab(new QWidget(tabs), QStringLiteral("Activity"));
        tabs->addTab(new QWidget(tabs), QStringLiteral("Disabled"));
        tabs->setTabEnabled(2, false); tabs->setCurrentIndex(1);
        layout->addWidget(tabs); target = tabs->findChild<QTabBar*>();
    } else if (component == QStringLiteral("rail")) {
        auto* rail = new QtMaterialNavigationRail(root);
        rail->addDestination(QStringLiteral("Home")); rail->addDestination(QStringLiteral("Search"));
        rail->addDestination(QStringLiteral("Disabled")); rail->setDestinationEnabled(2, false); rail->setCurrentIndex(1);
        layout->addWidget(rail); target = rail;
    } else if (component == QStringLiteral("menu")) {
        auto* menu = new QtMaterialMenu(root);
        menu->addItem(QStringLiteral("Open")); menu->setItemShortcutText(0, QStringLiteral("Ctrl+O"));
        menu->addSeparator(); menu->addItem(QStringLiteral("Show details"));
        menu->setItemCheckable(2, true); menu->setItemChecked(2, true); menu->setCurrentIndex(2);
        layout->addWidget(menu); target = menu;
    } else if (component == QStringLiteral("breadcrumb")) {
        auto* breadcrumb = new QtMaterialBreadcrumb(root);
        breadcrumb->setItems({QStringLiteral("Workspace"), QStringLiteral("Projects"), QStringLiteral("Source")});
        breadcrumb->setLocationEditable(true); breadcrumb->setEditingLocation(true);
        layout->addWidget(breadcrumb); target = breadcrumb->findChild<QLineEdit*>();
    } else if (component == QStringLiteral("palette")) {
        auto* palette = new QtMaterialCommandPalette(root);
        palette->setWindowFlags(Qt::Widget); palette->setModal(false); palette->setActivationShortcuts({});
        auto* model = new QStandardItemModel(3, 1, root);
        const QStringList titles = {QStringLiteral("Open workspace"), QStringLiteral("Open settings"), QStringLiteral("Run tests")};
        for (int i = 0; i < titles.size(); ++i) {
            model->setData(model->index(i, 0), titles.at(i));
            model->setData(model->index(i, 0), QStringLiteral("id.%1").arg(i), QtMaterialCommandPalette::IdRole);
            model->setData(model->index(i, 0), QStringLiteral("Action description %1").arg(i + 1), QtMaterialCommandPalette::SecondaryTextRole);
        }
        palette->setSourceModel(model); palette->setFavoriteCommandIds({QStringLiteral("id.0")});
        palette->setRecentCommandIds({QStringLiteral("id.2")});
        layout->addWidget(palette); target = palette->findChild<QLineEdit*>();
    } else {
        auto* split = new QtMaterialSplitView(root);
        split->addWidget(new QLabel(QStringLiteral("Explorer")));
        split->addWidget(new QLabel(QStringLiteral("Editor")));
        split->setSizes({180, 300}); layout->addWidget(split); target = split->handle(1);
    }
    root->resize(540, 420);
    return {root, target};
}

QWidget* buildInputSliderStateMatrix(const Theme& theme)
{
    auto* root = new QWidget;
    configureMatrixRoot(root, theme, QStringLiteral("inputSliderStateMatrix"));

    auto* layout = new QGridLayout(root);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setHorizontalSpacing(24);
    layout->setVerticalSpacing(14);
    addMatrixHeaders(layout, root, {
        QStringLiteral("Slider"),
        QStringLiteral("Range Slider")});

    const QStringList states = {
        QStringLiteral("Value"),
        QStringLiteral("Disabled"),
        QStringLiteral("RTL"),
        QStringLiteral("Vertical")};
    for (int i = 0; i < states.size(); ++i) {
        addStateLabel(layout, root, i + 1, states.at(i));
    }

    for (int row = 1; row <= 4; ++row) {
        const bool enabled = row != 2;
        const bool vertical = row == 4;
        const Qt::LayoutDirection direction =
            row == 3 ? Qt::RightToLeft : Qt::LeftToRight;
        const Qt::Orientation orientation =
            vertical ? Qt::Vertical : Qt::Horizontal;

        auto* slider = new QtMaterialSlider(orientation, root);
        slider->setRange(0, 100);
        slider->setValue(64);
        slider->setEnabled(enabled);
        slider->setLayoutDirection(direction);
        slider->setMinimumSize(
            vertical ? QSize(56, 220) : QSize(260, 56));
        layout->addWidget(slider, row, 1, Qt::AlignCenter);

        auto* range = new QtMaterialRangeSlider(root);
        range->setOrientation(orientation);
        range->setRange(0, 100);
        range->setValues(25, 75);
        range->setEnabled(enabled);
        range->setLayoutDirection(direction);
        range->setMinimumSize(
            vertical ? QSize(56, 220) : QSize(280, 56));
        layout->addWidget(range, row, 2, Qt::AlignCenter);
    }

    return root;
}

QWidget* buildDesktopDataStateMatrix(const Theme& theme)
{
    auto* root = new QWidget;
    configureMatrixRoot(root, theme, QStringLiteral("desktopDataStateMatrix"));

    auto* layout = new QGridLayout(root);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setHorizontalSpacing(16);
    layout->setVerticalSpacing(14);
    addMatrixHeaders(layout, root, {
        QStringLiteral("Table"),
        QStringLiteral("Tree"),
        QStringLiteral("Pagination"),
        QStringLiteral("Split View")});

    const QStringList states = {
        QStringLiteral("Default"),
        QStringLiteral("Disabled"),
        QStringLiteral("RTL")};
    for (int i = 0; i < states.size(); ++i) {
        addStateLabel(layout, root, i + 1, states.at(i));
    }

    for (int row = 1; row <= 3; ++row) {
        const bool enabled = row != 2;
        const Qt::LayoutDirection direction =
            row == 3 ? Qt::RightToLeft : Qt::LeftToRight;

        auto* tableModel = new QStandardItemModel(3, 3, root);
        tableModel->setHorizontalHeaderLabels({
            QStringLiteral("Name"),
            QStringLiteral("State"),
            QStringLiteral("Value")});
        for (int r = 0; r < 3; ++r) {
            tableModel->setData(tableModel->index(r, 0), QStringLiteral("Item %1").arg(r + 1));
            tableModel->setData(tableModel->index(r, 1), r % 2 ? QStringLiteral("Ready") : QStringLiteral("Pending"));
            tableModel->setData(tableModel->index(r, 2), (r + 1) * 10);
        }

        auto* table = new QtMaterialTable(root);
        table->setModel(tableModel);
        table->setCurrentIndex(tableModel->index(1, 0));
        table->setEnabled(enabled);
        table->setLayoutDirection(direction);
        table->setMinimumSize(QSize(360, 180));
        layout->addWidget(table, row, 1);

        auto* treeModel = new QStandardItemModel(root);
        auto* workspace = new QStandardItem(QStringLiteral("Workspace"));
        workspace->appendRow(new QStandardItem(QStringLiteral("Requirements")));
        workspace->appendRow(new QStandardItem(QStringLiteral("Models")));
        treeModel->appendRow(workspace);

        auto* tree = new QtMaterialTreeView(root);
        tree->setModel(treeModel);
        tree->expandAll();
        tree->setCurrentIndex(treeModel->index(0, 0));
        tree->setEnabled(enabled);
        tree->setLayoutDirection(direction);
        tree->setMinimumSize(QSize(260, 180));
        layout->addWidget(tree, row, 2);

        auto* pagination = new QtMaterialPagination(root);
        pagination->setTotalCount(123);
        pagination->setPageSize(25);
        pagination->setPage(2);
        pagination->setEnabled(enabled);
        pagination->setLayoutDirection(direction);
        pagination->setMinimumWidth(360);
        layout->addWidget(pagination, row, 3, Qt::AlignTop);

        auto* split = new QtMaterialSplitView(Qt::Horizontal, root);
        auto* leftPane = matrixLabel(QStringLiteral("Tree pane"), split, true);
        auto* rightPane = matrixLabel(QStringLiteral("Editor pane"), split, true);
        leftPane->setAlignment(Qt::AlignCenter);
        rightPane->setAlignment(Qt::AlignCenter);
        split->addWidget(leftPane);
        split->addWidget(rightPane);
        split->setSizes({120, 220});
        split->setEnabled(enabled);
        split->setLayoutDirection(direction);
        split->setMinimumSize(QSize(340, 160));
        layout->addWidget(split, row, 4);
    }

    return root;
}


QWidget* buildDataExtendedStateMatrix(const Theme& theme)
{
    auto* root = new QWidget;
    configureMatrixRoot(root, theme, QStringLiteral("dataExtendedStateMatrix"));
    auto* layout = new QGridLayout(root);
    layout->setContentsMargins(24, 24, 24, 24);
    addMatrixHeaders(layout, root, {QStringLiteral("List"), QStringLiteral("Grid"), QStringLiteral("Carousel"), QStringLiteral("Divider")});
    const QStringList states = {QStringLiteral("Default"), QStringLiteral("Disabled"), QStringLiteral("RTL")};
    for (int i=0;i<states.size();++i) addStateLabel(layout, root, i+1, states.at(i));
    for (int row=1;row<=3;++row) {
        const bool enabled=row!=2; const auto dir=row==3?Qt::RightToLeft:Qt::LeftToRight;
        auto* list=new QtMaterialList(root); list->addItem(QStringLiteral("Inbox")); list->addItem(QStringLiteral("Archive")); list->setEnabled(enabled); list->setLayoutDirection(dir); list->setMinimumSize(180,120); layout->addWidget(list,row,1);
        auto* grid=new QtMaterialGridList(root); grid->setColumns(2); grid->addGridItem(QStringLiteral("One")); grid->addGridItem(QStringLiteral("Two")); grid->setEnabled(enabled); grid->setLayoutDirection(dir); grid->setMinimumSize(220,140); layout->addWidget(grid,row,2);
        auto* carousel=new QtMaterialCarousel(root); carousel->addItem(QStringLiteral("One")); carousel->addItem(QStringLiteral("Two")); carousel->setEnabled(enabled); carousel->setLayoutDirection(dir); carousel->setMinimumSize(260,140); layout->addWidget(carousel,row,3);
        auto* divider=new QtMaterialDivider(Qt::Horizontal,root); divider->setEnabled(enabled); divider->setLayoutDirection(dir); divider->setLeadingInset(20); divider->setTrailingInset(36); divider->setMinimumWidth(180); layout->addWidget(divider,row,4);
    }
    return root;
}

QWidget* buildProgressCompactStateMatrix(const Theme& theme)
{
    auto* root = new QWidget;
    configureMatrixRoot(root, theme, QStringLiteral("progressCompactStateMatrix"));
    auto* layout = new QGridLayout(root);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setHorizontalSpacing(16);
    layout->setVerticalSpacing(12);
    addMatrixHeaders(layout, root, {
        QStringLiteral("Linear"),
        QStringLiteral("Circular"),
        QStringLiteral("Assist chip"),
        QStringLiteral("Filter chip"),
        QStringLiteral("Input chip"),
        QStringLiteral("Suggestion chip")});

    const QStringList states = {
        QStringLiteral("Default"),
        QStringLiteral("Selected / 100%"),
        QStringLiteral("Disabled"),
        QStringLiteral("RTL"),
        QStringLiteral("Indeterminate")};
    for (int i = 0; i < states.size(); ++i) {
        addStateLabel(layout, root, i + 1, states.at(i));
    }

    for (int row = 1; row <= 5; ++row) {
        const bool enabled = row != 3;
        const auto direction =
            row == 4 ? Qt::RightToLeft : Qt::LeftToRight;
        const bool indeterminate = row == 5;
        const qreal value = row == 2 ? 1.0 : 0.42;

        auto* linear = new QtMaterialLinearProgressIndicator(root);
        linear->setValue(value);
        linear->setIndeterminate(indeterminate);
        linear->setEnabled(enabled);
        linear->setLayoutDirection(direction);
        linear->setMinimumWidth(220);
        layout->addWidget(linear, row, 1);

        auto* circular = new QtMaterialCircularProgressIndicator(root);
        circular->setValue(value);
        circular->setIndeterminate(indeterminate);
        circular->setEnabled(enabled);
        circular->setLayoutDirection(direction);
        layout->addWidget(circular, row, 2, Qt::AlignCenter);

        auto* assist = new QtMaterialChip(QStringLiteral("Assist"), root);
        assist->setVariant(ChipVariant::Assist);
        assist->setEnabled(enabled);
        assist->setLayoutDirection(direction);
        layout->addWidget(assist, row, 3);

        auto* filter = new QtMaterialChip(QStringLiteral("Filter"), root);
        filter->setVariant(ChipVariant::Filter);
        filter->setChecked(row == 2 || row == 4);
        filter->setEnabled(enabled);
        filter->setLayoutDirection(direction);
        layout->addWidget(filter, row, 4);

        auto* input = new QtMaterialChip(QStringLiteral("Input"), root);
        input->setVariant(ChipVariant::Input);
        input->setRemovable(true);
        input->setEnabled(enabled);
        input->setLayoutDirection(direction);
        layout->addWidget(input, row, 5);

        auto* suggestion = new QtMaterialChip(QStringLiteral("Suggestion"), root);
        suggestion->setVariant(ChipVariant::Suggestion);
        suggestion->setEnabled(enabled);
        suggestion->setLayoutDirection(direction);
        layout->addWidget(suggestion, row, 6);
    }

    return root;
}

QWidget* buildSurfaceBarStateMatrix(const Theme& theme)
{
    auto* root=new QWidget; configureMatrixRoot(root,theme,QStringLiteral("surfaceBarStateMatrix"));
    auto* layout=new QGridLayout(root); layout->setContentsMargins(24,24,24,24);
    addMatrixHeaders(layout,root,{QStringLiteral("Card"),QStringLiteral("Banner"),QStringLiteral("Top bar"),QStringLiteral("Bottom bar")});
    const QStringList states={QStringLiteral("Default"),QStringLiteral("Disabled"),QStringLiteral("RTL")};
    for(int i=0;i<states.size();++i)addStateLabel(layout,root,i+1,states.at(i));
    for(int row=1;row<=3;++row){
        const bool enabled=row!=2; const auto dir=row==3?Qt::RightToLeft:Qt::LeftToRight;
        auto* card=new QtMaterialCard(root); card->setTitleText(QStringLiteral("Project")); card->setBodyText(QStringLiteral("Enterprise surface")); card->setEnabled(enabled); card->setLayoutDirection(dir); card->setMinimumSize(220,100); layout->addWidget(card,row,1);
        auto* banner=new QtMaterialBanner(QStringLiteral("Offline"),QStringLiteral("Changes are saved"),root); banner->setPrimaryActionText(QStringLiteral("Retry")); banner->setEnabled(enabled); banner->setLayoutDirection(dir); banner->setMinimumWidth(300); layout->addWidget(banner,row,2);
        auto* top=new QtMaterialTopAppBar(QStringLiteral("Inbox"),root); top->setEnabled(enabled); top->setLayoutDirection(dir); top->setMinimumWidth(280); layout->addWidget(top,row,3);
        auto* bottom=new QtMaterialBottomAppBar(QStringLiteral("Home"),root); bottom->setEnabled(enabled); bottom->setLayoutDirection(dir); bottom->setMinimumWidth(280); layout->addWidget(bottom,row,4);
    }
    return root;
}

QWidget* buildSurfaceOverlayStateMatrix(const Theme& theme)
{
    auto* root = new QWidget;
    configureMatrixRoot(root, theme, QStringLiteral("surfaceOverlayStateMatrix"));
    auto* layout = new QGridLayout(root);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setHorizontalSpacing(18);
    layout->setVerticalSpacing(12);
    addMatrixHeaders(layout, root, {
        QStringLiteral("Dialog"),
        QStringLiteral("Drawer"),
        QStringLiteral("Bottom sheet"),
        QStringLiteral("Snackbar")});

    const QStringList states = {
        QStringLiteral("Open"),
        QStringLiteral("Disabled"),
        QStringLiteral("RTL")};
    for (int i = 0; i < states.size(); ++i) {
        addStateLabel(layout, root, i + 1, states.at(i));
    }

    auto makeHost = [root]() {
        auto* host = new QWidget(root);
        host->setObjectName(QStringLiteral("visualOverlayHost"));
        host->setFixedSize(320, 210);
        return host;
    };

    for (int row = 1; row <= 3; ++row) {
        const bool enabled = row != 2;
        const auto direction =
            row == 3 ? Qt::RightToLeft : Qt::LeftToRight;

        auto* dialogHost = makeHost();
        auto* dialog = new QtMaterialDialog(dialogHost);
        dialog->setTitleText(QStringLiteral("Discard changes?"));
        dialog->setSupportingText(QStringLiteral("Unsaved changes will be lost."));
        dialog->setEnabled(enabled);
        dialog->setLayoutDirection(direction);
        dialog->open();
        layout->addWidget(dialogHost, row, 1);

        auto* drawerHost = makeHost();
        auto* drawer = new QtMaterialNavigationDrawer(drawerHost);
        drawer->setEdge(
            direction == Qt::RightToLeft
                ? QtMaterialNavigationDrawer::Edge::Right
                : QtMaterialNavigationDrawer::Edge::Left);
        drawer->setEnabled(enabled);
        drawer->setLayoutDirection(direction);
        drawer->resize(250, drawerHost->height());
        drawer->open();
        layout->addWidget(drawerHost, row, 2);

        auto* sheetHost = makeHost();
        auto* sheet = new QtMaterialBottomSheet(sheetHost);
        sheet->setModal(false);
        sheet->setExpandedHeight(170);
        sheet->setCollapsedHeight(72);
        sheet->setTitleText(QStringLiteral("Share"));
        sheet->setSupportingText(QStringLiteral("Choose a destination"));
        sheet->setExpanded(true);
        sheet->setEnabled(enabled);
        sheet->setLayoutDirection(direction);
        sheet->open();
        layout->addWidget(sheetHost, row, 3);

        auto* snackbarHost = makeHost();
        auto* snackbar = new QtMaterialSnackbar(snackbarHost);
        snackbar->setText(QStringLiteral("Changes saved"));
        snackbar->setActionText(QStringLiteral("Undo"));
        snackbar->setShowDismissButton(true);
        snackbar->setDuration(SnackbarDuration::Indefinite);
        snackbar->setEnabled(enabled);
        snackbar->setLayoutDirection(direction);
        snackbar->showSnackbar();
        layout->addWidget(snackbarHost, row, 4);
    }

    return root;
}

QWidget* buildExpressiveCatalogueStateMatrix(const Theme& theme)
{
    auto* root = new QWidget;
    configureMatrixRoot(root, theme, QStringLiteral("expressiveCatalogueStateMatrix"));

    auto* layout = new QGridLayout(root);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setHorizontalSpacing(18);
    layout->setVerticalSpacing(12);
    addMatrixHeaders(layout, root, {
        QStringLiteral("Split Button"),
        QStringLiteral("Button Group"),
        QStringLiteral("Floating Toolbar"),
        QStringLiteral("Loading"),
        QStringLiteral("Segmented List"),
        QStringLiteral("Menu"),
        QStringLiteral("Chip")});

    const QStringList states = {
        QStringLiteral("Default"),
        QStringLiteral("Variant / selected"),
        QStringLiteral("Disabled"),
        QStringLiteral("RTL")};
    for (int i = 0; i < states.size(); ++i) {
        addStateLabel(layout, root, i + 1, states.at(i));
    }

    for (int row = 1; row <= 4; ++row) {
        const bool enabled = row != 3;
        const Qt::LayoutDirection direction =
            row == 4 ? Qt::RightToLeft : Qt::LeftToRight;

        auto* split = new QtMaterialSplitButton(
            row == 2 ? QStringLiteral("Publish") : QStringLiteral("Create"),
            root);
        split->setEnabled(enabled);
        split->setLayoutDirection(direction);
        split->setMinimumWidth(190);
        layout->addWidget(split, row, 1);

        auto* group = new QtMaterialButtonGroup(root);
        group->addButton(QStringLiteral("Day"));
        group->addButton(QStringLiteral("Week"));
        group->addButton(QStringLiteral("Month"));
        group->setCurrentIndex(row == 2 ? 1 : 0);
        group->setEnabled(enabled);
        group->setLayoutDirection(direction);
        layout->addWidget(group, row, 2);

        auto* toolbar = new QtMaterialFloatingToolbar(root);
        toolbar->addAction(QIcon(), QStringLiteral("Edit"));
        toolbar->addAction(QIcon(), QStringLiteral("Share"));
        toolbar->addAction(QIcon(), QStringLiteral("More"));
        toolbar->setExpanded(row != 2);
        toolbar->setEnabled(enabled);
        toolbar->setLayoutDirection(direction);
        toolbar->setMinimumWidth(180);
        layout->addWidget(toolbar, row, 3);

        auto* loading = new QtMaterialLoadingIndicator(root);
        loading->setIndicatorSize(row == 2 ? 56 : 44);
        loading->setActive(row != 2);
        loading->setEnabled(enabled);
        loading->setLayoutDirection(direction);
        layout->addWidget(loading, row, 4, Qt::AlignCenter);

        auto* segmented = new QtMaterialSegmentedList(root);
        segmented->addItem(QStringLiteral("Personal"));
        segmented->addItem(QStringLiteral("Work"));
        segmented->addItem(QStringLiteral("Shared"));
        segmented->setCurrentIndex(row == 2 ? 1 : 0);
        segmented->setEnabled(enabled);
        segmented->setLayoutDirection(direction);
        segmented->setMinimumSize(220, 190);
        layout->addWidget(segmented, row, 5);

        auto* menu = new QtMaterialMenu(root);
        const int openIndex = menu->addItem(QStringLiteral("Open"));
        const int pinIndex = menu->addItem(QStringLiteral("Pin"));
        menu->setItemCheckable(pinIndex, true);
        menu->setItemChecked(pinIndex, row == 2);
        menu->setCurrentIndex(row == 2 ? pinIndex : openIndex);
        menu->setExpressive(true);
        menu->setEnabled(enabled);
        menu->setLayoutDirection(direction);
        menu->setMinimumWidth(180);
        layout->addWidget(menu, row, 6);

        auto* chip = new QtMaterialChip(
            row == 2 ? QStringLiteral("Selected") : QStringLiteral("Filter"),
            root);
        chip->setVariant(ChipVariant::Filter);
        chip->setExpressive(true);
        chip->setChecked(row == 2);
        chip->setEnabled(enabled);
        chip->setLayoutDirection(direction);
        layout->addWidget(chip, row, 7, Qt::AlignCenter);
    }

    return root;
}

QWidget* buildMissingMaterial3StateMatrix(const Theme& theme)
{
    auto* root = new QWidget;
    configureMatrixRoot(root, theme, QStringLiteral("missingMaterial3StateMatrix"));

    auto* layout = new QGridLayout(root);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setHorizontalSpacing(18);
    layout->setVerticalSpacing(12);
    addMatrixHeaders(layout, root, {
        QStringLiteral("Navigation Bar"),
        QStringLiteral("Side Sheet"),
        QStringLiteral("Tooltip"),
        QStringLiteral("Badge")});

    const QStringList states = {
        QStringLiteral("Default"),
        QStringLiteral("Variant"),
        QStringLiteral("Disabled"),
        QStringLiteral("RTL")};
    for (int i = 0; i < states.size(); ++i) {
        addStateLabel(layout, root, i + 1, states.at(i));
    }

    for (int row = 1; row <= 4; ++row) {
        const bool enabled = row != 3;
        const Qt::LayoutDirection direction =
            row == 4 ? Qt::RightToLeft : Qt::LeftToRight;

        auto* bar = new QtMaterialNavigationBar(root);
        bar->addDestination(QStringLiteral("Home"));
        bar->addDestination(QStringLiteral("Search"));
        bar->addDestination(QStringLiteral("Settings"));
        bar->setCurrentIndex(row == 2 ? 2 : 1);
        bar->setDestinationEnabled(2, row != 3);
        bar->setLabelsVisible(row != 2);
        bar->setEnabled(enabled);
        bar->setLayoutDirection(direction);
        bar->setMinimumSize(QSize(360, 80));
        layout->addWidget(bar, row, 1);

        auto* sheetHost = new QWidget(root);
        sheetHost->setObjectName(QStringLiteral("missingMaterial3SideSheetHost"));
        sheetHost->setFixedSize(320, 190);
        auto* sheet = new QtMaterialSideSheet(sheetHost);
        sheet->setTitleText(row == 2
            ? QStringLiteral("Filters")
            : QStringLiteral("Details"));
        sheet->setEdge(
            row == 2
                ? QtMaterialSideSheet::Edge::Left
                : QtMaterialSideSheet::Edge::Right);
        sheet->setModal(row != 2);
        sheet->setEnabled(enabled);
        sheet->setLayoutDirection(direction);
        auto* sheetContent = new QVBoxLayout(sheet->contentWidget());
        auto* sheetLabel = new QLabel(
            row == 2
                ? QStringLiteral("Supporting filters")
                : QStringLiteral("Supporting content"),
            sheet->contentWidget());
        sheetContent->addWidget(sheetLabel);
        sheet->open();
        layout->addWidget(sheetHost, row, 2);

        auto* tooltip = new QtMaterialTooltip(root);
        tooltip->setWindowFlags(Qt::Widget);
        tooltip->setText(
            row == 2
                ? QStringLiteral("Longer helpful context")
                : row == 4
                    ? QStringLiteral("RTL tooltip")
                    : QStringLiteral("Helpful context"));
        tooltip->setPlacement(
            row == 2
                ? QtMaterialTooltip::Placement::Below
                : QtMaterialTooltip::Placement::Above);
        tooltip->setEnabled(enabled);
        tooltip->setLayoutDirection(direction);
        tooltip->resize(tooltip->sizeHint());
        tooltip->setMinimumSize(tooltip->sizeHint());
        tooltip->show();
        layout->addWidget(tooltip, row, 3, Qt::AlignCenter);

        auto* badge = new QtMaterialBadge(root);
        if (row == 2) {
            badge->setDot(true);
        } else {
            badge->setMaximum(99);
            badge->setCount(row == 3 ? 120 : 8);
        }
        badge->setEnabled(enabled);
        badge->setLayoutDirection(direction);
        badge->resize(badge->sizeHint());
        layout->addWidget(badge, row, 4, Qt::AlignCenter);
    }

    return root;
}

QWidget* buildAdaptiveDesktopFixture(
    const Theme& theme,
    int logicalWidth,
    Qt::LayoutDirection direction)
{
    ThemeManager::instance().setTheme(theme);

    auto* shell = new QtMaterialAdaptiveShell;
    shell->setObjectName(QStringLiteral("adaptiveDesktopVisualFixture"));
    shell->setLayoutDirection(direction);
    shell->setFixedSize(logicalWidth, 620);
    shell->setSupportingPaneWidth(360);

    auto* navigation = shell->navigationSuite();
    navigation->addDestination(QStringLiteral("Home"));
    navigation->addDestination(QStringLiteral("Search"));
    navigation->addDestination(QStringLiteral("Projects"));
    navigation->addDestination(QStringLiteral("Settings"));
    navigation->setDestinationEnabled(2, false);
    navigation->setCurrentIndex(1);

    auto configurePane = [&theme](QWidget* pane, const QString& objectName) {
        pane->setObjectName(objectName);
        pane->setAutoFillBackground(true);
        pane->setPalette(QtMaterialPaletteAdapter::toPalette(theme, pane->palette()));
    };

    auto* content = new QWidget;
    configurePane(content, QStringLiteral("adaptiveDesktopMainContent"));
    auto* contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(24, 24, 24, 24);
    auto* contentTitle = new QLabel(QStringLiteral("Main adaptive content"), content);
    QFont contentFont = contentTitle->font();
    contentFont.setPixelSize(22);
    contentFont.setBold(true);
    contentTitle->setFont(contentFont);
    auto* contentInfo = new QLabel(
        QStringLiteral("Selection is retained while navigation changes between Bar and Rail."),
        content);
    contentInfo->setWordWrap(true);
    auto* contentButton = new QtMaterialFilledButton(content);
    contentButton->setText(QStringLiteral("Primary action"));
    contentLayout->addWidget(contentTitle);
    contentLayout->addWidget(contentInfo);
    contentLayout->addWidget(contentButton, 0, Qt::AlignLeft);
    contentLayout->addStretch(1);

    auto* supporting = new QWidget;
    configurePane(supporting, QStringLiteral("adaptiveDesktopSupportingPane"));
    auto* supportingLayout = new QVBoxLayout(supporting);
    supportingLayout->setContentsMargins(20, 20, 20, 20);
    auto* supportingTitle = new QLabel(QStringLiteral("Supporting pane"), supporting);
    QFont supportingFont = supportingTitle->font();
    supportingFont.setBold(true);
    supportingTitle->setFont(supportingFont);
    auto* supportingInfo = new QLabel(
        QStringLiteral("Visible from Expanded when the main pane can keep its minimum width."),
        supporting);
    supportingInfo->setWordWrap(true);
    supportingLayout->addWidget(supportingTitle);
    supportingLayout->addWidget(supportingInfo);
    supportingLayout->addStretch(1);

    shell->setContentWidget(content);
    shell->setSupportingWidget(supporting);
    return shell;
}

QWidget* buildLayoutStateMatrix(const Theme& theme)
{
    auto* root = new QWidget;
    configureMatrixRoot(root, theme, QStringLiteral("layoutStateMatrix"));
    auto* layout = new QGridLayout(root);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setHorizontalSpacing(18);
    layout->setVerticalSpacing(12);
    addMatrixHeaders(layout, root, {
        QStringLiteral("Horizontal"), QStringLiteral("Vertical")});

    const QStringList states = {
        QStringLiteral("Default"), QStringLiteral("Collapsed"), QStringLiteral("RTL")};
    for (int i = 0; i < states.size(); ++i)
        addStateLabel(layout, root, i + 1, states.at(i));

    for (int row = 1; row <= 3; ++row) {
        const auto direction = row == 3 ? Qt::RightToLeft : Qt::LeftToRight;

        auto makePane = [](const QString& text, QWidget* parent) {
            auto* pane = new QLabel(text, parent);
            pane->setAlignment(Qt::AlignCenter);
            pane->setMinimumSize(80, 56);
            return pane;
        };

        auto* horizontal = new QtMaterialSplitView(Qt::Horizontal, root);
        horizontal->setLayoutDirection(direction);
        horizontal->setAnimatedCollapseEnabled(false);
        horizontal->addWidget(makePane(QStringLiteral("Master"), horizontal));
        horizontal->addWidget(makePane(QStringLiteral("Detail"), horizontal));
        horizontal->setDefaultPaneSizes({120, 180});
        horizontal->setSizes({120, 180});
        if (row == 2) horizontal->setPaneCollapsed(0, true);
        horizontal->setMinimumSize(320, 100);
        layout->addWidget(horizontal, row, 1);

        auto* vertical = new QtMaterialSplitView(Qt::Vertical, root);
        vertical->setLayoutDirection(direction);
        vertical->setAnimatedCollapseEnabled(false);
        vertical->addWidget(makePane(QStringLiteral("Top"), vertical));
        vertical->addWidget(makePane(QStringLiteral("Bottom"), vertical));
        vertical->setDefaultPaneSizes({70, 90});
        vertical->setSizes({70, 90});
        if (row == 2) vertical->setPaneCollapsed(1, true);
        vertical->setMinimumSize(220, 180);
        layout->addWidget(vertical, row, 2);
    }
    return root;
}

void addFamilyThemeRows(const QString& prefix)
{
    QTest::addColumn<QString>("caseName");
    QTest::addColumn<ThemeMode>("mode");
    QTest::addColumn<ContrastMode>("contrast");

    const QByteArray lightName = (prefix + QStringLiteral("_light_standard")).toLatin1();
    QTest::newRow(lightName.constData()) << prefix + QStringLiteral("_light_standard") << ThemeMode::Light << ContrastMode::Standard;
    const QByteArray darkName = (prefix + QStringLiteral("_dark_standard")).toLatin1();
    QTest::newRow(darkName.constData()) << prefix + QStringLiteral("_dark_standard") << ThemeMode::Dark << ContrastMode::Standard;
    const QByteArray highName = (prefix + QStringLiteral("_light_high")).toLatin1();
    QTest::newRow(highName.constData()) << prefix + QStringLiteral("_light_high") << ThemeMode::Light << ContrastMode::High;
}

void writeSmokeArtifact(const QString& caseName, const QImage& image, const QString& kind)
{
    QVERIFY(!image.isNull());
    const QString artifactPath = QDir(QtMaterialVisualTest::artifactsDir()).absoluteFilePath(caseName + QStringLiteral(".actual.png"));
    QString error;
    QVERIFY2(QtMaterialVisualTest::savePng(artifactPath, image, &error), qPrintable(error));
    QtMaterialVisualTest::writeManifestEntry(caseName, image, kind);
}

} // namespace

class tst_ThemeVisualRegression : public QObject {
    Q_OBJECT

private slots:
    void navigationFocusCandidateGoldens_data();
    void navigationFocusCandidateGoldens();
    void navigationSplitStateMatrixSmoke_data();
    void navigationSplitStateMatrixSmoke();
    void navigationSplitStateMatrixCandidateGoldens_data();
    void navigationSplitStateMatrixCandidateGoldens();
    void tokenBoardGoldens_data();
    void tokenBoardGoldens();
    void componentGridSmoke_data();
    void componentGridSmoke();
    void componentGridStrictGoldens_data();
    void componentGridStrictGoldens();
    void selectionStateMatrixSmoke_data();
    void selectionStateMatrixSmoke();
    void selectionStateMatrixCandidateGoldens_data();
    void selectionStateMatrixCandidateGoldens();
    void inputFieldStateMatrixSmoke_data();
    void inputFieldStateMatrixSmoke();
    void inputFieldStateMatrixCandidateGoldens_data();
    void inputFieldStateMatrixCandidateGoldens();
    void inputCompositeStateMatrixSmoke_data();
    void inputCompositeStateMatrixSmoke();
    void inputCompositeStateMatrixCandidateGoldens_data();
    void inputCompositeStateMatrixCandidateGoldens();
    void navigationPrimaryStateMatrixSmoke_data();
    void navigationPrimaryStateMatrixSmoke();
    void navigationPrimaryStateMatrixCandidateGoldens_data();
    void navigationPrimaryStateMatrixCandidateGoldens();
    void navigationDesktopStateMatrixSmoke_data();
    void navigationDesktopStateMatrixSmoke();
    void navigationDesktopStateMatrixCandidateGoldens_data();
    void navigationDesktopStateMatrixCandidateGoldens();
    void inputSliderStateMatrixSmoke_data();
    void inputSliderStateMatrixSmoke();
    void inputSliderStateMatrixCandidateGoldens_data();
    void inputSliderStateMatrixCandidateGoldens();
    void desktopDataStateMatrixSmoke_data();
    void desktopDataStateMatrixSmoke();
    void desktopDataStateMatrixCandidateGoldens_data();
    void desktopDataStateMatrixCandidateGoldens();
    void dataExtendedStateMatrixSmoke_data(); void dataExtendedStateMatrixSmoke(); void dataExtendedStateMatrixCandidateGoldens_data(); void dataExtendedStateMatrixCandidateGoldens();
    void progressCompactStateMatrixSmoke_data(); void progressCompactStateMatrixSmoke(); void progressCompactStateMatrixCandidateGoldens_data(); void progressCompactStateMatrixCandidateGoldens();
    void surfaceBarStateMatrixSmoke_data(); void surfaceBarStateMatrixSmoke(); void surfaceBarStateMatrixCandidateGoldens_data(); void surfaceBarStateMatrixCandidateGoldens();
    void surfaceOverlayStateMatrixSmoke_data(); void surfaceOverlayStateMatrixSmoke(); void surfaceOverlayStateMatrixCandidateGoldens_data(); void surfaceOverlayStateMatrixCandidateGoldens();
    void missingMaterial3StateMatrixSmoke_data(); void missingMaterial3StateMatrixSmoke(); void missingMaterial3StateMatrixCandidateGoldens_data(); void missingMaterial3StateMatrixCandidateGoldens();
    void expressiveCatalogueStateMatrixSmoke_data(); void expressiveCatalogueStateMatrixSmoke(); void expressiveCatalogueStateMatrixCandidateGoldens_data(); void expressiveCatalogueStateMatrixCandidateGoldens();
    void adaptiveDesktopCandidateGoldens_data();
    void adaptiveDesktopCandidateGoldens();
    void layoutStateMatrixSmoke_data(); void layoutStateMatrixSmoke(); void layoutStateMatrixCandidateGoldens_data(); void layoutStateMatrixCandidateGoldens();

};

void tst_ThemeVisualRegression::tokenBoardGoldens_data()
{
    QTest::addColumn<QString>("caseName");
    QTest::addColumn<ThemeMode>("mode");
    QTest::addColumn<ContrastMode>("contrast");
    QTest::addColumn<QColor>("seed");

    QTest::newRow("seed_6750A4_light_standard") << QStringLiteral("token_seed_6750A4_light_standard") << ThemeMode::Light << ContrastMode::Standard << QColor(QStringLiteral("#6750A4"));
    QTest::newRow("seed_6750A4_dark_standard") << QStringLiteral("token_seed_6750A4_dark_standard") << ThemeMode::Dark << ContrastMode::Standard << QColor(QStringLiteral("#6750A4"));
    QTest::newRow("seed_00639B_light_high") << QStringLiteral("token_seed_00639B_light_high") << ThemeMode::Light << ContrastMode::High << QColor(QStringLiteral("#00639B"));
    QTest::newRow("seed_FF0000_dark_high") << QStringLiteral("token_seed_FF0000_dark_high") << ThemeMode::Dark << ContrastMode::High << QColor(QStringLiteral("#FF0000"));
}

void tst_ThemeVisualRegression::tokenBoardGoldens()
{
    QFETCH(QString, caseName);
    QFETCH(ThemeMode, mode);
    QFETCH(ContrastMode, contrast);
    QFETCH(QColor, seed);

    const Theme theme = makeTheme(mode, contrast, seed);
    const QImage image = QtMaterialVisualTest::renderTokenBoard(theme);
    QCOMPARE(image.size(), QSize(720, 420));
    QtMaterialVisualTest::verifyOrUpdateGolden(caseName, image);
}

void tst_ThemeVisualRegression::componentGridSmoke_data()
{
    QTest::addColumn<QString>("caseName");
    QTest::addColumn<ThemeMode>("mode");
    QTest::addColumn<ContrastMode>("contrast");

    QTest::newRow("light_standard") << QStringLiteral("component_grid_light_standard") << ThemeMode::Light << ContrastMode::Standard;
    QTest::newRow("dark_standard") << QStringLiteral("component_grid_dark_standard") << ThemeMode::Dark << ContrastMode::Standard;
    QTest::newRow("light_high") << QStringLiteral("component_grid_light_high") << ThemeMode::Light << ContrastMode::High;
}

void tst_ThemeVisualRegression::componentGridSmoke()
{
    QFETCH(QString, caseName);
    QFETCH(ThemeMode, mode);
    QFETCH(ContrastMode, contrast);

    Theme theme = makeStaticComponentTheme(mode, contrast);
    std::unique_ptr<QWidget> grid(buildComponentGrid(theme));
    const QImage image = QtMaterialVisualTest::renderWidget(grid.get());
    QVERIFY(!image.isNull());
    QVERIFY(image.width() >= 360);
    QVERIFY(image.height() >= 220);

    const QString artifactPath = QDir(QtMaterialVisualTest::artifactsDir()).absoluteFilePath(caseName + QStringLiteral(".actual.png"));
    QString error;
    QVERIFY2(QtMaterialVisualTest::savePng(artifactPath, image, &error), qPrintable(error));
    QtMaterialVisualTest::writeManifestEntry(caseName, image, QStringLiteral("widget-smoke"));
}

void tst_ThemeVisualRegression::componentGridStrictGoldens_data()
{
    componentGridSmoke_data();
}

void tst_ThemeVisualRegression::componentGridStrictGoldens()
{
    if (!QtMaterialVisualTest::strictGoldens() && !QtMaterialVisualTest::updateGoldens()) {
        QSKIP("Component-widget pixel goldens are opt-in. Set QTMATERIAL3_VISUAL_STRICT=1 to compare or QTMATERIAL3_UPDATE_VISUAL_GOLDENS=1 to create.");
    }

    QFETCH(QString, caseName);
    QFETCH(ThemeMode, mode);
    QFETCH(ContrastMode, contrast);

    Theme theme = makeStaticComponentTheme(mode, contrast);
    std::unique_ptr<QWidget> grid(buildComponentGrid(theme));
    const QImage image = QtMaterialVisualTest::renderWidget(grid.get());
    QtMaterialVisualTest::verifyOrUpdateGolden(caseName, image);
}

void tst_ThemeVisualRegression::selectionStateMatrixSmoke_data()
{
    addFamilyThemeRows(QStringLiteral("selection_matrix"));
}

void tst_ThemeVisualRegression::selectionStateMatrixSmoke()
{
    QFETCH(QString, caseName);
    QFETCH(ThemeMode, mode);
    QFETCH(ContrastMode, contrast);

    const Theme theme = makeStaticComponentTheme(mode, contrast);
    std::unique_ptr<QWidget> matrix(buildSelectionStateMatrix(theme));
    writeSmokeArtifact(caseName, QtMaterialVisualTest::renderWidget(matrix.get()), QStringLiteral("selection-state-matrix"));
}

void tst_ThemeVisualRegression::selectionStateMatrixCandidateGoldens_data()
{
    selectionStateMatrixSmoke_data();
}

void tst_ThemeVisualRegression::selectionStateMatrixCandidateGoldens()
{
    if (!QtMaterialVisualTest::strictGoldens() && !QtMaterialVisualTest::updateGoldens()) {
        QSKIP("Family matrix goldens are opt-in.");
    }

    QFETCH(QString, caseName);
    QFETCH(ThemeMode, mode);
    QFETCH(ContrastMode, contrast);

    const Theme theme = makeStaticComponentTheme(mode, contrast);
    std::unique_ptr<QWidget> matrix(buildSelectionStateMatrix(theme));
    QtMaterialVisualTest::verifyOrUpdateCandidateGolden(caseName, QtMaterialVisualTest::renderWidget(matrix.get()));
}

void tst_ThemeVisualRegression::inputFieldStateMatrixSmoke_data()
{
    addFamilyThemeRows(QStringLiteral("input_field_matrix"));
}

void tst_ThemeVisualRegression::inputFieldStateMatrixSmoke()
{
    QFETCH(QString, caseName);
    QFETCH(ThemeMode, mode);
    QFETCH(ContrastMode, contrast);

    const Theme theme = makeStaticComponentTheme(mode, contrast);
    std::unique_ptr<QWidget> matrix(buildInputFieldStateMatrix(theme));
    writeSmokeArtifact(caseName, QtMaterialVisualTest::renderWidget(matrix.get()), QStringLiteral("input-field-state-matrix"));
}

void tst_ThemeVisualRegression::inputFieldStateMatrixCandidateGoldens_data()
{
    inputFieldStateMatrixSmoke_data();
}

void tst_ThemeVisualRegression::inputFieldStateMatrixCandidateGoldens()
{
    if (!QtMaterialVisualTest::strictGoldens() && !QtMaterialVisualTest::updateGoldens()) {
        QSKIP("Family matrix goldens are opt-in.");
    }

    QFETCH(QString, caseName);
    QFETCH(ThemeMode, mode);
    QFETCH(ContrastMode, contrast);

    const Theme theme = makeStaticComponentTheme(mode, contrast);
    std::unique_ptr<QWidget> matrix(buildInputFieldStateMatrix(theme));
    QtMaterialVisualTest::verifyOrUpdateCandidateGolden(caseName, QtMaterialVisualTest::renderWidget(matrix.get()));
}

void tst_ThemeVisualRegression::inputCompositeStateMatrixSmoke_data()
{
    addFamilyThemeRows(QStringLiteral("input_composite_matrix"));
}

void tst_ThemeVisualRegression::inputCompositeStateMatrixSmoke()
{
    QFETCH(QString, caseName);
    QFETCH(ThemeMode, mode);
    QFETCH(ContrastMode, contrast);

    const Theme theme = makeStaticComponentTheme(mode, contrast);
    std::unique_ptr<QWidget> matrix(buildInputCompositeStateMatrix(theme));
    writeSmokeArtifact(caseName, QtMaterialVisualTest::renderWidget(matrix.get()), QStringLiteral("input-composite-state-matrix"));
}

void tst_ThemeVisualRegression::inputCompositeStateMatrixCandidateGoldens_data()
{
    inputCompositeStateMatrixSmoke_data();
}

void tst_ThemeVisualRegression::inputCompositeStateMatrixCandidateGoldens()
{
    if (!QtMaterialVisualTest::strictGoldens() && !QtMaterialVisualTest::updateGoldens()) {
        QSKIP("Family matrix goldens are opt-in.");
    }

    QFETCH(QString, caseName);
    QFETCH(ThemeMode, mode);
    QFETCH(ContrastMode, contrast);

    const Theme theme = makeStaticComponentTheme(mode, contrast);
    std::unique_ptr<QWidget> matrix(buildInputCompositeStateMatrix(theme));
    QtMaterialVisualTest::verifyOrUpdateCandidateGolden(caseName, QtMaterialVisualTest::renderWidget(matrix.get()));
}

void tst_ThemeVisualRegression::navigationPrimaryStateMatrixSmoke_data()
{
    addFamilyThemeRows(QStringLiteral("navigation_primary_matrix"));
}

void tst_ThemeVisualRegression::navigationPrimaryStateMatrixSmoke()
{
    QFETCH(QString, caseName);
    QFETCH(ThemeMode, mode);
    QFETCH(ContrastMode, contrast);

    const Theme theme = makeStaticComponentTheme(mode, contrast);
    std::unique_ptr<QWidget> matrix(buildNavigationPrimaryStateMatrix(theme));
    writeSmokeArtifact(
        caseName,
        QtMaterialVisualTest::renderWidget(matrix.get()),
        QStringLiteral("navigation-primary-state-matrix"));
}

void tst_ThemeVisualRegression::navigationPrimaryStateMatrixCandidateGoldens_data()
{
    navigationPrimaryStateMatrixSmoke_data();
}

void tst_ThemeVisualRegression::navigationPrimaryStateMatrixCandidateGoldens()
{
    if (!QtMaterialVisualTest::strictGoldens() && !QtMaterialVisualTest::updateGoldens()) {
        QSKIP("Family matrix goldens are opt-in.");
    }

    QFETCH(QString, caseName);
    QFETCH(ThemeMode, mode);
    QFETCH(ContrastMode, contrast);

    const Theme theme = makeStaticComponentTheme(mode, contrast);
    std::unique_ptr<QWidget> matrix(buildNavigationPrimaryStateMatrix(theme));
    QtMaterialVisualTest::verifyOrUpdateCandidateGolden(
        caseName,
        QtMaterialVisualTest::renderWidget(matrix.get()));
}

void tst_ThemeVisualRegression::navigationDesktopStateMatrixSmoke_data()
{
    addFamilyThemeRows(QStringLiteral("navigation_desktop_matrix"));
}

void tst_ThemeVisualRegression::navigationDesktopStateMatrixSmoke()
{
    QFETCH(QString, caseName);
    QFETCH(ThemeMode, mode);
    QFETCH(ContrastMode, contrast);

    const Theme theme = makeStaticComponentTheme(mode, contrast);
    std::unique_ptr<QWidget> matrix(buildNavigationDesktopStateMatrix(theme));
    writeSmokeArtifact(
        caseName,
        QtMaterialVisualTest::renderWidget(matrix.get()),
        QStringLiteral("navigation-desktop-state-matrix"));
}

void tst_ThemeVisualRegression::navigationDesktopStateMatrixCandidateGoldens_data()
{
    navigationDesktopStateMatrixSmoke_data();
}

void tst_ThemeVisualRegression::navigationDesktopStateMatrixCandidateGoldens()
{
    if (!QtMaterialVisualTest::strictGoldens() && !QtMaterialVisualTest::updateGoldens()) {
        QSKIP("Family matrix goldens are opt-in.");
    }

    QFETCH(QString, caseName);
    QFETCH(ThemeMode, mode);
    QFETCH(ContrastMode, contrast);

    const Theme theme = makeStaticComponentTheme(mode, contrast);
    std::unique_ptr<QWidget> matrix(buildNavigationDesktopStateMatrix(theme));
    QtMaterialVisualTest::verifyOrUpdateCandidateGolden(
        caseName,
        QtMaterialVisualTest::renderWidget(matrix.get()));
}

void tst_ThemeVisualRegression::inputSliderStateMatrixSmoke_data()
{
    addFamilyThemeRows(QStringLiteral("input_slider_matrix"));
}

void tst_ThemeVisualRegression::inputSliderStateMatrixSmoke()
{
    QFETCH(QString, caseName);
    QFETCH(ThemeMode, mode);
    QFETCH(ContrastMode, contrast);

    const Theme theme = makeStaticComponentTheme(mode, contrast);
    std::unique_ptr<QWidget> matrix(buildInputSliderStateMatrix(theme));
    writeSmokeArtifact(
        caseName,
        QtMaterialVisualTest::renderWidget(matrix.get()),
        QStringLiteral("input-slider-state-matrix"));
}

void tst_ThemeVisualRegression::inputSliderStateMatrixCandidateGoldens_data()
{
    inputSliderStateMatrixSmoke_data();
}

void tst_ThemeVisualRegression::inputSliderStateMatrixCandidateGoldens()
{
    if (!QtMaterialVisualTest::strictGoldens() && !QtMaterialVisualTest::updateGoldens()) {
        QSKIP("Family matrix goldens are opt-in.");
    }

    QFETCH(QString, caseName);
    QFETCH(ThemeMode, mode);
    QFETCH(ContrastMode, contrast);

    const Theme theme = makeStaticComponentTheme(mode, contrast);
    std::unique_ptr<QWidget> matrix(buildInputSliderStateMatrix(theme));
    QtMaterialVisualTest::verifyOrUpdateCandidateGolden(
        caseName,
        QtMaterialVisualTest::renderWidget(matrix.get()));
}

void tst_ThemeVisualRegression::desktopDataStateMatrixSmoke_data()
{
    addFamilyThemeRows(QStringLiteral("desktop_data_matrix"));
}

void tst_ThemeVisualRegression::desktopDataStateMatrixSmoke()
{
    QFETCH(QString, caseName);
    QFETCH(ThemeMode, mode);
    QFETCH(ContrastMode, contrast);

    const Theme theme = makeStaticComponentTheme(mode, contrast);
    std::unique_ptr<QWidget> matrix(buildDesktopDataStateMatrix(theme));
    writeSmokeArtifact(
        caseName,
        QtMaterialVisualTest::renderWidget(matrix.get()),
        QStringLiteral("desktop-data-state-matrix"));
}

void tst_ThemeVisualRegression::desktopDataStateMatrixCandidateGoldens_data()
{
    desktopDataStateMatrixSmoke_data();
}

void tst_ThemeVisualRegression::desktopDataStateMatrixCandidateGoldens()
{
    if (!QtMaterialVisualTest::strictGoldens() && !QtMaterialVisualTest::updateGoldens()) {
        QSKIP("Family matrix goldens are opt-in.");
    }

    QFETCH(QString, caseName);
    QFETCH(ThemeMode, mode);
    QFETCH(ContrastMode, contrast);

    const Theme theme = makeStaticComponentTheme(mode, contrast);
    std::unique_ptr<QWidget> matrix(buildDesktopDataStateMatrix(theme));
    QtMaterialVisualTest::verifyOrUpdateCandidateGolden(
        caseName,
        QtMaterialVisualTest::renderWidget(matrix.get()));
}


#define QTM3_MATRIX_CASES(Name, Prefix, Builder, Kind) \
void tst_ThemeVisualRegression::Name##Smoke_data(){ addFamilyThemeRows(QStringLiteral(Prefix)); } \
void tst_ThemeVisualRegression::Name##Smoke(){ QFETCH(QString,caseName); QFETCH(ThemeMode,mode); QFETCH(ContrastMode,contrast); const Theme theme=makeStaticComponentTheme(mode,contrast); std::unique_ptr<QWidget> matrix(Builder(theme)); writeSmokeArtifact(caseName,QtMaterialVisualTest::renderWidget(matrix.get()),QStringLiteral(Kind)); } \
void tst_ThemeVisualRegression::Name##CandidateGoldens_data(){ Name##Smoke_data(); } \
void tst_ThemeVisualRegression::Name##CandidateGoldens(){ if(!QtMaterialVisualTest::strictGoldens()&&!QtMaterialVisualTest::updateGoldens()) QSKIP("Family matrix goldens are opt-in."); QFETCH(QString,caseName); QFETCH(ThemeMode,mode); QFETCH(ContrastMode,contrast); const Theme theme=makeStaticComponentTheme(mode,contrast); std::unique_ptr<QWidget> matrix(Builder(theme)); QtMaterialVisualTest::verifyOrUpdateCandidateGolden(caseName,QtMaterialVisualTest::renderWidget(matrix.get())); }

QTM3_MATRIX_CASES(dataExtendedStateMatrix, "data_extended_matrix", buildDataExtendedStateMatrix, "data-extended-state-matrix")
QTM3_MATRIX_CASES(navigationSplitStateMatrix, "navigation_split_matrix", buildNavigationSplitStateMatrix, "navigation-split-state-matrix")
QTM3_MATRIX_CASES(progressCompactStateMatrix, "progress_compact_matrix", buildProgressCompactStateMatrix, "progress-compact-state-matrix")
QTM3_MATRIX_CASES(surfaceBarStateMatrix, "surface_bar_matrix", buildSurfaceBarStateMatrix, "surface-bar-state-matrix")
QTM3_MATRIX_CASES(surfaceOverlayStateMatrix, "surface_overlay_matrix", buildSurfaceOverlayStateMatrix, "surface-overlay-state-matrix")
QTM3_MATRIX_CASES(missingMaterial3StateMatrix, "missing_material3_matrix", buildMissingMaterial3StateMatrix, "missing-material3-state-matrix")
QTM3_MATRIX_CASES(expressiveCatalogueStateMatrix, "expressive_catalogue_matrix", buildExpressiveCatalogueStateMatrix, "expressive-catalogue-state-matrix")
QTM3_MATRIX_CASES(layoutStateMatrix, "layout_matrix", buildLayoutStateMatrix, "layout-state-matrix")
#undef QTM3_MATRIX_CASES

void tst_ThemeVisualRegression::adaptiveDesktopCandidateGoldens_data()
{
    QTest::addColumn<QString>("caseName");
    QTest::addColumn<ThemeMode>("mode");
    QTest::addColumn<ContrastMode>("contrast");
    QTest::addColumn<int>("logicalWidth");
    QTest::addColumn<int>("direction");

    struct WidthCase {
        const char* name;
        int width;
    };
    const WidthCase widths[] = {
        {"compact", 520},
        {"medium", 720},
        {"expanded", 1000},
        {"large", 1280},
        {"extra_large", 1640},
    };

    struct ThemeCase {
        const char* name;
        ThemeMode mode;
        ContrastMode contrast;
    };
    const ThemeCase themes[] = {
        {"light_standard", ThemeMode::Light, ContrastMode::Standard},
        {"dark_standard", ThemeMode::Dark, ContrastMode::Standard},
        {"light_high", ThemeMode::Light, ContrastMode::High},
    };

    for (const WidthCase& widthCase : widths) {
        for (int direction : {int(Qt::LeftToRight), int(Qt::RightToLeft)}) {
            const QString directionName =
                direction == int(Qt::RightToLeft)
                    ? QStringLiteral("rtl")
                    : QStringLiteral("ltr");
            for (const ThemeCase& themeCase : themes) {
                const QString name =
                    QStringLiteral("adaptive_desktop_%1_%2_%3")
                        .arg(
                            QString::fromLatin1(widthCase.name),
                            directionName,
                            QString::fromLatin1(themeCase.name));
                QTest::newRow(qPrintable(name))
                    << name
                    << themeCase.mode
                    << themeCase.contrast
                    << widthCase.width
                    << direction;
            }
        }
    }
}

void tst_ThemeVisualRegression::adaptiveDesktopCandidateGoldens()
{
    if (!qEnvironmentVariableIsSet("QTMATERIAL3_ADAPTIVE_VISUAL")) {
        QSKIP("Adaptive/Desktop visual evidence runs only through its dedicated CTest target.");
    }

    if (!QtMaterialVisualTest::strictGoldens()
        && !QtMaterialVisualTest::updateGoldens()) {
        QSKIP("Adaptive/Desktop goldens are opt-in.");
    }

    QFETCH(QString, caseName);
    QFETCH(ThemeMode, mode);
    QFETCH(ContrastMode, contrast);
    QFETCH(int, logicalWidth);
    QFETCH(int, direction);

    const Theme theme = makeStaticComponentTheme(mode, contrast);
    std::unique_ptr<QWidget> fixture(
        buildAdaptiveDesktopFixture(
            theme,
            logicalWidth,
            static_cast<Qt::LayoutDirection>(direction)));
    QtMaterialVisualTest::verifyOrUpdateCandidateGolden(
        caseName,
        QtMaterialVisualTest::renderWidget(fixture.get()));
}

void tst_ThemeVisualRegression::navigationFocusCandidateGoldens_data()
{
    QTest::addColumn<QString>("caseName"); QTest::addColumn<QString>("component");
    QTest::addColumn<ThemeMode>("mode"); QTest::addColumn<ContrastMode>("contrast");
    const QStringList components = {QStringLiteral("tabs"), QStringLiteral("rail"), QStringLiteral("menu"),
        QStringLiteral("breadcrumb"), QStringLiteral("palette"), QStringLiteral("split")};
    for (const QString& component : components) {
        for (int variant = 0; variant < 3; ++variant) {
            const QString suffix = variant == 0 ? QStringLiteral("light_standard") : variant == 1 ? QStringLiteral("dark_standard") : QStringLiteral("light_high");
            const QString name = QStringLiteral("navigation_focus_%1_%2").arg(component, suffix);
            QTest::newRow(qPrintable(name)) << name << component << (variant == 1 ? ThemeMode::Dark : ThemeMode::Light)
                << (variant == 2 ? ContrastMode::High : ContrastMode::Standard);
        }
    }
}

void tst_ThemeVisualRegression::navigationFocusCandidateGoldens()
{
    if (!QtMaterialVisualTest::strictGoldens() && !QtMaterialVisualTest::updateGoldens()) { QSKIP("Focus goldens are opt-in."); }
    QFETCH(QString, caseName); QFETCH(QString, component); QFETCH(ThemeMode, mode); QFETCH(ContrastMode, contrast);
    struct CursorGuard {
        int previous = QApplication::cursorFlashTime();
        CursorGuard() { QApplication::setCursorFlashTime(0); }
        ~CursorGuard() { QApplication::setCursorFlashTime(previous); }
    } cursorGuard;
    const auto fixture = buildNavigationFocusFixture(makeStaticComponentTheme(mode, contrast), component);
    std::unique_ptr<QWidget> root(fixture.root);
    QVERIFY(fixture.target);
    root->resize(root->sizeHint().expandedTo(QSize(720, 420)));
    QCursor::setPos(0, 0); // Keep native hover outside the focused reference fixture.
    root->show(); QVERIFY(QTest::qWaitForWindowExposed(root.get()));
    root->activateWindow(); fixture.target->setFocus(Qt::TabFocusReason);
    QTRY_VERIFY(fixture.target->hasFocus());
    QCoreApplication::processEvents();
    QtMaterialVisualTest::verifyOrUpdateCandidateGolden(caseName, root->grab().toImage());
}

QTEST_MAIN(tst_ThemeVisualRegression)
#include "tst_theme_visual_regression.moc"
