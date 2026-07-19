#pragma once

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/widgets/selection/qtmaterialchip.h"

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialInputChip final
    : public QtMaterialChip
{
public:
    explicit QtMaterialInputChip(
        QWidget* parent = nullptr);

    explicit QtMaterialInputChip(
        const QString& text,
        QWidget* parent = nullptr);

    ~QtMaterialInputChip() override;
};

} // namespace QtMaterial
