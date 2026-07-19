#include "qtmaterial/widgets/compact/qtmaterialinputchip.h"

namespace QtMaterial {

QtMaterialInputChip::QtMaterialInputChip(
    QWidget* parent)
    : QtMaterialChip(parent)
{
    setVariant(ChipVariant::Input);
    setRemovable(true);
}

QtMaterialInputChip::QtMaterialInputChip(
    const QString& text,
    QWidget* parent)
    : QtMaterialChip(text, parent)
{
    setVariant(ChipVariant::Input);
    setRemovable(true);
}

QtMaterialInputChip::~QtMaterialInputChip() = default;

} // namespace QtMaterial
