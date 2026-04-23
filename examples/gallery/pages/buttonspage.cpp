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

    auto* filled = new QtMaterial::QtMaterialFilledButton(this);
    filled->setText(QStringLiteral("Filled"));

    auto* tonal = new QtMaterial::QtMaterialFilledTonalButton(this);
    tonal->setText(QStringLiteral("Tonal"));

    auto* outlined = new QtMaterial::QtMaterialOutlinedButton(this);
    outlined->setText(QStringLiteral("Outlined"));

    auto* elevated = new QtMaterial::QtMaterialElevatedButton(this);
    elevated->setText(QStringLiteral("Elevated"));

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

    root->addStretch(1);
}
