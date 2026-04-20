#pragma once
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"
namespace QtMaterial {
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialFilledButton : public QtMaterialTextButton
{
    Q_OBJECT
public:
    explicit QtMaterialFilledButton(QWidget* parent = nullptr);
    ~QtMaterialFilledButton() override;

protected:
    void paintEvent(QPaintEvent* event) override;
    ButtonSpec resolveButtonSpec() const override;
};
} // namespace QtMaterial
