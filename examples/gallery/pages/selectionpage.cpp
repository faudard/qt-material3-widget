#include "selectionpage.h"
#include <QVBoxLayout>
#include "qtmaterial/widgets/selection/qtmaterialcheckbox.h"
#include "qtmaterial/widgets/selection/qtmaterialswitch.h"
SelectionPage::SelectionPage(QWidget* parent) : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    auto* checkbox = new QtMaterial::QtMaterialCheckbox(this);
    checkbox->setText(QStringLiteral("Checkbox"));
    auto* sw = new QtMaterial::QtMaterialSwitch(this);
    layout->addWidget(checkbox);
    layout->addWidget(sw);
    layout->addStretch(1);
}
