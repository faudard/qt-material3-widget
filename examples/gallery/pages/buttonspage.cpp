#include "buttonspage.h"

#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSizePolicy>
#include <QStyle>
#include <QVBoxLayout>

#include "qtmaterial/widgets/buttons/qtmaterialelevatedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialextendedfab.h"
#include "qtmaterial/widgets/buttons/qtmaterialfab.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialiconbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"

namespace {

QLabel* sectionTitle(const QString& text, QWidget* parent)
{
    auto* label = new QLabel(text, parent);
    QFont titleFont = label->font();
    titleFont.setBold(true);
    titleFont.setPointSize(titleFont.pointSize() + 1);
    label->setFont(titleFont);
    return label;
}

QLabel* helperText(const QString& text, QWidget* parent)
{
    auto* label = new QLabel(text, parent);
    label->setWordWrap(true);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    return label;
}

QFrame* separator(QWidget* parent)
{
    auto* line = new QFrame(parent);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    return line;
}

QIcon standardIcon(QWidget* widget, QStyle::StandardPixmap pixmap)
{
    return widget->style()->standardIcon(pixmap, nullptr, widget);
}

void addLabeledWidget(QGridLayout* layout, int row, int column, const QString& labelText, QWidget* widget, QWidget* parent)
{
    auto* cell = new QWidget(parent);
    auto* cellLayout = new QVBoxLayout(cell);
    cellLayout->setContentsMargins(0, 0, 0, 0);
    cellLayout->setSpacing(8);

    auto* label = new QLabel(labelText, cell);
    label->setAlignment(Qt::AlignHCenter);
    label->setWordWrap(true);

    cellLayout->addWidget(label);
    cellLayout->addWidget(widget, 0, Qt::AlignHCenter);
    layout->addWidget(cell, row, column);
}

} // namespace

ButtonsPage::ButtonsPage(QWidget* parent)
    : QWidget(parent)
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(24, 24, 24, 24);
    root->setSpacing(20);

    root->addWidget(sectionTitle(QStringLiteral("Buttons"), this));
    root->addWidget(helperText(
        QStringLiteral("Standard Material button variants plus FAB / Extended FAB examples. "
                       "Use this page to visually check theme, density, disabled state and sizing behavior."),
        this));

    auto* buttonGrid = new QGridLayout();
    buttonGrid->setHorizontalSpacing(16);
    buttonGrid->setVerticalSpacing(16);

    auto* text = new QtMaterial::QtMaterialTextButton(this);
    text->setText(QStringLiteral("Text"));
    text->setDensity(QtMaterial::Density::Compact);

    auto* filled = new QtMaterial::QtMaterialFilledButton(this);
    filled->setText(QStringLiteral("Filled"));
    filled->setDensity(QtMaterial::Density::Compact);


    auto* tonal = new QtMaterial::QtMaterialFilledTonalButton(this);
    tonal->setText(QStringLiteral("Tonal"));
    tonal->setDensity(QtMaterial::Density::Compact);

    auto* outlined = new QtMaterial::QtMaterialOutlinedButton(this);
    outlined->setText(QStringLiteral("Outlined"));
    outlined->setDensity(QtMaterial::Density::Compact);

    auto* elevated = new QtMaterial::QtMaterialElevatedButton(this);
    elevated->setText(QStringLiteral("Elevated"));
    elevated->setDensity(QtMaterial::Density::Compact);

    auto* icon = new QtMaterial::QtMaterialIconButton(this);
    icon->setIcon(standardIcon(this, QStyle::SP_DialogOpenButton));
    icon->setAccessibleName(QStringLiteral("Open"));
    icon->setAccessibleDescription(QStringLiteral("Opens a file using an icon-only Material button."));
    icon->setToolTip(QStringLiteral("Open"));

    addLabeledWidget(buttonGrid, 0, 0, QStringLiteral("Text"), text, this);
    addLabeledWidget(buttonGrid, 0, 1, QStringLiteral("Filled"), filled, this);
    addLabeledWidget(buttonGrid, 0, 2, QStringLiteral("Tonal"), tonal, this);
    addLabeledWidget(buttonGrid, 1, 0, QStringLiteral("Outlined"), outlined, this);
    addLabeledWidget(buttonGrid, 1, 1, QStringLiteral("Elevated"), elevated, this);
    addLabeledWidget(buttonGrid, 1, 2, QStringLiteral("Icon"), icon, this);
    root->addLayout(buttonGrid);

    root->addWidget(separator(this));
    root->addWidget(sectionTitle(QStringLiteral("Floating action buttons"), this));
    root->addWidget(helperText(
        QStringLiteral("FAB is icon-only and intended for one promoted action. Extended FAB can be text-only "
                       "or icon + text, and should expand horizontally according to its label. "
                       "Hover, keyboard-focus and press the examples to check motion and interaction elevation."),
        this));

    auto* fabGrid = new QGridLayout();
    fabGrid->setHorizontalSpacing(20);
    fabGrid->setVerticalSpacing(20);

    const QIcon composeIcon = standardIcon(this, QStyle::SP_FileDialogNewFolder);
    const QIcon addIcon = standardIcon(this, QStyle::SP_DialogApplyButton);

    auto* fab = new QtMaterial::QtMaterialFab(composeIcon, this);
    fab->setAccessibleName(QStringLiteral("Create"));
    fab->setAccessibleDescription(QStringLiteral("Creates a new item. This icon-only FAB must expose its action through accessibility metadata."));
    fab->setToolTip(QStringLiteral("Create"));

    auto* disabledFab = new QtMaterial::QtMaterialFab(addIcon, this);
    disabledFab->setAccessibleName(QStringLiteral("Create unavailable"));
    disabledFab->setAccessibleDescription(QStringLiteral("Disabled icon-only FAB example."));
    disabledFab->setToolTip(QStringLiteral("Create unavailable"));
    disabledFab->setEnabled(false);

    auto* extendedText = new QtMaterial::QtMaterialExtendedFab(QStringLiteral("Compose"), this);
    extendedText->setAccessibleName(QStringLiteral("Compose"));
    extendedText->setAccessibleDescription(QStringLiteral("Text-only extended FAB example."));
    extendedText->setToolTip(QStringLiteral("Compose"));

    auto* extendedIconText = new QtMaterial::QtMaterialExtendedFab(composeIcon, QStringLiteral("Compose"), this);
    extendedIconText->setAccessibleName(QStringLiteral("Compose"));
    extendedIconText->setAccessibleDescription(QStringLiteral("Extended FAB example with leading icon and visible label."));
    extendedIconText->setToolTip(QStringLiteral("Compose"));

    auto* extendedLong = new QtMaterial::QtMaterialExtendedFab(addIcon, QStringLiteral("Create document"), this);
    extendedLong->setAccessibleName(QStringLiteral("Create document"));
    extendedLong->setAccessibleDescription(QStringLiteral("Extended FAB example with a longer visible label."));
    extendedLong->setToolTip(QStringLiteral("Create document"));

    auto* disabledExtended = new QtMaterial::QtMaterialExtendedFab(composeIcon, QStringLiteral("Disabled"), this);
    disabledExtended->setAccessibleName(QStringLiteral("Compose unavailable"));
    disabledExtended->setAccessibleDescription(QStringLiteral("Disabled extended FAB example."));
    disabledExtended->setToolTip(QStringLiteral("Compose unavailable"));
    disabledExtended->setEnabled(false);

    auto* secondaryFab = new QtMaterial::QtMaterialFab(composeIcon, this);
    secondaryFab->setFabVariant(QtMaterial::QtMaterialFabVariant::Secondary);
    secondaryFab->setAccessibleName(QStringLiteral("Create secondary"));
    secondaryFab->setToolTip(QStringLiteral("Create secondary"));

    auto* tertiaryFab = new QtMaterial::QtMaterialFab(composeIcon, this);
    tertiaryFab->setFabVariant(QtMaterial::QtMaterialFabVariant::Tertiary);
    tertiaryFab->setAccessibleName(QStringLiteral("Create tertiary"));
    tertiaryFab->setToolTip(QStringLiteral("Create tertiary"));

    auto* surfaceFab = new QtMaterial::QtMaterialFab(composeIcon, this);
    surfaceFab->setFabVariant(QtMaterial::QtMaterialFabVariant::Surface);
    surfaceFab->setAccessibleName(QStringLiteral("Create surface"));
    surfaceFab->setToolTip(QStringLiteral("Create surface"));

    auto* secondaryExtended = new QtMaterial::QtMaterialExtendedFab(composeIcon, QStringLiteral("Secondary"), this);
    secondaryExtended->setFabVariant(QtMaterial::QtMaterialFabVariant::Secondary);
    secondaryExtended->setAccessibleName(QStringLiteral("Secondary action"));
    secondaryExtended->setToolTip(QStringLiteral("Secondary action"));

    auto* tertiaryExtended = new QtMaterial::QtMaterialExtendedFab(composeIcon, QStringLiteral("Tertiary"), this);
    tertiaryExtended->setFabVariant(QtMaterial::QtMaterialFabVariant::Tertiary);
    tertiaryExtended->setAccessibleName(QStringLiteral("Tertiary action"));
    tertiaryExtended->setToolTip(QStringLiteral("Tertiary action"));

    auto* surfaceExtended = new QtMaterial::QtMaterialExtendedFab(composeIcon, QStringLiteral("Surface"), this);
    surfaceExtended->setFabVariant(QtMaterial::QtMaterialFabVariant::Surface);
    surfaceExtended->setAccessibleName(QStringLiteral("Surface action"));
    surfaceExtended->setToolTip(QStringLiteral("Surface action"));

    addLabeledWidget(fabGrid, 0, 0, QStringLiteral("FAB"), fab, this);
    addLabeledWidget(fabGrid, 0, 1, QStringLiteral("Disabled FAB"), disabledFab, this);
    addLabeledWidget(fabGrid, 0, 2, QStringLiteral("Extended text"), extendedText, this);
    addLabeledWidget(fabGrid, 1, 0, QStringLiteral("Extended icon + text"), extendedIconText, this);
    addLabeledWidget(fabGrid, 1, 1, QStringLiteral("Extended long label"), extendedLong, this);
    addLabeledWidget(fabGrid, 1, 2, QStringLiteral("Disabled Extended"), disabledExtended, this);
    addLabeledWidget(fabGrid, 2, 0, QStringLiteral("Secondary FAB"), secondaryFab, this);
    addLabeledWidget(fabGrid, 2, 1, QStringLiteral("Tertiary FAB"), tertiaryFab, this);
    addLabeledWidget(fabGrid, 2, 2, QStringLiteral("Surface FAB"), surfaceFab, this);
    addLabeledWidget(fabGrid, 3, 0, QStringLiteral("Secondary Extended"), secondaryExtended, this);
    addLabeledWidget(fabGrid, 3, 1, QStringLiteral("Tertiary Extended"), tertiaryExtended, this);
    addLabeledWidget(fabGrid, 3, 2, QStringLiteral("Surface Extended"), surfaceExtended, this);
    root->addLayout(fabGrid);

 // BEGIN GENERATED BUTTONS MATURITY GALLERY COVERAGE
 root->addWidget(separator(this));

 root->addWidget(sectionTitle(QStringLiteral("Buttons maturity coverage"), this));
 root->addWidget(helperText(
     QStringLiteral("This section is intentionally exhaustive: disabled standard buttons, icon + text, "
                    "long labels, RTL layout and explicit accessibility names for icon-only actions. "
                    "It exists to make the gallery a concrete proof for the Buttons maturity matrix."),
     this));

 auto* disabledGrid = new QGridLayout();
 disabledGrid->setHorizontalSpacing(16);
 disabledGrid->setVerticalSpacing(16);

 auto* disabledText = new QtMaterial::QtMaterialTextButton(this);
 disabledText->setText(QStringLiteral("Text"));
 disabledText->setEnabled(false);

 auto* disabledFilled = new QtMaterial::QtMaterialFilledButton(this);
 disabledFilled->setText(QStringLiteral("Filled"));
 disabledFilled->setEnabled(false);

 auto* disabledTonal = new QtMaterial::QtMaterialFilledTonalButton(this);
 disabledTonal->setText(QStringLiteral("Tonal"));
 disabledTonal->setEnabled(false);

 auto* disabledOutlined = new QtMaterial::QtMaterialOutlinedButton(this);
 disabledOutlined->setText(QStringLiteral("Outlined"));
 disabledOutlined->setEnabled(false);

 auto* disabledElevated = new QtMaterial::QtMaterialElevatedButton(this);
 disabledElevated->setText(QStringLiteral("Elevated"));
 disabledElevated->setEnabled(false);

 auto* disabledIcon = new QtMaterial::QtMaterialIconButton(this);
 disabledIcon->setIcon(standardIcon(this, QStyle::SP_DialogOpenButton));
 disabledIcon->setAccessibleName(QStringLiteral("Open unavailable"));
 disabledIcon->setAccessibleDescription(QStringLiteral("Disabled icon-only button with explicit accessible name."));
 disabledIcon->setToolTip(QStringLiteral("Open unavailable"));
 disabledIcon->setEnabled(false);

 addLabeledWidget(disabledGrid, 0, 0, QStringLiteral("Disabled text"), disabledText, this);
 addLabeledWidget(disabledGrid, 0, 1, QStringLiteral("Disabled filled"), disabledFilled, this);
 addLabeledWidget(disabledGrid, 0, 2, QStringLiteral("Disabled tonal"), disabledTonal, this);
 addLabeledWidget(disabledGrid, 1, 0, QStringLiteral("Disabled outlined"), disabledOutlined, this);
 addLabeledWidget(disabledGrid, 1, 1, QStringLiteral("Disabled elevated"), disabledElevated, this);
 addLabeledWidget(disabledGrid, 1, 2, QStringLiteral("Disabled icon"), disabledIcon, this);
 root->addLayout(disabledGrid);

 root->addWidget(helperText(
     QStringLiteral("Icon-only buttons and compact FABs must expose an explicit accessible name. "
                    "Visible labels normally provide the accessible name for text-bearing buttons."),
     this));

 auto* iconTextGrid = new QGridLayout();
 iconTextGrid->setHorizontalSpacing(16);
 iconTextGrid->setVerticalSpacing(16);

 const QIcon saveIcon = standardIcon(this, QStyle::SP_DialogSaveButton);
 const QIcon editIcon = standardIcon(this, QStyle::SP_FileDialogDetailedView);

 auto* textLong = new QtMaterial::QtMaterialTextButton(this);
 textLong->setText(QStringLiteral("Review very long action label"));

 auto* filledIconText = new QtMaterial::QtMaterialFilledButton(this);
 filledIconText->setText(QStringLiteral("Save"));
 filledIconText->setIcon(saveIcon);

 auto* tonalIconText = new QtMaterial::QtMaterialFilledTonalButton(this);
 tonalIconText->setText(QStringLiteral("Edit"));
 tonalIconText->setIcon(editIcon);

 auto* outlinedIconText = new QtMaterial::QtMaterialOutlinedButton(this);
 outlinedIconText->setText(QStringLiteral("Open"));
 outlinedIconText->setIcon(standardIcon(this, QStyle::SP_DialogOpenButton));

 auto* elevatedIconText = new QtMaterial::QtMaterialElevatedButton(this);
 elevatedIconText->setText(QStringLiteral("Upload"));
 elevatedIconText->setIcon(standardIcon(this, QStyle::SP_ArrowUp));

 auto* filledLong = new QtMaterial::QtMaterialFilledButton(this);
 filledLong->setText(QStringLiteral("Create project from selected template"));
 filledLong->setIcon(standardIcon(this, QStyle::SP_FileDialogNewFolder));

 addLabeledWidget(iconTextGrid, 0, 0, QStringLiteral("Text long label"), textLong, this);
 addLabeledWidget(iconTextGrid, 0, 1, QStringLiteral("Filled icon + text"), filledIconText, this);
 addLabeledWidget(iconTextGrid, 0, 2, QStringLiteral("Tonal icon + text"), tonalIconText, this);
 addLabeledWidget(iconTextGrid, 1, 0, QStringLiteral("Outlined icon + text"), outlinedIconText, this);
 addLabeledWidget(iconTextGrid, 1, 1, QStringLiteral("Elevated icon + text"), elevatedIconText, this);
 addLabeledWidget(iconTextGrid, 1, 2, QStringLiteral("Filled long label"), filledLong, this);
 root->addLayout(iconTextGrid);

 root->addWidget(separator(this));
 root->addWidget(sectionTitle(QStringLiteral("RTL preview"), this));
 root->addWidget(helperText(
     QStringLiteral("The container below uses Qt::RightToLeft. Text-bearing buttons should keep stable "
                    "size hints while mirroring start/end padding and icon/text order where applicable."),
     this));

 auto* rtlContainer = new QWidget(this);
 rtlContainer->setLayoutDirection(Qt::RightToLeft);
 auto* rtlLayout = new QGridLayout(rtlContainer);
 rtlLayout->setHorizontalSpacing(16);
 rtlLayout->setVerticalSpacing(16);

 auto* rtlFilled = new QtMaterial::QtMaterialFilledButton(rtlContainer);
 rtlFilled->setText(QStringLiteral("حفظ"));
 rtlFilled->setIcon(saveIcon);
 rtlFilled->setAccessibleName(QStringLiteral("Save RTL"));

 auto* rtlOutlined = new QtMaterial::QtMaterialOutlinedButton(rtlContainer);
 rtlOutlined->setText(QStringLiteral("فتح"));
 rtlOutlined->setIcon(standardIcon(this, QStyle::SP_DialogOpenButton));
 rtlOutlined->setAccessibleName(QStringLiteral("Open RTL"));

 auto* rtlExtended = new QtMaterial::QtMaterialExtendedFab(composeIcon, QStringLiteral("إنشاء"), rtlContainer);
 rtlExtended->setAccessibleName(QStringLiteral("Create RTL"));
 rtlExtended->setToolTip(QStringLiteral("Create RTL"));

 auto* rtlLong = new QtMaterial::QtMaterialFilledTonalButton(rtlContainer);
 rtlLong->setText(QStringLiteral("مراجعة الإجراء الطويل"));
 rtlLong->setIcon(editIcon);
 rtlLong->setAccessibleName(QStringLiteral("Review long RTL action"));

 addLabeledWidget(rtlLayout, 0, 0, QStringLiteral("Filled RTL"), rtlFilled, rtlContainer);
 addLabeledWidget(rtlLayout, 0, 1, QStringLiteral("Outlined RTL"), rtlOutlined, rtlContainer);
 addLabeledWidget(rtlLayout, 1, 0, QStringLiteral("Extended FAB RTL"), rtlExtended, rtlContainer);
 addLabeledWidget(rtlLayout, 1, 1, QStringLiteral("Long label RTL"), rtlLong, rtlContainer);
 root->addWidget(rtlContainer);
 // END GENERATED BUTTONS MATURITY GALLERY COVERAGE

 root->addStretch(1);
}
