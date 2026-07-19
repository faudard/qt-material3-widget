#pragma once

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/widgets/selection/qtmaterialchip.h"

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialFilterChip final
    : public QtMaterialChip
{
public:
    explicit QtMaterialFilterChip(
        QWidget* parent = nullptr);

    explicit QtMaterialFilterChip(
        const QString& text,
        QWidget* parent = nullptr);

    ~QtMaterialFilterChip() override;
};

} // namespace QtMaterial
