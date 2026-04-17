#include "inputspage.h"
#include <QVBoxLayout>
#include "qtmaterial/widgets/inputs/qtmaterialautocompletepopup.h"
#include "qtmaterial/widgets/inputs/qtmaterialdatefield.h"
#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

InputsPage::InputsPage(QWidget* parent) : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);

    auto* outlined = new QtMaterial::QtMaterialOutlinedTextField(this);
    outlined->setLabelText(QStringLiteral("Email"));
    outlined->setSupportingText(QStringLiteral("We will never share your email."));
    outlined->setText(QStringLiteral("user@example.com"));

    auto* errorField = new QtMaterial::QtMaterialOutlinedTextField(this);
    errorField->setLabelText(QStringLiteral("Password"));
    errorField->setSupportingText(QStringLiteral("At least 8 characters."));
    errorField->setHasErrorState(true);
    errorField->setErrorText(QStringLiteral("Password is too short."));
    errorField->setText(QStringLiteral("123"));

    auto* filled = new QtMaterial::QtMaterialFilledTextField(this);
    filled->setLabelText(QStringLiteral("Filled text field"));
    filled->setSupportingText(QStringLiteral("Alternative container style."));

    auto* date = new QtMaterial::QtMaterialDateField(this);
    date->setLabelText(QStringLiteral("Date"));
    date->setSupportingText(QStringLiteral("MM / DD / YYYY"));

    auto* popup = new QtMaterial::QtMaterialAutocompletePopup(this);
    popup->setSuggestions({QStringLiteral("Alpha"), QStringLiteral("Beta"), QStringLiteral("Gamma")});

    layout->addWidget(outlined);
    layout->addWidget(errorField);
    layout->addWidget(filled);
    layout->addWidget(date);
    layout->addWidget(popup);
    layout->addStretch(1);
}
