#include "qtmaterial3designercollection.h"

#include <functional>
#include <utility>

#include <QIcon>
#include <QString>
#include <QWidget>
#include <QtUiPlugin/QDesignerCustomWidgetInterface>

#include "qtmaterial/widgets/buttons/qtmaterialelevatedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"
#include "qtmaterial/widgets/data/qtmaterialtable.h"
#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"
#include "qtmaterial/widgets/navigation/qtmaterialtabs.h"
#include "qtmaterial/widgets/selection/qtmaterialcheckbox.h"
#include "qtmaterial/widgets/selection/qtmaterialradiobutton.h"
#include "qtmaterial/widgets/selection/qtmaterialswitch.h"
#include "qtmaterial/widgets/surfaces/qtmaterialcard.h"

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
                    QObject* parent)
        : QObject(parent)
        , className_(std::move(className))
        , includeFile_(std::move(includeFile))
        , group_(std::move(group))
        , objectName_(std::move(objectName))
        , toolTip_(std::move(toolTip))
        , container_(container)
        , factory_(std::move(factory))
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
        return QStringLiteral(
            "<ui language=\"c++\">"
            "<widget class=\"%1\" name=\"%2\">"
            "<property name=\"geometry\">"
            "<rect><x>0</x><y>0</y><width>160</width><height>48</height></rect>"
            "</property>"
            "</widget>"
            "</ui>")
            .arg(className_, objectName_);
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
};

template <typename T>
void addWidget(QList<QDesignerCustomWidgetInterface*>& widgets,
               QObject* owner,
               const char* className,
               const char* includeFile,
               const char* group,
               const char* objectName,
               const char* toolTip,
               bool container = false)
{
    widgets.append(new WidgetInterface(
        QString::fromLatin1(className),
        QString::fromLatin1(includeFile),
        QString::fromLatin1(group),
        QString::fromLatin1(objectName),
        QString::fromLatin1(toolTip),
        container,
        [](QWidget* parent) { return new T(parent); },
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
        "Qt Material 3 - Buttons", "materialFilledButton", "Material 3 filled button");
    addWidget<QtMaterialOutlinedButton>(
        widgets_, this, "QtMaterial::QtMaterialOutlinedButton",
        "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h",
        "Qt Material 3 - Buttons", "materialOutlinedButton", "Material 3 outlined button");
    addWidget<QtMaterialElevatedButton>(
        widgets_, this, "QtMaterial::QtMaterialElevatedButton",
        "qtmaterial/widgets/buttons/qtmaterialelevatedbutton.h",
        "Qt Material 3 - Buttons", "materialElevatedButton", "Material 3 elevated button");
    addWidget<QtMaterialTextButton>(
        widgets_, this, "QtMaterial::QtMaterialTextButton",
        "qtmaterial/widgets/buttons/qtmaterialtextbutton.h",
        "Qt Material 3 - Buttons", "materialTextButton", "Material 3 text button");

    addWidget<QtMaterialOutlinedTextField>(
        widgets_, this, "QtMaterial::QtMaterialOutlinedTextField",
        "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h",
        "Qt Material 3 - Inputs", "materialOutlinedTextField", "Material 3 outlined text field");
    addWidget<QtMaterialFilledTextField>(
        widgets_, this, "QtMaterial::QtMaterialFilledTextField",
        "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h",
        "Qt Material 3 - Inputs", "materialFilledTextField", "Material 3 filled text field");

    addWidget<QtMaterialCheckbox>(
        widgets_, this, "QtMaterial::QtMaterialCheckbox",
        "qtmaterial/widgets/selection/qtmaterialcheckbox.h",
        "Qt Material 3 - Selection", "materialCheckbox", "Material 3 checkbox");
    addWidget<QtMaterialRadioButton>(
        widgets_, this, "QtMaterial::QtMaterialRadioButton",
        "qtmaterial/widgets/selection/qtmaterialradiobutton.h",
        "Qt Material 3 - Selection", "materialRadioButton", "Material 3 radio button");
    addWidget<QtMaterialSwitch>(
        widgets_, this, "QtMaterial::QtMaterialSwitch",
        "qtmaterial/widgets/selection/qtmaterialswitch.h",
        "Qt Material 3 - Selection", "materialSwitch", "Material 3 switch");

    addWidget<QtMaterialTabs>(
        widgets_, this, "QtMaterial::QtMaterialTabs",
        "qtmaterial/widgets/navigation/qtmaterialtabs.h",
        "Qt Material 3 - Navigation", "materialTabs", "Material 3 tabs", true);

    addWidget<QtMaterialCard>(
        widgets_, this, "QtMaterial::QtMaterialCard",
        "qtmaterial/widgets/surfaces/qtmaterialcard.h",
        "Qt Material 3 - Surfaces", "materialCard", "Material 3 card", true);

    addWidget<QtMaterialTable>(
        widgets_, this, "QtMaterial::QtMaterialTable",
        "qtmaterial/widgets/data/qtmaterialtable.h",
        "Qt Material 3 - Data", "materialTable", "Material 3 table");
}

QList<QDesignerCustomWidgetInterface*> QtMaterial3DesignerCollection::customWidgets() const
{
    return widgets_;
}

#include "qtmaterial3designercollection.moc"
