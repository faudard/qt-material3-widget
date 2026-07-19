#include "qtmaterial/widgets/compact/qtmaterialfilterchip.h"

namespace QtMaterial {

QtMaterialFilterChip::QtMaterialFilterChip(
    QWidget* parent)
    : QtMaterialChip(parent)
{
    setVariant(ChipVariant::Filter);
}

QtMaterialFilterChip::QtMaterialFilterChip(
    const QString& text,
    QWidget* parent)
    : QtMaterialChip(text, parent)
{
    setVariant(ChipVariant::Filter);
}

QtMaterialFilterChip::~QtMaterialFilterChip() = default;

} // namespace QtMaterial
