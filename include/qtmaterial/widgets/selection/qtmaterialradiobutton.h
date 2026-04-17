#pragma once
#include "qtmaterial/widgets/selection/qtmaterialcheckbox.h"
namespace QtMaterial {
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialRadioButton : public QtMaterialCheckbox
{
    Q_OBJECT
public:
    explicit QtMaterialRadioButton(QWidget* parent = nullptr);
    ~QtMaterialRadioButton() override;
};
} // namespace QtMaterial
