#pragma once
#include "qtmaterial/core/qtmaterialdensity.h"
#include "qtmaterial/specs/qtmaterialbuttonspec.h"
#include "qtmaterial/specs/qtmaterialcheckboxspec.h"
#include "qtmaterial/specs/qtmaterialdialogspec.h"
#include "qtmaterial/specs/qtmaterialiconbuttonspec.h"
#include "qtmaterial/specs/qtmaterialradiobuttonspec.h"
#include "qtmaterial/specs/qtmaterialswitchspec.h"
#include "qtmaterial/specs/qtmaterialtextfieldspec.h"
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialtheme.h"
namespace QtMaterial {
class QTMATERIAL3_SPECS_EXPORT SpecFactory
{
public:
    SpecFactory();
    ~SpecFactory();
    ButtonSpec textButtonSpec(const Theme& theme, Density density = Density::Default) const;
    ButtonSpec filledButtonSpec(const Theme& theme, Density density = Density::Default) const;
    ButtonSpec filledTonalButtonSpec(const Theme& theme, Density density = Density::Default) const;
    ButtonSpec outlinedButtonSpec(const Theme& theme, Density density = Density::Default) const;
    ButtonSpec elevatedButtonSpec(const Theme& theme, Density density = Density::Default) const;
    IconButtonSpec iconButtonSpec(const Theme& theme, Density density = Density::Default) const;
    CheckboxSpec checkboxSpec(const Theme& theme, Density density = Density::Default) const;
    RadioButtonSpec radioButtonSpec(const Theme& theme, Density density = Density::Default) const;
    SwitchSpec switchSpec(const Theme& theme, Density density = Density::Default) const;
    DialogSpec dialogSpec(const Theme& theme) const;
    TextFieldSpec outlinedTextFieldSpec(const Theme& theme, Density density = Density::Default) const;
    TextFieldSpec filledTextFieldSpec(const Theme& theme, Density density = Density::Default) const;
private:
    int buttonHeightForDensity(Density density) const;
};
} // namespace QtMaterial
