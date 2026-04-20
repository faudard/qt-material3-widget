#include "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h"

#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {

QtMaterialFilledTonalButton::QtMaterialFilledTonalButton(QWidget* parent)
    : QtMaterialFilledButton(parent)
{
}

QtMaterialFilledTonalButton::QtMaterialFilledTonalButton(const QString& text, QWidget* parent)
    : QtMaterialFilledButton(parent)
{
    setText(text);
}

QtMaterialFilledTonalButton::~QtMaterialFilledTonalButton() = default;

ButtonSpec QtMaterialFilledTonalButton::resolveButtonSpec() const
{
    SpecFactory factory;
    return factory.filledTonalButtonSpec(theme(), density());
}

} // namespace QtMaterial
