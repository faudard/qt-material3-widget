#include "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
namespace QtMaterial {
QtMaterialFilledTonalButton::QtMaterialFilledTonalButton(QWidget* parent) : QtMaterialFilledButton(parent) {}
QtMaterialFilledTonalButton::~QtMaterialFilledTonalButton() = default;
ButtonSpec QtMaterialFilledTonalButton::resolveButtonSpec() const
{
    SpecFactory factory;
    return factory.filledTonalButtonSpec(theme(), density());
}
} // namespace QtMaterial
