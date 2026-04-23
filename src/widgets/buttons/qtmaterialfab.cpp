#include "qtmaterial/widgets/buttons/qtmaterialfab.h"

#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {
namespace {

void applyFabVariant(FabSpec& spec, const Theme& theme, QtMaterialFabVariant variant)
{
    const ColorScheme& colors = theme.colorScheme();

    switch (variant) {
    case QtMaterialFabVariant::Secondary:
        spec.containerColor = colors.color(ColorRole::SecondaryContainer);
        spec.iconColor = colors.color(ColorRole::OnSecondaryContainer);
        break;
    case QtMaterialFabVariant::Tertiary:
        spec.containerColor = colors.color(ColorRole::TertiaryContainer);
        spec.iconColor = colors.color(ColorRole::OnTertiaryContainer);
        break;
    case QtMaterialFabVariant::Surface:
        spec.containerColor = colors.color(ColorRole::SurfaceContainerHigh);
        spec.iconColor = colors.color(ColorRole::Primary);
        break;
    case QtMaterialFabVariant::Primary:
    default:
        spec.containerColor = colors.color(ColorRole::PrimaryContainer);
        spec.iconColor = colors.color(ColorRole::OnPrimaryContainer);
        break;
    }

    spec.stateLayerColor = spec.iconColor;
}

ButtonSpec fabToButtonSpec(const FabSpec& fab)
{
    ButtonSpec spec;
    spec.containerColor = fab.containerColor;
    spec.labelColor = fab.iconColor;
    spec.iconColor = fab.iconColor;
    spec.disabledContainerColor = fab.disabledContainerColor;
    spec.disabledLabelColor = fab.disabledIconColor;
    spec.stateLayerColor = fab.stateLayerColor;
    spec.focusRingColor = fab.iconColor;
    spec.shapeRole = fab.shapeRole;
    spec.elevationRole = fab.elevationRole;
    spec.motionToken = fab.motionToken;
    spec.touchTarget = fab.touchTarget;
    spec.containerHeight = fab.containerDiameter;
    spec.horizontalPadding = 0;
    spec.iconSize = fab.iconSize;
    spec.iconSpacing = 0;
    return spec;
}

} // namespace

QtMaterialFab::QtMaterialFab(QWidget* parent)
    : QtMaterialFab(QIcon(), parent)
{
}

QtMaterialFab::QtMaterialFab(const QIcon& icon, QWidget* parent)
    : QtMaterialFilledButton(parent)
{
    setText(QString());
    setIcon(icon);
    setCheckable(false);
}

QtMaterialFab::~QtMaterialFab() = default;

void QtMaterialFab::setFabVariant(QtMaterialFabVariant variant)
{
    if (m_variant == variant) {
        return;
    }

    m_variant = variant;
    invalidateResolvedSpec();
    update();
}

QtMaterialFabVariant QtMaterialFab::fabVariant() const noexcept
{
    return m_variant;
}

ButtonSpec QtMaterialFab::resolveButtonSpec() const
{
    SpecFactory factory;
    FabSpec spec = factory.fabSpec(theme(), density());
    applyFabVariant(spec, theme(), m_variant);
    return fabToButtonSpec(spec);
}

QSize QtMaterialFab::sizeHint() const
{
    const ButtonSpec spec = resolveButtonSpec();
    return spec.touchTarget.expandedTo(QSize(spec.containerHeight, spec.containerHeight));
}

QSize QtMaterialFab::minimumSizeHint() const
{
    return sizeHint();
}

} // namespace QtMaterial
