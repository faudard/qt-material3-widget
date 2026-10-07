#include "gallerycatalog.h"

const QVector<GalleryComponentEntry>& galleryComponentCatalog()
{
    static const QVector<GalleryComponentEntry> entries = {
        { QStringLiteral("button.elevated"), QStringLiteral("Elevated Button"), QStringLiteral("Buttons"), QStringLiteral("/buttons/elevated"), QStringLiteral("QtMaterialElevatedButton"), QStringLiteral("qtmaterial/widgets/buttons/qtmaterialelevatedbutton.h") },
        { QStringLiteral("button.extended-fab"), QStringLiteral("Extended Floating Action Button"), QStringLiteral("Buttons"), QStringLiteral("/buttons/extended-fab"), QStringLiteral("QtMaterialExtendedFab"), QStringLiteral("qtmaterial/widgets/buttons/qtmaterialextendedfab.h") },
        { QStringLiteral("button.fab"), QStringLiteral("Floating Action Button"), QStringLiteral("Buttons"), QStringLiteral("/buttons/fab"), QStringLiteral("QtMaterialFab"), QStringLiteral("qtmaterial/widgets/buttons/qtmaterialfab.h") },
        { QStringLiteral("button.filled"), QStringLiteral("Filled Button"), QStringLiteral("Buttons"), QStringLiteral("/buttons/filled"), QStringLiteral("QtMaterialFilledButton"), QStringLiteral("qtmaterial/widgets/buttons/qtmaterialfilledbutton.h") },
        { QStringLiteral("button.filled-tonal"), QStringLiteral("Filled Tonal Button"), QStringLiteral("Buttons"), QStringLiteral("/buttons/filled-tonal"), QStringLiteral("QtMaterialFilledTonalButton"), QStringLiteral("qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h") },
        { QStringLiteral("button.icon"), QStringLiteral("Icon Button"), QStringLiteral("Buttons"), QStringLiteral("/buttons/icon"), QStringLiteral("QtMaterialIconButton"), QStringLiteral("qtmaterial/widgets/buttons/qtmaterialiconbutton.h") },
        { QStringLiteral("button.outlined"), QStringLiteral("Outlined Button"), QStringLiteral("Buttons"), QStringLiteral("/buttons/outlined"), QStringLiteral("QtMaterialOutlinedButton"), QStringLiteral("qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h") },
        { QStringLiteral("button.text"), QStringLiteral("Text Button"), QStringLiteral("Buttons"), QStringLiteral("/buttons/text"), QStringLiteral("QtMaterialTextButton"), QStringLiteral("qtmaterial/widgets/buttons/qtmaterialtextbutton.h") },
        { QStringLiteral("compact.chip"), QStringLiteral("Chip"), QStringLiteral("Compact controls"), QStringLiteral("/compact/chip"), QStringLiteral("QtMaterialChip"), QStringLiteral("qtmaterial/widgets/selection/qtmaterialchip.h") },
        { QStringLiteral("data.carousel"), QStringLiteral("Carousel"), QStringLiteral("Data"), QStringLiteral("/data/carousel"), QStringLiteral("QtMaterialCarousel"), QStringLiteral("qtmaterial/widgets/data/qtmaterialcarousel.h") },
        { QStringLiteral("data.divider"), QStringLiteral("Divider"), QStringLiteral("Data display"), QStringLiteral("/data/divider"), QStringLiteral("QtMaterialDivider"), QStringLiteral("qtmaterial/widgets/data/qtmaterialdivider.h") },
        { QStringLiteral("data.grid-list"), QStringLiteral("Grid List"), QStringLiteral("Data"), QStringLiteral("/data/grid-list"), QStringLiteral("QtMaterialGridList"), QStringLiteral("qtmaterial/widgets/data/qtmaterialgridlist.h") },
        { QStringLiteral("data.table"), QStringLiteral("Table"), QStringLiteral("Data"), QStringLiteral("/data/table"), QStringLiteral("QtMaterialTable"), QStringLiteral("qtmaterial/widgets/data/qtmaterialtable.h") },
        { QStringLiteral("input.combo-box"), QStringLiteral("Combo Box"), QStringLiteral("Inputs"), QStringLiteral("/inputs/combo-box"), QStringLiteral("QtMaterialComboBox"), QStringLiteral("qtmaterial/widgets/inputs/qtmaterialcombobox.h") },
        { QStringLiteral("input.date-range-picker"), QStringLiteral("Date Range Picker"), QStringLiteral("Inputs"), QStringLiteral("/inputs/date-range-picker"), QStringLiteral("QtMaterialDateRangePicker"), QStringLiteral("qtmaterial/widgets/inputs/qtmaterialdaterangepicker.h") },
        { QStringLiteral("input.date.field"), QStringLiteral("Date Field"), QStringLiteral("Inputs"), QStringLiteral("/inputs/date-field"), QStringLiteral("QtMaterialDateField"), QStringLiteral("qtmaterial/widgets/inputs/qtmaterialdatefield.h") },
        { QStringLiteral("input.range-slider"), QStringLiteral("Range Slider"), QStringLiteral("Inputs"), QStringLiteral("/inputs/range-slider"), QStringLiteral("QtMaterialRangeSlider"), QStringLiteral("qtmaterial/widgets/inputs/qtmaterialrangeslider.h") },
        { QStringLiteral("input.search-bar"), QStringLiteral("Search Bar"), QStringLiteral("Inputs"), QStringLiteral("/inputs/search-bar"), QStringLiteral("QtMaterialSearchBar"), QStringLiteral("qtmaterial/widgets/inputs/qtmaterialsearchbar.h") },
        { QStringLiteral("input.search-view"), QStringLiteral("Search View"), QStringLiteral("Inputs"), QStringLiteral("/inputs/search-view"), QStringLiteral("QtMaterialSearchView"), QStringLiteral("qtmaterial/widgets/inputs/qtmaterialsearchview.h") },
        { QStringLiteral("input.slider"), QStringLiteral("Slider"), QStringLiteral("Inputs"), QStringLiteral("/inputs/slider"), QStringLiteral("QtMaterialSlider"), QStringLiteral("qtmaterial/widgets/inputs/qtmaterialslider.h") },
        { QStringLiteral("input.text.filled"), QStringLiteral("Filled Text Field"), QStringLiteral("Inputs"), QStringLiteral("/inputs/filled-text-field"), QStringLiteral("QtMaterialFilledTextField"), QStringLiteral("qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h") },
        { QStringLiteral("input.text.outlined"), QStringLiteral("Outlined Text Field"), QStringLiteral("Inputs"), QStringLiteral("/inputs/outlined-text-field"), QStringLiteral("QtMaterialOutlinedTextField"), QStringLiteral("qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h") },
        { QStringLiteral("input.time-field"), QStringLiteral("Time Field"), QStringLiteral("Inputs"), QStringLiteral("/inputs/time-field"), QStringLiteral("QtMaterialTimeField"), QStringLiteral("qtmaterial/widgets/inputs/qtmaterialtimefield.h") },
        { QStringLiteral("input.time-picker"), QStringLiteral("Time Picker"), QStringLiteral("Inputs"), QStringLiteral("/inputs/time-picker"), QStringLiteral("QtMaterialTimePicker"), QStringLiteral("qtmaterial/widgets/inputs/qtmaterialtimepicker.h") },
        { QStringLiteral("navigation.rail"), QStringLiteral("Navigation Rail"), QStringLiteral("Navigation"), QStringLiteral("/navigation/rail"), QStringLiteral("QtMaterialNavigationRail"), QStringLiteral("qtmaterial/widgets/navigation/qtmaterialnavigationrail.h") },
        { QStringLiteral("navigation.tabs"), QStringLiteral("Tabs"), QStringLiteral("Navigation"), QStringLiteral("/navigation/tabs"), QStringLiteral("QtMaterialTabs"), QStringLiteral("qtmaterial/widgets/navigation/qtmaterialtabs.h") },
        { QStringLiteral("progress.circular"), QStringLiteral("Circular Progress Indicator"), QStringLiteral("Progress"), QStringLiteral("/progress/circular"), QStringLiteral("QtMaterialCircularProgressIndicator"), QStringLiteral("qtmaterial/widgets/progress/qtmaterialcircularprogressindicator.h") },
        { QStringLiteral("progress.linear"), QStringLiteral("Linear Progress Indicator"), QStringLiteral("Progress"), QStringLiteral("/progress/linear"), QStringLiteral("QtMaterialLinearProgressIndicator"), QStringLiteral("qtmaterial/widgets/progress/qtmateriallinearprogressindicator.h") },
        { QStringLiteral("selection.checkbox"), QStringLiteral("Checkbox"), QStringLiteral("Selection"), QStringLiteral("/selection/checkbox"), QStringLiteral("QtMaterialCheckbox"), QStringLiteral("qtmaterial/widgets/selection/qtmaterialcheckbox.h") },
        { QStringLiteral("selection.radio"), QStringLiteral("Radio Button"), QStringLiteral("Selection"), QStringLiteral("/selection/radio"), QStringLiteral("QtMaterialRadioButton"), QStringLiteral("qtmaterial/widgets/selection/qtmaterialradiobutton.h") },
        { QStringLiteral("selection.segmented-button"), QStringLiteral("Segmented Button"), QStringLiteral("Selection"), QStringLiteral("/selection/segmented-button"), QStringLiteral("QtMaterialSegmentedButton"), QStringLiteral("qtmaterial/widgets/selection/qtmaterialsegmentedbutton.h") },
        { QStringLiteral("selection.switch"), QStringLiteral("Switch"), QStringLiteral("Selection"), QStringLiteral("/selection/switch"), QStringLiteral("QtMaterialSwitch"), QStringLiteral("qtmaterial/widgets/selection/qtmaterialswitch.h") },
        { QStringLiteral("surface.banner"), QStringLiteral("Banner"), QStringLiteral("Surfaces"), QStringLiteral("/surfaces/banner"), QStringLiteral("QtMaterialBanner"), QStringLiteral("qtmaterial/widgets/surfaces/qtmaterialbanner.h") },
        { QStringLiteral("surface.bottom-app-bar"), QStringLiteral("Bottom App Bar"), QStringLiteral("Surfaces"), QStringLiteral("/surfaces/bottom-app-bar"), QStringLiteral("QtMaterialBottomAppBar"), QStringLiteral("qtmaterial/widgets/surfaces/qtmaterialbottomappbar.h") },
        { QStringLiteral("surface.bottom-sheet"), QStringLiteral("Bottom Sheet"), QStringLiteral("Surfaces"), QStringLiteral("/surfaces/bottom-sheet"), QStringLiteral("QtMaterialBottomSheet"), QStringLiteral("qtmaterial/widgets/surfaces/qtmaterialbottomsheet.h") },
        { QStringLiteral("surface.card"), QStringLiteral("Card"), QStringLiteral("Surfaces"), QStringLiteral("/surfaces/card"), QStringLiteral("QtMaterialCard"), QStringLiteral("qtmaterial/widgets/surfaces/qtmaterialcard.h") },
        { QStringLiteral("surface.dialog"), QStringLiteral("Dialog"), QStringLiteral("Surfaces"), QStringLiteral("/surfaces/dialog"), QStringLiteral("QtMaterialDialog"), QStringLiteral("qtmaterial/widgets/surfaces/qtmaterialdialog.h") },
        { QStringLiteral("surface.navigation-drawer"), QStringLiteral("Navigation Drawer"), QStringLiteral("Surfaces"), QStringLiteral("/surfaces/navigation-drawer"), QStringLiteral("QtMaterialNavigationDrawer"), QStringLiteral("qtmaterial/widgets/surfaces/qtmaterialnavigationdrawer.h") },
        { QStringLiteral("surface.snackbar"), QStringLiteral("Snackbar"), QStringLiteral("Surfaces"), QStringLiteral("/surfaces/snackbar"), QStringLiteral("QtMaterialSnackbar"), QStringLiteral("qtmaterial/widgets/surfaces/qtmaterialsnackbar.h") },
        { QStringLiteral("surface.top-app-bar"), QStringLiteral("Top App Bar"), QStringLiteral("Surfaces"), QStringLiteral("/surfaces/top-app-bar"), QStringLiteral("QtMaterialTopAppBar"), QStringLiteral("qtmaterial/widgets/surfaces/qtmaterialtopappbar.h") },
        { QStringLiteral("data.tree-view"), QStringLiteral("Tree View"), QStringLiteral("Data"), QStringLiteral("/data/tree-view"), QStringLiteral("QtMaterialTreeView"), QStringLiteral("qtmaterial/widgets/data/qtmaterialtreeview.h") },
        { QStringLiteral("data.pagination"), QStringLiteral("Pagination"), QStringLiteral("Data"), QStringLiteral("/data/pagination"), QStringLiteral("QtMaterialPagination"), QStringLiteral("qtmaterial/widgets/data/qtmaterialpagination.h") },
        { QStringLiteral("layout.split-view"), QStringLiteral("Split View"), QStringLiteral("Layouts"), QStringLiteral("/layouts/split-view"), QStringLiteral("QtMaterialSplitView"), QStringLiteral("qtmaterial/widgets/layouts/qtmaterialsplitview.h") },
        { QStringLiteral("navigation.breadcrumb"), QStringLiteral("Breadcrumb"), QStringLiteral("Navigation"), QStringLiteral("/navigation/breadcrumb"), QStringLiteral("QtMaterialBreadcrumb"), QStringLiteral("qtmaterial/widgets/navigation/qtmaterialbreadcrumb.h") },
        { QStringLiteral("navigation.command-palette"), QStringLiteral("Command Palette"), QStringLiteral("Navigation"), QStringLiteral("/navigation/command-palette"), QStringLiteral("QtMaterialCommandPalette"), QStringLiteral("qtmaterial/widgets/navigation/qtmaterialcommandpalette.h") },
        { QStringLiteral("data.list"), QStringLiteral("List"), QStringLiteral("Data"), QStringLiteral("/data/list"), QStringLiteral("QtMaterialList"), QStringLiteral("qtmaterial/widgets/data/qtmateriallist.h") },
        { QStringLiteral("input.autocomplete"), QStringLiteral("Autocomplete"), QStringLiteral("Inputs"), QStringLiteral("/inputs/autocomplete"), QStringLiteral("QtMaterialAutocomplete"), QStringLiteral("qtmaterial/widgets/inputs/qtmaterialautocomplete.h") },
        { QStringLiteral("input.date-picker"), QStringLiteral("Date Picker"), QStringLiteral("Inputs"), QStringLiteral("/inputs/date-picker"), QStringLiteral("QtMaterialDatePicker"), QStringLiteral("qtmaterial/widgets/qtmaterialdatepicker.h") },
        { QStringLiteral("navigation.menu"), QStringLiteral("Menu"), QStringLiteral("Navigation"), QStringLiteral("/navigation/menu"), QStringLiteral("QtMaterialMenu"), QStringLiteral("qtmaterial/widgets/navigation/qtmaterialmenu.h") },
        { QStringLiteral("navigation.bar"), QStringLiteral("Navigation Bar"), QStringLiteral("Navigation"), QStringLiteral("/navigation/bar"), QStringLiteral("QtMaterialNavigationBar"), QStringLiteral("qtmaterial/widgets/navigation/qtmaterialnavigationbar.h") },
        { QStringLiteral("surface.side-sheet"), QStringLiteral("Side Sheet"), QStringLiteral("Surfaces"), QStringLiteral("/surfaces/side-sheet"), QStringLiteral("QtMaterialSideSheet"), QStringLiteral("qtmaterial/widgets/surfaces/qtmaterialsidesheet.h") },
        { QStringLiteral("surface.tooltip"), QStringLiteral("Tooltip"), QStringLiteral("Surfaces"), QStringLiteral("/surfaces/tooltip"), QStringLiteral("QtMaterialTooltip"), QStringLiteral("qtmaterial/widgets/surfaces/qtmaterialtooltip.h") },
        { QStringLiteral("data.badge"), QStringLiteral("Badge"), QStringLiteral("Data display"), QStringLiteral("/surfaces/badge"), QStringLiteral("QtMaterialBadge"), QStringLiteral("qtmaterial/widgets/data/qtmaterialbadge.h") },
        { QStringLiteral("layout.adaptive-shell"), QStringLiteral("Adaptive Shell"), QStringLiteral("Layouts"), QStringLiteral("/layouts/adaptive-shell"), QStringLiteral("QtMaterialAdaptiveShell"), QStringLiteral("qtmaterial/widgets/layouts/qtmaterialadaptiveshell.h") },
        { QStringLiteral("navigation.suite"), QStringLiteral("Navigation Suite"), QStringLiteral("Navigation"), QStringLiteral("/navigation/suite"), QStringLiteral("QtMaterialNavigationSuite"), QStringLiteral("qtmaterial/widgets/navigation/qtmaterialnavigationsuite.h") },
        { QStringLiteral("button.split"), QStringLiteral("Split Button"), QStringLiteral("Buttons"), QStringLiteral("/buttons/split-button"), QStringLiteral("QtMaterialSplitButton"), QStringLiteral("qtmaterial/widgets/buttons/qtmaterialsplitbutton.h") },
        { QStringLiteral("button.group"), QStringLiteral("Button Group"), QStringLiteral("Buttons"), QStringLiteral("/buttons/button-group"), QStringLiteral("QtMaterialButtonGroup"), QStringLiteral("qtmaterial/widgets/buttons/qtmaterialbuttongroup.h") },
        { QStringLiteral("navigation.floating-toolbar"), QStringLiteral("Floating Toolbar"), QStringLiteral("Navigation"), QStringLiteral("/navigation/floating-toolbar"), QStringLiteral("QtMaterialFloatingToolbar"), QStringLiteral("qtmaterial/widgets/navigation/qtmaterialfloatingtoolbar.h") },
        { QStringLiteral("progress.loading-indicator"), QStringLiteral("Loading Indicator"), QStringLiteral("Progress"), QStringLiteral("/progress/loading-indicator"), QStringLiteral("QtMaterialLoadingIndicator"), QStringLiteral("qtmaterial/widgets/progress/qtmaterialloadingindicator.h") },
        { QStringLiteral("data.segmented-list"), QStringLiteral("Segmented List"), QStringLiteral("Data"), QStringLiteral("/data/segmented-list"), QStringLiteral("QtMaterialSegmentedList"), QStringLiteral("qtmaterial/widgets/data/qtmaterialsegmentedlist.h") }
    };
    return entries;
}

int galleryTabForRoute(const QString& route)
{
    if (route.startsWith(QStringLiteral("/buttons"))) return 0;
    if (route.startsWith(QStringLiteral("/selection"))) return 1;
    if (route.startsWith(QStringLiteral("/surfaces"))) return 2;
    if (route.startsWith(QStringLiteral("/inputs"))) return 3;
    if (route.startsWith(QStringLiteral("/navigation"))) return 4;
    if (route.startsWith(QStringLiteral("/data"))) return 5;
    if (route.startsWith(QStringLiteral("/compact"))) return 5;
    if (route.startsWith(QStringLiteral("/progress"))) return 9;
    if (route.startsWith(QStringLiteral("/layouts"))) return 7;
    return 0;
}
