#include "qtmaterial/widgets/buttons/qtmaterialextendedfab.h"

#include <QFontMetrics>

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

ButtonSpec extendedFabToButtonSpec(const FabSpec& fab)
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
    spec.horizontalPadding = 20;
    spec.iconSize = fab.iconSize;
    spec.iconSpacing = 12;
    return spec;
}

} // namespace

QtMaterialExtendedFab::QtMaterialExtendedFab(QWidget* parent)
    : QtMaterialExtendedFab(QIcon(), QString(), parent)
{
}

QtMaterialExtendedFab::QtMaterialExtendedFab(const QString& text, QWidget* parent)
    : QtMaterialExtendedFab(QIcon(), text, parent)
{
}

QtMaterialExtendedFab::QtMaterialExtendedFab(const QIcon& icon, const QString& text,
                                             QWidget* parent)
    : QtMaterialFilledButton(parent)
{
    setIcon(icon);
    setText(text);
    setCheckable(false);
}

QtMaterialExtendedFab::~QtMaterialExtendedFab() = default;

void QtMaterialExtendedFab::setFabVariant(QtMaterialFabVariant variant)
{
    if (m_variant == variant) {
        return;
    }

    m_variant = variant;
    invalidateResolvedSpec();
    update();
}

QtMaterialFabVariant QtMaterialExtendedFab::fabVariant() const noexcept
{
    return m_variant;
}

ButtonSpec QtMaterialExtendedFab::resolveButtonSpec() const
{
    SpecFactory factory;
    FabSpec spec = factory.extendedFabSpec(theme(), density());
    applyFabVariant(spec, theme(), m_variant);
    return extendedFabToButtonSpec(spec);
}

QSize QtMaterialExtendedFab::sizeHint() const
{
    const ButtonSpec spec = resolveButtonSpec();

    QFont resolvedFont = font();
    if (theme().typography().contains(spec.labelTypeRole)) {
        resolvedFont = theme().typography().style(spec.labelTypeRole).font;
    }

    const QFontMetrics fm(resolvedFont);
    const int textWidth = text().isEmpty() ? 0 : fm.horizontalAdvance(text());
    const int iconWidth = icon().isNull() ? 0 : spec.iconSize;
    const int spacing = (!icon().isNull() && !text().isEmpty()) ? spec.iconSpacing : 0;
    const int visualWidth = spec.horizontalPadding * 2 + iconWidth + spacing + textWidth;
    return spec.touchTarget.expandedTo(QSize(visualWidth, spec.containerHeight));
}

QSize QtMaterialExtendedFab::minimumSizeHint() const
{
    return sizeHint();
}

} // namespace QtMaterial
