#include "qtmaterial/widgets/buttons/qtmaterialfab.h"
namespace QtMaterial {
QtMaterialFab::QtMaterialFab(QWidget* parent) : QtMaterialFilledButton(parent)
{
    setText(QString());
    setMinimumSize(56, 56);
}
QtMaterialFab::~QtMaterialFab() = default;
QSize QtMaterialFab::sizeHint() const { return QSize(56, 56); }
} // namespace QtMaterial
