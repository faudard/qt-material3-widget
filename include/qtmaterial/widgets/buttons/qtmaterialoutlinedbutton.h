#pragma once
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"
namespace QtMaterial {
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialOutlinedButton : public QtMaterialTextButton
{
    Q_OBJECT
public:
    explicit QtMaterialOutlinedButton(QWidget* parent = nullptr);
    ~QtMaterialOutlinedButton() override;

protected:
    void paintEvent(QPaintEvent* event) override;
    ButtonSpec resolveButtonSpec() const override;
};
} // namespace QtMaterial
