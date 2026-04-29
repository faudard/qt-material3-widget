#include "qtmaterial/core/qtmaterialcomponentregistry.h"

#include <QStringList>

namespace QtMaterial {

QString componentMaturityToString(ComponentMaturity maturity)
{
    switch (maturity) {
    case ComponentMaturity::Complete: return QStringLiteral("complete");
    case ComponentMaturity::Usable: return QStringLiteral("usable");
    case ComponentMaturity::Partial: return QStringLiteral("partial");
    case ComponentMaturity::Skeleton: return QStringLiteral("skeleton");
    case ComponentMaturity::Planned: return QStringLiteral("planned");
    }
    return QStringLiteral("planned");
}

ComponentMaturity componentMaturityFromString(const QString& value)
{
    const QString normalized = value.trimmed().toLower();
    if (normalized == QLatin1String("complete")) return ComponentMaturity::Complete;
    if (normalized == QLatin1String("usable")) return ComponentMaturity::Usable;
    if (normalized == QLatin1String("partial")) return ComponentMaturity::Partial;
    if (normalized == QLatin1String("skeleton")) return ComponentMaturity::Skeleton;
    return ComponentMaturity::Planned;
}

bool isReleaseBlockingMaturity(ComponentMaturity maturity) noexcept
{
    return maturity == ComponentMaturity::Skeleton || maturity == ComponentMaturity::Planned;
}

QVector<ComponentDescriptor> defaultComponentRegistry()
{
    return QVector<ComponentDescriptor>{
        {QStringLiteral("button.text"), QStringLiteral("Text Button"), QStringLiteral("Buttons"), QStringLiteral("qtmaterial/widgets/qtmaterialtextbutton.h"), QStringLiteral("QtMaterial3::Widgets"), QStringLiteral("ButtonSpec"), QStringLiteral("QtMaterialTextButton"), QStringLiteral("docs/public-api/buttons.md"), QStringLiteral("tst_textbutton"), QStringLiteral("/buttons/text"), ComponentMaturity::Usable},
        {QStringLiteral("button.filled"), QStringLiteral("Filled Button"), QStringLiteral("Buttons"), QStringLiteral("qtmaterial/widgets/qtmaterialfilledbutton.h"), QStringLiteral("QtMaterial3::Widgets"), QStringLiteral("ButtonSpec"), QStringLiteral("QtMaterialFilledButton"), QStringLiteral("docs/public-api/buttons.md"), QStringLiteral("tst_filledbutton"), QStringLiteral("/buttons/filled"), ComponentMaturity::Usable},
        {QStringLiteral("button.outlined"), QStringLiteral("Outlined Button"), QStringLiteral("Buttons"), QStringLiteral("qtmaterial/widgets/qtmaterialoutlinedbutton.h"), QStringLiteral("QtMaterial3::Widgets"), QStringLiteral("ButtonSpec"), QStringLiteral("QtMaterialOutlinedButton"), QStringLiteral("docs/public-api/buttons.md"), QStringLiteral("tst_outlinedbutton"), QStringLiteral("/buttons/outlined"), ComponentMaturity::Usable},
        {QStringLiteral("button.icon"), QStringLiteral("Icon Button"), QStringLiteral("Buttons"), QStringLiteral("qtmaterial/widgets/qtmaterialiconbutton.h"), QStringLiteral("QtMaterial3::Widgets"), QStringLiteral("IconButtonSpec"), QStringLiteral("QtMaterialIconButton"), QStringLiteral("docs/public-api/buttons.md"), QStringLiteral("tst_iconbutton"), QStringLiteral("/buttons/icon"), ComponentMaturity::Partial},
        {QStringLiteral("button.fab"), QStringLiteral("FAB"), QStringLiteral("Buttons"), QStringLiteral("qtmaterial/widgets/qtmaterialfab.h"), QStringLiteral("QtMaterial3::Widgets"), QStringLiteral("FabSpec"), QStringLiteral("QtMaterialFab"), QStringLiteral("docs/public-api/buttons.md"), QStringLiteral("tst_fab"), QStringLiteral("/buttons/fab"), ComponentMaturity::Partial},
        {QStringLiteral("input.text.outlined"), QStringLiteral("Outlined Text Field"), QStringLiteral("Inputs"), QStringLiteral("qtmaterial/widgets/qtmaterialoutlinedtextfield.h"), QStringLiteral("QtMaterial3::Widgets"), QStringLiteral("TextFieldSpec"), QStringLiteral("QtMaterialOutlinedTextField"), QStringLiteral("docs/public-api/inputs.md"), QStringLiteral("tst_outlinedtextfield"), QStringLiteral("/inputs/outlined-text-field"), ComponentMaturity::Partial},
        {QStringLiteral("input.text.filled"), QStringLiteral("Filled Text Field"), QStringLiteral("Inputs"), QStringLiteral("qtmaterial/widgets/qtmaterialfilledtextfield.h"), QStringLiteral("QtMaterial3::Widgets"), QStringLiteral("TextFieldSpec"), QStringLiteral("QtMaterialFilledTextField"), QStringLiteral("docs/public-api/inputs.md"), QStringLiteral("tst_filledtextfield"), QStringLiteral("/inputs/filled-text-field"), ComponentMaturity::Partial},
        {QStringLiteral("selection.checkbox"), QStringLiteral("Checkbox"), QStringLiteral("Selection"), QStringLiteral("qtmaterial/widgets/qtmaterialcheckbox.h"), QStringLiteral("QtMaterial3::Widgets"), QStringLiteral("CheckboxSpec"), QStringLiteral("QtMaterialCheckbox"), QStringLiteral("docs/public-api/selection.md"), QStringLiteral("tst_checkbox"), QStringLiteral("/selection/checkbox"), ComponentMaturity::Usable},
        {QStringLiteral("selection.radio"), QStringLiteral("Radio Button"), QStringLiteral("Selection"), QStringLiteral("qtmaterial/widgets/qtmaterialradiobutton.h"), QStringLiteral("QtMaterial3::Widgets"), QStringLiteral("RadioButtonSpec"), QStringLiteral("QtMaterialRadioButton"), QStringLiteral("docs/public-api/selection.md"), QStringLiteral("tst_radiobutton"), QStringLiteral("/selection/radio"), ComponentMaturity::Usable},
        {QStringLiteral("selection.switch"), QStringLiteral("Switch"), QStringLiteral("Selection"), QStringLiteral("qtmaterial/widgets/qtmaterialswitch.h"), QStringLiteral("QtMaterial3::Widgets"), QStringLiteral("SwitchSpec"), QStringLiteral("QtMaterialSwitch"), QStringLiteral("docs/public-api/selection.md"), QStringLiteral("tst_switch"), QStringLiteral("/selection/switch"), ComponentMaturity::Usable},
        {QStringLiteral("surface.dialog"), QStringLiteral("Dialog"), QStringLiteral("Surfaces"), QStringLiteral("qtmaterial/widgets/qtmaterialdialog.h"), QStringLiteral("QtMaterial3::Widgets"), QStringLiteral("DialogSpec"), QStringLiteral("QtMaterialDialog"), QStringLiteral("docs/public-api/surfaces.md"), QStringLiteral("tst_dialog"), QStringLiteral("/surfaces/dialog"), ComponentMaturity::Partial},
        {QStringLiteral("surface.card"), QStringLiteral("Card"), QStringLiteral("Surfaces"), QStringLiteral("qtmaterial/widgets/qtmaterialcard.h"), QStringLiteral("QtMaterial3::Widgets"), QStringLiteral("CardSpec"), QStringLiteral("QtMaterialCard"), QStringLiteral("docs/public-api/surfaces.md"), QStringLiteral("tst_card"), QStringLiteral("/surfaces/card"), ComponentMaturity::Partial},
        {QStringLiteral("surface.snackbar"), QStringLiteral("Snackbar"), QStringLiteral("Surfaces"), QStringLiteral("qtmaterial/widgets/qtmaterialsnackbar.h"), QStringLiteral("QtMaterial3::Widgets"), QStringLiteral("SnackbarSpec"), QStringLiteral("QtMaterialSnackbar"), QStringLiteral("docs/public-api/surfaces.md"), QStringLiteral("tst_snackbar"), QStringLiteral("/surfaces/snackbar"), ComponentMaturity::Partial},
        {QStringLiteral("navigation.tabs"), QStringLiteral("Tabs"), QStringLiteral("Navigation"), QStringLiteral("qtmaterial/widgets/qtmaterialtabs.h"), QStringLiteral("QtMaterial3::Widgets"), QStringLiteral("TabsSpec"), QStringLiteral("QtMaterialTabs"), QStringLiteral("docs/public-api/navigation.md"), QStringLiteral("tst_tabs"), QStringLiteral("/navigation/tabs"), ComponentMaturity::Partial},
        {QStringLiteral("progress.linear"), QStringLiteral("Linear Progress Indicator"), QStringLiteral("Progress"), QStringLiteral("qtmaterial/widgets/qtmaterialprogressindicator.h"), QStringLiteral("QtMaterial3::Widgets"), QStringLiteral("ProgressIndicatorSpec"), QStringLiteral("QtMaterialLinearProgressIndicator"), QStringLiteral("docs/public-api/progress.md"), QStringLiteral("tst_progressindicators"), QStringLiteral("/progress/linear"), ComponentMaturity::Partial},
        {QStringLiteral("data.divider"), QStringLiteral("Divider"), QStringLiteral("Data"), QStringLiteral("qtmaterial/widgets/qtmaterialdivider.h"), QStringLiteral("QtMaterial3::Widgets"), QStringLiteral("DividerSpec"), QStringLiteral("QtMaterialDivider"), QStringLiteral("docs/public-api/data.md"), QStringLiteral("tst_divider"), QStringLiteral("/data/divider"), ComponentMaturity::Usable},
        {QStringLiteral("navigation.rail"), QStringLiteral("Navigation Rail"), QStringLiteral("Navigation"), QStringLiteral("qtmaterial/widgets/qtmaterialnavigationrail.h"), QStringLiteral("QtMaterial3::Widgets"), QStringLiteral("NavigationRailSpec"), QStringLiteral("QtMaterialNavigationRail"), QStringLiteral("docs/public-api/navigation.md"), QString(), QStringLiteral("/navigation/rail"), ComponentMaturity::Planned},
        {QStringLiteral("compact.chip"), QStringLiteral("Chip"), QStringLiteral("Compact controls"), QStringLiteral("qtmaterial/widgets/qtmaterialchip.h"), QStringLiteral("QtMaterial3::Widgets"), QStringLiteral("ChipSpec"), QStringLiteral("QtMaterialChip"), QStringLiteral("docs/public-api/compact-controls.md"), QString(), QStringLiteral("/compact/chip"), ComponentMaturity::Planned},
    };
}

const ComponentDescriptor* findComponentDescriptor(const QVector<ComponentDescriptor>& registry, const QString& id)
{
    for (const ComponentDescriptor& descriptor : registry) {
        if (descriptor.id == id) {
            return &descriptor;
        }
    }
    return nullptr;
}

} // namespace QtMaterial
