#include "buttonspage.h"
#include <QGridLayout>
#include "qtmaterial/widgets/buttons/qtmaterialelevatedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialextendedfab.h"
#include "qtmaterial/widgets/buttons/qtmaterialfab.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialiconbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"
ButtonsPage::ButtonsPage(QWidget* parent) : QWidget(parent)
{
    auto* layout = new QGridLayout(this);
    auto* text = new QtMaterial::QtMaterialTextButton(this); text->setText(QStringLiteral("Text"));
    auto* filled = new QtMaterial::QtMaterialFilledButton(this); filled->setText(QStringLiteral("Filled"));
    auto* tonal = new QtMaterial::QtMaterialFilledTonalButton(this); tonal->setText(QStringLiteral("Tonal"));
    auto* outlined = new QtMaterial::QtMaterialOutlinedButton(this); outlined->setText(QStringLiteral("Outlined"));
    auto* elevated = new QtMaterial::QtMaterialElevatedButton(this); elevated->setText(QStringLiteral("Elevated"));
    auto* icon = new QtMaterial::QtMaterialIconButton(this);
    auto* fab = new QtMaterial::QtMaterialFab(this);
    auto* extFab = new QtMaterial::QtMaterialExtendedFab(this); extFab->setText(QStringLiteral("Extended FAB"));
    layout->addWidget(text, 0, 0);
    layout->addWidget(filled, 0, 1);
    layout->addWidget(tonal, 0, 2);
    layout->addWidget(outlined, 1, 0);
    layout->addWidget(elevated, 1, 1);
    layout->addWidget(icon, 1, 2);
    layout->addWidget(fab, 2, 0);
    layout->addWidget(extFab, 2, 1, 1, 2);
    layout->setRowStretch(3, 1);
}
