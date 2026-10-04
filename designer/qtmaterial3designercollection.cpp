#include "qtmaterial3designercollection.h"

#include <functional>
#include <utility>

#include <QIcon>
#include <QString>
#include <QWidget>
#include <QtUiPlugin/QDesignerCustomWidgetInterface>

#include "qtmaterial/widgets/buttons/qtmaterialelevatedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"
#include "qtmaterial/widgets/data/qtmaterialdivider.h"
#include "qtmaterial/widgets/data/qtmaterialpagination.h"
#include "qtmaterial/widgets/data/qtmaterialtable.h"
#include "qtmaterial/widgets/data/qtmaterialtreeview.h"
#include "qtmaterial/widgets/inputs/qtmaterialcombobox.h"
#include "qtmaterial/widgets/inputs/qtmaterialdatefield.h"
#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"
#include "qtmaterial/widgets/inputs/qtmaterialrangeslider.h"
#include "qtmaterial/widgets/inputs/qtmaterialsearchbar.h"
#include "qtmaterial/widgets/inputs/qtmaterialslider.h"
#include "qtmaterial/widgets/navigation/qtmaterialbreadcrumb.h"
#include "qtmaterial/widgets/navigation/qtmaterialtabs.h"
#include "qtmaterial/widgets/progress/qtmaterialcircularprogressindicator.h"
#include "qtmaterial/widgets/progress/qtmateriallinearprogressindicator.h"
#include "qtmaterial/widgets/selection/qtmaterialcheckbox.h"
#include "qtmaterial/widgets/selection/qtmaterialchip.h"
#include "qtmaterial/widgets/selection/qtmaterialradiobutton.h"
#include "qtmaterial/widgets/selection/qtmaterialswitch.h"
#include "qtmaterial/widgets/surfaces/qtmaterialbottomappbar.h"
#include "qtmaterial/widgets/surfaces/qtmaterialcard.h"
#include "qtmaterial/widgets/surfaces/qtmaterialtopappbar.h"

namespace {

class WidgetInterface final : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    using Factory = std::function<QWidget*(QWidget*)>;

    WidgetInterface(QString className,
                    QString includeFile,
                    QString group,
                    QString objectName,
                    QString toolTip,
                    bool container,
                    Factory factory,
                    QString defaultPropertiesXml,
                    QObject* parent)
        : QObject(parent)
        , className_(std::move(className))
        , includeFile_(std::move(includeFile))
        , group_(std::move(group))
        , objectName_(std::move(objectName))
        , toolTip_(std::move(toolTip))
        , container_(container)
        , factory_(std::move(factory))
        , defaultPropertiesXml_(std::move(defaultPropertiesXml))
    {
    }

    QString name() const override { return className_; }
    QString group() const override { return group_; }
    QString toolTip() const override { return toolTip_; }
    QString whatsThis() const override { return toolTip_; }
    QString includeFile() const override { return includeFile_; }
    QIcon icon() const override { return {}; }
    bool isContainer() const override { return container_; }
    bool isInitialized() const override { return initialized_; }

    QWidget* createWidget(QWidget* parent) override
    {
        QWidget* widget = factory_(parent);
        widget->setObjectName(objectName_);
        return widget;
    }

    void initialize(QDesignerFormEditorInterface*) override
    {
        initialized_ = true;
    }

    QString domXml() const override
    {
        const QString properties = QStringLiteral(
            "<property name=\"geometry\">"
            "<rect><x>0</x><y>0</y><width>180</width><height>48</height></rect>"
            "</property>"
            "<property name=\"toolTip\"><string>%1</string></property>%2")
            .arg(toolTip_.toHtmlEscaped(), defaultPropertiesXml_);

        return QStringLiteral(
            "<ui language=\"c++\">"
            "<widget class=\"%1\" name=\"%2\">%3</widget>"
            "</ui>")
            .arg(className_, objectName_, properties);
    }

private:
    QString className_;
    QString includeFile_;
    QString group_;
    QString objectName_;
    QString toolTip_;
    bool container_ = false;
    bool initialized_ = false;
    Factory factory_;
    QString defaultPropertiesXml_;
};

template <typename T>
void addWidget(QList<QDesignerCustomWidgetInterface*>& widgets,
               QObject* owner,
               const char* className,
               const char* includeFile,
               const char* group,
               const char* objectName,
               const char* toolTip,
               bool container = false,
               const char* defaultPropertiesXml = "")
{
    widgets.append(new WidgetInterface(
        QString::fromLatin1(className),
        QString::fromLatin1(includeFile),
        QString::fromLatin1(group),
        QString::fromLatin1(objectName),
        QString::fromLatin1(toolTip),
        container,
        [](QWidget* parent) { return new T(parent); },
        QString::fromLatin1(defaultPropertiesXml),
        owner));
}

} // namespace

QtMaterial3DesignerCollection::QtMaterial3DesignerCollection(QObject* parent)
    : QObject(parent)
{
    using namespace QtMaterial;

    addWidget<QtMaterialFilledButton>(
        widgets_, this, "QtMaterial::QtMaterialFilledButton",
        "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h",
        "Qt Material 3 - Buttons", "materialFilledButton", "Material 3 filled button", false,
        "<property name=\"text\"><string>Filled button</string></property>");
    addWidget<QtMaterialFilledTonalButton>(
        widgets_, this, "QtMaterial::QtMaterialFilledTonalButton",
        "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h",
        "Qt Material 3 - Buttons", "materialFilledTonalButton", "Material 3 filled tonal button", false,
        "<property name=\"text\"><string>Filled tonal button</string></property>");
    addWidget<QtMaterialOutlinedButton>(
        widgets_, this, "QtMaterial::QtMaterialOutlinedButton",
        "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h",
        "Qt Material 3 - Buttons", "materialOutlinedButton", "Material 3 outlined button", false,
        "<property name=\"text\"><string>Outlined button</string></property>");
    addWidget<QtMaterialElevatedButton>(
        widgets_, this, "QtMaterial::QtMaterialElevatedButton",
        "qtmaterial/widgets/buttons/qtmaterialelevatedbutton.h",
        "Qt Material 3 - Buttons", "materialElevatedButton", "Material 3 elevated button", false,
        "<property name=\"text\"><string>Elevated button</string></property>");
    addWidget<QtMaterialTextButton>(
        widgets_, this, "QtMaterial::QtMaterialTextButton",
        "qtmaterial/widgets/buttons/qtmaterialtextbutton.h",
        "Qt Material 3 - Buttons", "materialTextButton", "Material 3 text button", false,
        "<property name=\"text\"><string>Text button</string></property>");

    addWidget<QtMaterialComboBox>(
        widgets_, this, "QtMaterial::QtMaterialComboBox",
        "qtmaterial/widgets/inputs/qtmaterialcombobox.h",
        "Qt Material 3 - Inputs", "materialComboBox", "Material 3 combo box", false,
        "<property name=\"labelText\"><string>Option</string></property>");
    addWidget<QtMaterialSlider>(
        widgets_, this, "QtMaterial::QtMaterialSlider",
        "qtmaterial/widgets/inputs/qtmaterialslider.h",
        "Qt Material 3 - Inputs", "materialSlider", "Material 3 slider", false,
        "<property name=\"minimum\"><number>0</number></property>"
        "<property name=\"maximum\"><number>100</number></property>"
        "<property name=\"value\"><number>40</number></property>");
    addWidget<QtMaterialRangeSlider>(
        widgets_, this, "QtMaterial::QtMaterialRangeSlider",
        "qtmaterial/widgets/inputs/qtmaterialrangeslider.h",
        "Qt Material 3 - Inputs", "materialRangeSlider", "Material 3 range slider", false,
        "<property name=\"minimum\"><number>0</number></property>"
        "<property name=\"maximum\"><number>100</number></property>"
        "<property name=\"lowerValue\"><number>25</number></property>"
        "<property name=\"upperValue\"><number>75</number></property>");
    addWidget<QtMaterialOutlinedTextField>(
        widgets_, this, "QtMaterial::QtMaterialOutlinedTextField",
        "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h",
        "Qt Material 3 - Inputs", "materialOutlinedTextField", "Material 3 outlined text field", false,
        "<property name=\"label\"><string>Label</string></property>"
        "<property name=\"placeholderText\"><string>Enter text</string></property>");
    addWidget<QtMaterialFilledTextField>(
        widgets_, this, "QtMaterial::QtMaterialFilledTextField",
        "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h",
        "Qt Material 3 - Inputs", "materialFilledTextField", "Material 3 filled text field", false,
        "<property name=\"label\"><string>Label</string></property>"
        "<property name=\"placeholderText\"><string>Enter text</string></property>");
    addWidget<QtMaterialDateField>(
        widgets_, this, "QtMaterialDateField",
        "qtmaterial/widgets/inputs/qtmaterialdatefield.h",
        "Qt Material 3 - Inputs", "materialDateField", "Material 3 date field", false,
        "<property name=\"label\"><string>Date</string></property>"
        "<property name=\"displayFormat\"><string>yyyy-MM-dd</string></property>"
        "<property name=\"clearable\"><bool>true</bool></property>");
    addWidget<QtMaterialSearchBar>(
        widgets_, this, "QtMaterial::QtMaterialSearchBar",
        "qtmaterial/widgets/inputs/qtmaterialsearchbar.h",
        "Qt Material 3 - Inputs", "materialSearchBar", "Material 3 search bar", false,
        "<property name=\"placeholderText\"><string>Search</string></property>"
        "<property name=\"clearButtonVisible\"><bool>true</bool></property>");

    addWidget<QtMaterialCheckbox>(
        widgets_, this, "QtMaterial::QtMaterialCheckbox",
        "qtmaterial/widgets/selection/qtmaterialcheckbox.h",
        "Qt Material 3 - Selection", "materialCheckbox", "Material 3 checkbox", false,
        "<property name=\"text\"><string>Checkbox</string></property>");
    addWidget<QtMaterialRadioButton>(
        widgets_, this, "QtMaterial::QtMaterialRadioButton",
        "qtmaterial/widgets/selection/qtmaterialradiobutton.h",
        "Qt Material 3 - Selection", "materialRadioButton", "Material 3 radio button", false,
        "<property name=\"text\"><string>Radio button</string></property>");
    addWidget<QtMaterialSwitch>(
        widgets_, this, "QtMaterial::QtMaterialSwitch",
        "qtmaterial/widgets/selection/qtmaterialswitch.h",
        "Qt Material 3 - Selection", "materialSwitch", "Material 3 switch", false,
        "<property name=\"text\"><string>Switch</string></property>");
    addWidget<QtMaterialChip>(
        widgets_, this, "QtMaterial::QtMaterialChip",
        "qtmaterial/widgets/selection/qtmaterialchip.h",
        "Qt Material 3 - Selection", "materialChip", "Material 3 chip", false,
        "<property name=\"text\"><string>Chip</string></property>");

    addWidget<QtMaterialBreadcrumb>(
        widgets_, this, "QtMaterial::QtMaterialBreadcrumb",
        "qtmaterial/widgets/navigation/qtmaterialbreadcrumb.h",
        "Qt Material 3 - Navigation", "materialBreadcrumb", "Material 3 breadcrumb");
    addWidget<QtMaterialTabs>(
        widgets_, this, "QtMaterial::QtMaterialTabs",
        "qtmaterial/widgets/navigation/qtmaterialtabs.h",
        "Qt Material 3 - Navigation", "materialTabs", "Material 3 tabs", true);

    addWidget<QtMaterialCard>(
        widgets_, this, "QtMaterial::QtMaterialCard",
        "qtmaterial/widgets/surfaces/qtmaterialcard.h",
        "Qt Material 3 - Surfaces", "materialCard", "Material 3 card", false,
        "<property name=\"titleText\"><string>Card title</string></property>"
        "<property name=\"bodyText\"><string>Supporting content</string></property>");
    addWidget<QtMaterialTopAppBar>(
        widgets_, this, "QtMaterialTopAppBar",
        "qtmaterial/widgets/surfaces/qtmaterialtopappbar.h",
        "Qt Material 3 - Surfaces", "materialTopAppBar", "Material 3 top app bar", false,
        "<property name=\"title\"><string>Page title</string></property>");
    addWidget<QtMaterialBottomAppBar>(
        widgets_, this, "QtMaterialBottomAppBar",
        "qtmaterial/widgets/surfaces/qtmaterialbottomappbar.h",
        "Qt Material 3 - Surfaces", "materialBottomAppBar", "Material 3 bottom app bar", false,
        "<property name=\"title\"><string>Actions</string></property>");
    addWidget<QtMaterialDivider>(
        widgets_, this, "QtMaterial::QtMaterialDivider",
        "qtmaterial/widgets/data/qtmaterialdivider.h",
        "Qt Material 3 - Surfaces", "materialDivider", "Material 3 divider", false,
        "<property name=\"thickness\"><number>1</number></property>"
        "<property name=\"decorative\"><bool>true</bool></property>");

    addWidget<QtMaterialLinearProgressIndicator>(
        widgets_, this, "QtMaterial::QtMaterialLinearProgressIndicator",
        "qtmaterial/widgets/progress/qtmateriallinearprogressindicator.h",
        "Qt Material 3 - Progress", "materialLinearProgress", "Material 3 linear progress indicator", false,
        "<property name=\"value\"><double>0.65</double></property>");
    addWidget<QtMaterialCircularProgressIndicator>(
        widgets_, this, "QtMaterial::QtMaterialCircularProgressIndicator",
        "qtmaterial/widgets/progress/qtmaterialcircularprogressindicator.h",
        "Qt Material 3 - Progress", "materialCircularProgress", "Material 3 circular progress indicator", false,
        "<property name=\"value\"><double>0.65</double></property>");

    addWidget<QtMaterialPagination>(
        widgets_, this, "QtMaterial::QtMaterialPagination",
        "qtmaterial/widgets/data/qtmaterialpagination.h",
        "Qt Material 3 - Data", "materialPagination", "Material 3 pagination", false,
        "<property name=\"page\"><number>1</number></property>"
        "<property name=\"pageSize\"><number>10</number></property>"
        "<property name=\"totalCount\"><number>100</number></property>");
    addWidget<QtMaterialTable>(
        widgets_, this, "QtMaterial::QtMaterialTable",
        "qtmaterial/widgets/data/qtmaterialtable.h",
        "Qt Material 3 - Data", "materialTable", "Material 3 table");
    addWidget<QtMaterialTreeView>(
        widgets_, this, "QtMaterial::QtMaterialTreeView",
        "qtmaterial/widgets/data/qtmaterialtreeview.h",
        "Qt Material 3 - Data", "materialTreeView", "Material 3 tree view");
}

QList<QDesignerCustomWidgetInterface*> QtMaterial3DesignerCollection::customWidgets() const
{
    return widgets_;
}

#include "qtmaterial3designercollection.moc"
