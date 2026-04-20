#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"

#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {

QtMaterialFilledTextField::QtMaterialFilledTextField(QWidget* parent)
    : QtMaterialOutlinedTextField(parent)
{
}

QtMaterialFilledTextField::QtMaterialFilledTextField(const QString& labelText, QWidget* parent)
    : QtMaterialOutlinedTextField(parent)
{
    setLabelText(labelText);
}

QtMaterialFilledTextField::~QtMaterialFilledTextField() = default;

QtMaterialOutlinedTextField::ShellVariant QtMaterialFilledTextField::shellVariant() const
{
    return ShellVariant::Filled;
}

TextFieldSpec QtMaterialFilledTextField::resolveTextFieldSpec(const SpecFactory& factory) const
{
    return factory.filledTextFieldSpec(theme(), density());
}

} // namespace QtMaterial
