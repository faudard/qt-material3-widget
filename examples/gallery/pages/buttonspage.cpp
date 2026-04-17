#include "buttonspage.h"
#include <QHBoxLayout>
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialiconbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"
ButtonsPage::ButtonsPage(QWidget* parent) : QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);
    auto* text = new QtMaterial::QtMaterialTextButton(this);
    text->setText(QStringLiteral("Text button"));
    auto* filled = new QtMaterial::QtMaterialFilledButton(this);
    filled->setText(QStringLiteral("Filled button"));
    auto* icon = new QtMaterial::QtMaterialIconButton(this);
    layout->addWidget(text);
    layout->addWidget(filled);
    layout->addWidget(icon);
    layout->addStretch(1);
}
