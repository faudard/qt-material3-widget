#pragma once
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
namespace QtMaterial {
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialElevatedButton : public QtMaterialFilledButton
{
    Q_OBJECT
public:
    explicit QtMaterialElevatedButton(QWidget* parent = nullptr);
    ~QtMaterialElevatedButton() override;
};
} // namespace QtMaterial
