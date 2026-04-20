#include "qtmaterial/widgets/buttons/qtmaterialfab.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
namespace QtMaterial {
namespace {
ButtonSpec toButtonSpec(const FabSpec& fab)
{
    ButtonSpec spec;
    spec.containerColor = fab.containerColor;
    spec.labelColor = fab.iconColor;
    spec.iconColor = fab.iconColor;
    spec.disabledContainerColor = fab.disabledContainerColor;
    spec.disabledLabelColor = fab.disabledIconColor;
    spec.stateLayerColor = fab.stateLayerColor;
    spec.shapeRole = fab.shapeRole;
    spec.elevationRole = fab.elevationRole;
    spec.motionToken = fab.motionToken;
    spec.touchTarget = fab.touchTarget;
    spec.containerHeight = fab.containerDiameter;
    spec.horizontalPadding = qMax(0, (fab.containerDiameter - fab.iconSize) / 2);
    spec.iconSize = fab.iconSize;
    spec.iconSpacing = 0;
    return spec;
}
}
QtMaterialFab::QtMaterialFab(QWidget* parent) : QtMaterialFilledButton(parent)
{
    setText(QString());
    setMinimumSize(56, 56);
}
QtMaterialFab::~QtMaterialFab() = default;
ButtonSpec QtMaterialFab::resolveButtonSpec() const
{
    SpecFactory factory;
    return toButtonSpec(factory.fabSpec(theme(), density()));
}
QSize QtMaterialFab::sizeHint() const
{
    ensureSpecResolved();
    return m_spec.touchTarget;
}
QSize QtMaterialFab::minimumSizeHint() const
{
    return sizeHint();
}
} // namespace QtMaterial
