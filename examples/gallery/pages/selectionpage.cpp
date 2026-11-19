#include "selectionpage.h"

#include <QHBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
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

    auto* filterChip = new QtMaterial::QtMaterialChip(QStringLiteral("Filter chip"), this);
    filterChip->setVariant(QtMaterial::ChipVariant::Filter);
    filterChip->setChecked(true);

    auto* suggestionChip = new QtMaterial::QtMaterialChip(QStringLiteral("Suggestion chip"), this);
    suggestionChip->setVariant(QtMaterial::ChipVariant::Suggestion);

    auto* inputChip = new QtMaterial::QtMaterialChip(QStringLiteral("Input chip"), this);
    inputChip->setVariant(QtMaterial::ChipVariant::Input);
    inputChip->setRemovable(true);
    inputChip->setToolTip(QStringLiteral("Remove input chip"));
    connect(
        inputChip,
        &QtMaterial::QtMaterialChip::removeRequested,
        inputChip,
        &QWidget::hide);

    auto* segmented = new QtMaterial::QtMaterialSegmentedButton(this);
    segmented->addSegment(QStringLiteral("Day"));
    segmented->addSegment(QStringLiteral("Week"));
    segmented->addSegment(QStringLiteral("Month"));
    segmented->setCurrentIndex(1);

    layout->addLayout(row);
    layout->addWidget(assistChip);
    layout->addWidget(filterChip);
    layout->addWidget(inputChip);
    layout->addWidget(suggestionChip);
    layout->addWidget(segmented);
    auto* expressiveChips = new QCheckBox(tr("Expressive chips"), this);
    auto* disableFilter = new QCheckBox(tr("Disable filter chip"), this);
    auto* restoreInput = new QPushButton(tr("Restore input chip"), this);
    auto* chipStatus = new QLabel(tr("No chip action yet"), this);
    chipStatus->setAccessibleName(tr("Last chip action"));
    layout->addWidget(expressiveChips);
    layout->addWidget(disableFilter);
    layout->addWidget(restoreInput);
    layout->addWidget(chipStatus);
    connect(expressiveChips, &QCheckBox::toggled, this,
            [assistChip, filterChip, suggestionChip, inputChip](bool expressive) {
                for (auto* chip : {assistChip, filterChip, suggestionChip, inputChip}) chip->setExpressive(expressive);
            });
    connect(disableFilter, &QCheckBox::toggled, filterChip, &QWidget::setDisabled);
    connect(restoreInput, &QPushButton::clicked, inputChip, &QWidget::show);
    connect(filterChip, &QAbstractButton::clicked, chipStatus, [chipStatus](bool checked) {
        chipStatus->setText(checked ? QObject::tr("Filter checked") : QObject::tr("Filter unchecked"));
    });
    connect(inputChip, &QtMaterial::QtMaterialChip::removeRequested, chipStatus, [chipStatus, restoreInput] {
        chipStatus->setText(QObject::tr("Input chip removed"));
        restoreInput->setFocus(Qt::OtherFocusReason);
    });
    layout->addStretch(1);
}
