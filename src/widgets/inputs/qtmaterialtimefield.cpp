#include "qtmaterial/widgets/inputs/qtmaterialtimefield.h"

namespace QtMaterial {

QtMaterialTimeField::QtMaterialTimeField(QWidget* parent)
    : QTimeEdit(parent)
{
    setObjectName(QStringLiteral("qtmaterial_time_field"));
    setDisplayFormat(QStringLiteral("HH:mm"));
    setKeyboardTracking(false);
    setFocusPolicy(Qt::StrongFocus);
    setMinimumHeight(48);
    setAccessibleName(tr("Time"));
}

QtMaterialTimeField::~QtMaterialTimeField() = default;

} // namespace QtMaterial
