#pragma once

#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"
#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class SpecFactory;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialFilledTextField : public QtMaterialOutlinedTextField
{
    Q_OBJECT
public:
    explicit QtMaterialFilledTextField(QWidget* parent = nullptr);
    ~QtMaterialFilledTextField() override;

protected:
    ShellVariant shellVariant() const override;
    TextFieldSpec resolveTextFieldSpec(const SpecFactory& factory) const override;
};

} // namespace QtMaterial
