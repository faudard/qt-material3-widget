#include "inputspage.h"
#include <QVBoxLayout>
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"
InputsPage::InputsPage(QWidget* parent) : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    auto* field = new QtMaterial::QtMaterialOutlinedTextField(this);
    field->setLabelText(QStringLiteral("Outlined text field"));
    field->setSupportingText(QStringLiteral("Supporting text"));
    layout->addWidget(field);
    layout->addStretch(1);
}
