#include "qtmaterial/widgets/buttons/qtmaterialextendedfab.h"
namespace QtMaterial {
QtMaterialExtendedFab::QtMaterialExtendedFab(QWidget* parent) : QtMaterialFilledButton(parent)
{
    setMinimumHeight(56);
}
QtMaterialExtendedFab::~QtMaterialExtendedFab() = default;
QSize QtMaterialExtendedFab::sizeHint() const { return QSize(120, 56); }
} // namespace QtMaterial
