#pragma once
#include "qtmaterial/core/qtmaterialsurface.h"
namespace QtMaterial {
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialCard : public QtMaterialSurface
{
    Q_OBJECT
public:
    explicit QtMaterialCard(QWidget* parent = nullptr);
    ~QtMaterialCard() override;
protected:
    void paintEvent(QPaintEvent* event) override;
};
} // namespace QtMaterial
