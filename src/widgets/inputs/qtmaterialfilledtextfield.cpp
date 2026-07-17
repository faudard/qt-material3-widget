#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"
#include "qtmaterial/specs/qtmaterialtextfieldspecresolver.h"


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

TextFieldSpec
QtMaterialFilledTextField::resolveTextFieldSpec() const
{
    return TextFieldSpecResolver()
        .filledTextFieldSpec(
            theme(),
            density());
}

} // namespace QtMaterial
