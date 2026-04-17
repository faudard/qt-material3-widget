#pragma once
#include "qtmaterial/core/qtmaterialoverlaysurface.h"
namespace QtMaterial {
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialBottomSheet : public QtMaterialOverlaySurface
{
    Q_OBJECT
public:
    explicit QtMaterialBottomSheet(QWidget* parent = nullptr);
    ~QtMaterialBottomSheet() override;
protected:
    void paintEvent(QPaintEvent* event) override;
};
} // namespace QtMaterial
