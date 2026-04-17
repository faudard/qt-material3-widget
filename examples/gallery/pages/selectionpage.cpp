#include "selectionpage.h"
#include <QHBoxLayout>
#include "qtmaterial/widgets/selection/qtmaterialcheckbox.h"
#include "qtmaterial/widgets/selection/qtmaterialradiobutton.h"
#include "qtmaterial/widgets/selection/qtmaterialswitch.h"
SelectionPage::SelectionPage(QWidget* parent) : QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);
    auto* checkbox = new QtMaterial::QtMaterialCheckbox(this); checkbox->setText(QStringLiteral("Checkbox"));
    auto* radio = new QtMaterial::QtMaterialRadioButton(this); radio->setText(QStringLiteral("Radio"));
    auto* sw = new QtMaterial::QtMaterialSwitch(this);
    layout->addWidget(checkbox);
    layout->addWidget(radio);
    layout->addWidget(sw);
    layout->addStretch(1);
}
