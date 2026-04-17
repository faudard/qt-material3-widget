#pragma once
#include "qtmaterial/core/qtmaterialabstractbutton.h"
namespace QtMaterial {
class QTMATERIAL3_CORE_EXPORT QtMaterialSelectionControl : public QtMaterialAbstractButton
{
    Q_OBJECT
public:
    explicit QtMaterialSelectionControl(QWidget* parent = nullptr);
    ~QtMaterialSelectionControl() override;
    Qt::Alignment labelAlignment() const noexcept;
    void setLabelAlignment(Qt::Alignment alignment);
    int spacing() const noexcept;
    void setSpacing(int spacing);
protected:
    QRect indicatorRect() const;
    QRect labelRect() const;
    QSize sizeHint() const override;
private:
    Qt::Alignment m_labelAlignment;
    int m_spacing;
};
} // namespace QtMaterial
