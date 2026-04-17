#pragma once
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"
namespace QtMaterial {
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialDateField : public QtMaterialOutlinedTextField
{
    Q_OBJECT
public:
    explicit QtMaterialDateField(QWidget* parent = nullptr);
    ~QtMaterialDateField() override;
};
} // namespace QtMaterial
