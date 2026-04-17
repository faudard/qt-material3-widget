#pragma once
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"
namespace QtMaterial {
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialFilledTextField : public QtMaterialOutlinedTextField
{
    Q_OBJECT
public:
    explicit QtMaterialFilledTextField(QWidget* parent = nullptr);
    ~QtMaterialFilledTextField() override;
protected:
    void paintEvent(QPaintEvent* event) override;
};
} // namespace QtMaterial
