#include "qtmaterial/widgets/inputs/qtmaterialdatefield.h"
namespace QtMaterial {
QtMaterialDateField::QtMaterialDateField(QWidget* parent) : QtMaterialOutlinedTextField(parent) { setLabelText(QStringLiteral("Date")); }
QtMaterialDateField::~QtMaterialDateField() = default;
} // namespace QtMaterial
