#include "selectionpage.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "qtmaterial/widgets/selection/qtmaterialcheckbox.h"
#include "qtmaterial/widgets/selection/qtmaterialchip.h"
#include "qtmaterial/widgets/selection/qtmaterialradiobutton.h"
#include "qtmaterial/widgets/selection/qtmaterialsegmentedbutton.h"
#include "qtmaterial/widgets/selection/qtmaterialswitch.h"

SelectionPage::SelectionPage(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);

    auto* row = new QHBoxLayout;

    auto* checkbox = new QtMaterial::QtMaterialCheckbox(this);
    checkbox->setText(QStringLiteral("Checkbox"));

    auto* radio = new QtMaterial::QtMaterialRadioButton(this);
    radio->setText(QStringLiteral("Radio"));

    auto* sw = new QtMaterial::QtMaterialSwitch(this);

    row->addWidget(checkbox);
    row->addWidget(radio);
    row->addWidget(sw);
    row->addStretch(1);

    auto* assistChip = new QtMaterial::QtMaterialChip(QStringLiteral("Assist chip"), this);
    assistChip->setVariant(QtMaterial::ChipVariant::Assist);

    auto* inputChip = new QtMaterial::QtMaterialChip(QStringLiteral("Input chip"), this);
    inputChip->setVariant(QtMaterial::ChipVariant::Input);
    inputChip->setRemovable(true);

    auto* segmented = new QtMaterial::QtMaterialSegmentedButton(this);
    segmented->addSegment(QStringLiteral("Day"));
    segmented->addSegment(QStringLiteral("Week"));
    segmented->addSegment(QStringLiteral("Month"));
    segmented->setCurrentIndex(1);

    layout->addLayout(row);
    layout->addWidget(assistChip);
    layout->addWidget(inputChip);
    layout->addWidget(segmented);
    layout->addStretch(1);
}
