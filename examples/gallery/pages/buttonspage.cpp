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
    icon->setToolTip(QStringLiteral("Icon button"));

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
                       "or icon + text, and should expand horizontally according to its label."),
        this));

    auto* fabGrid = new QGridLayout();
    fabGrid->setHorizontalSpacing(20);
    fabGrid->setVerticalSpacing(20);

    const QIcon composeIcon = standardIcon(this, QStyle::SP_FileDialogNewFolder);
    const QIcon addIcon = standardIcon(this, QStyle::SP_DialogApplyButton);

    auto* fab = new QtMaterial::QtMaterialFab(composeIcon, this);
    fab->setAccessibleName(QStringLiteral("Create"));
    fab->setToolTip(QStringLiteral("FAB: icon-only promoted action"));

    auto* disabledFab = new QtMaterial::QtMaterialFab(addIcon, this);
    disabledFab->setAccessibleName(QStringLiteral("Disabled create"));
    disabledFab->setToolTip(QStringLiteral("Disabled FAB"));
    disabledFab->setEnabled(false);

    auto* extendedText = new QtMaterial::QtMaterialExtendedFab(QStringLiteral("Compose"), this);
    extendedText->setAccessibleName(QStringLiteral("Compose"));
    extendedText->setToolTip(QStringLiteral("Extended FAB: text-only"));

    auto* extendedIconText = new QtMaterial::QtMaterialExtendedFab(composeIcon, QStringLiteral("Compose"), this);
    extendedIconText->setAccessibleName(QStringLiteral("Compose"));
    extendedIconText->setToolTip(QStringLiteral("Extended FAB: icon and text"));

    auto* extendedLong = new QtMaterial::QtMaterialExtendedFab(addIcon, QStringLiteral("Create document"), this);
    extendedLong->setAccessibleName(QStringLiteral("Create document"));
    extendedLong->setToolTip(QStringLiteral("Extended FAB: longer label"));

    auto* disabledExtended = new QtMaterial::QtMaterialExtendedFab(composeIcon, QStringLiteral("Disabled"), this);
    disabledExtended->setAccessibleName(QStringLiteral("Disabled compose"));
    disabledExtended->setToolTip(QStringLiteral("Disabled Extended FAB"));
    disabledExtended->setEnabled(false);

    addLabeledWidget(fabGrid, 0, 0, QStringLiteral("FAB"), fab, this);
    addLabeledWidget(fabGrid, 0, 1, QStringLiteral("Disabled FAB"), disabledFab, this);
    addLabeledWidget(fabGrid, 0, 2, QStringLiteral("Extended text"), extendedText, this);
    addLabeledWidget(fabGrid, 1, 0, QStringLiteral("Extended icon + text"), extendedIconText, this);
    addLabeledWidget(fabGrid, 1, 1, QStringLiteral("Extended long label"), extendedLong, this);
    addLabeledWidget(fabGrid, 1, 2, QStringLiteral("Disabled Extended"), disabledExtended, this);
    root->addLayout(fabGrid);

    root->addStretch(1);
}
