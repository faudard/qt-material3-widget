#pragma once
#include "qtmaterial/core/qtmaterialoverlaysurface.h"
namespace QtMaterial {
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialNavigationDrawer : public QtMaterialOverlaySurface
{
    Q_OBJECT
public:
    explicit QtMaterialNavigationDrawer(QWidget* parent = nullptr);
    ~QtMaterialNavigationDrawer() override;
protected:
    void paintEvent(QPaintEvent* event) override;
};
} // namespace QtMaterial
