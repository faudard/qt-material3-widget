#include "qtmaterial/widgets/buttons/qtmaterialextendedfab.h"
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
    spec.horizontalPadding = 20;
    spec.iconSize = fab.iconSize;
    spec.iconSpacing = 12;
    return spec;
}
}
QtMaterialExtendedFab::QtMaterialExtendedFab(QWidget* parent) : QtMaterialFilledButton(parent)
{
    setMinimumHeight(56);
}
QtMaterialExtendedFab::~QtMaterialExtendedFab() = default;
ButtonSpec QtMaterialExtendedFab::resolveButtonSpec() const
{
    SpecFactory factory;
    return toButtonSpec(factory.extendedFabSpec(theme(), density()));
}
QSize QtMaterialExtendedFab::sizeHint() const
{
    ensureSpecResolved();
    return QtMaterialFilledButton::sizeHint().expandedTo(QSize(80, m_spec.touchTarget.height()));
}
QSize QtMaterialExtendedFab::minimumSizeHint() const
{
    ensureSpecResolved();
    return QSize(80, m_spec.touchTarget.height());
}
} // namespace QtMaterial
