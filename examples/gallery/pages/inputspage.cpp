#include "inputspage.h"

#include <QVBoxLayout>

#include "qtmaterial/widgets/inputs/qtmaterialautocompletepopup.h"
#include "qtmaterial/widgets/inputs/qtmaterialdatefield.h"
#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

InputsPage::InputsPage(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);

    auto* outlined = new QtMaterial::QtMaterialOutlinedTextField(this);
    outlined->setLabelText(QStringLiteral("Outlined text field"));

    auto* filled = new QtMaterial::QtMaterialFilledTextField(this);
    filled->setLabelText(QStringLiteral("Filled text field"));

    auto* date = new QtMaterialDateField(this);
    date->setLabelText(QStringLiteral("Date field"));

    auto* popup = new QtMaterialAutocompletePopup(this);
    popup->setSuggestions({
        QStringLiteral("Alpha"),
        QStringLiteral("Beta"),
        QStringLiteral("Gamma"),
        QStringLiteral("Delta")
    });

    layout->addWidget(outlined);
    layout->addWidget(filled);
    layout->addWidget(date);
    layout->addWidget(popup);
    layout->addStretch(1);
}
