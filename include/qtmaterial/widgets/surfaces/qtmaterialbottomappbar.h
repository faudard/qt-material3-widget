#pragma once
#include "qtmaterial/core/qtmaterialsurface.h"
namespace QtMaterial {
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialBottomAppBar : public QtMaterialSurface
{
    Q_OBJECT
public:
    explicit QtMaterialBottomAppBar(QWidget* parent = nullptr);
    ~QtMaterialBottomAppBar() override;
protected:
    void paintEvent(QPaintEvent* event) override;
};
} // namespace QtMaterial
