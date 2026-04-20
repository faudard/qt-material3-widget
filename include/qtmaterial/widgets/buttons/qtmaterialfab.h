#pragma once
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
namespace QtMaterial {
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialFab : public QtMaterialFilledButton
{
    Q_OBJECT
public:
    explicit QtMaterialFab(QWidget* parent = nullptr);
    ~QtMaterialFab() override;
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
protected:
    ButtonSpec resolveButtonSpec() const override;
};
} // namespace QtMaterial
