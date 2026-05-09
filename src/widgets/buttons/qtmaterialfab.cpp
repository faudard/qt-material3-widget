#include "qtmaterial/widgets/buttons/qtmaterialfab.h"

#include <Qt>

#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {

struct QtMaterialFabPrivate {
    bool m_requiresAccessibleName = true;
    QString m_iconAccessibleName;
    QString m_lastAccessibilitySummary;
    QtMaterialFabVariant m_fabVariant = QtMaterialFabVariant::Primary;
};

namespace {

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
    : QtMaterialFilledButton(parent)
    , d_ptr(std::make_unique<QtMaterialFabPrivate>())
{
    initializeFab();
}

QtMaterialFab::QtMaterialFab(const QIcon& icon, QWidget* parent)
    : QtMaterialFilledButton(parent)
    , d_ptr(std::make_unique<QtMaterialFabPrivate>())
{
    initializeFab();
    setIcon(icon);
    syncAccessibilityState();
}

QtMaterialFab::~QtMaterialFab() = default;

void QtMaterialFab::initializeFab()
{
    setText(QString());
    setCheckable(false);
    setFocusPolicy(Qt::StrongFocus);
    setMaterialComponent(QStringLiteral("button"));
    setMaterialVariant(QStringLiteral("fab"));
    setMaterialRole(QStringLiteral("action"));
    syncAccessibilityState();
}

bool QtMaterialFab::requiresAccessibleName() const noexcept
{
    return d_ptr->m_requiresAccessibleName;
}

void QtMaterialFab::setRequiresAccessibleName(bool required)
{
    if (d_ptr->m_requiresAccessibleName == required) {
        return;
    }

    d_ptr->m_requiresAccessibleName = required;
    syncAccessibilityState();
    update();
}

QString QtMaterialFab::iconAccessibleName() const
{
    return d_ptr->m_iconAccessibleName;
}

void QtMaterialFab::setIconAccessibleName(const QString& name)
{
    if (d_ptr->m_iconAccessibleName == name) {
        return;
    }

    d_ptr->m_iconAccessibleName = name;
    syncAccessibilityState();
    update();
}

QString QtMaterialFab::effectiveAccessibleName() const
{
    const QString explicitName = accessibleName().trimmed();
    if (!explicitName.isEmpty()) {
        return explicitName;
    }

    const QString iconName = d_ptr->m_iconAccessibleName.trimmed();
    if (!iconName.isEmpty()) {
        return iconName;
    }

    if (!requiresAccessibleName()) {
        const QString tooltipName = toolTip().trimmed();
        if (!tooltipName.isEmpty()) {
            return tooltipName;
        }

        return QStringLiteral("Floating action button");
    }

    return QString();
}

bool QtMaterialFab::hasUsableAccessibleName() const
{
    return !effectiveAccessibleName().trimmed().isEmpty();
}

QString QtMaterialFab::accessibilitySummary() const
{
    const QString name = effectiveAccessibleName();
    if (!name.isEmpty()) {
        return name;
    }

    return QStringLiteral("Floating action button requires an accessible name");
}

void QtMaterialFab::syncAccessibilityState()
{
    const QString summary = accessibilitySummary();

    if (accessibleName().trimmed().isEmpty() && !d_ptr->m_iconAccessibleName.trimmed().isEmpty()) {
        setAccessibleName(d_ptr->m_iconAccessibleName.trimmed());
    }

    if (hasUsableAccessibleName()) {
        setAccessibleDescription(QStringLiteral("Floating action button"));
    } else {
        setAccessibleDescription(QStringLiteral(
            "Floating action button requires an accessible name for assistive technologies"));
    }

    if (d_ptr->m_lastAccessibilitySummary != summary) {
        d_ptr->m_lastAccessibilitySummary = summary;
        Q_EMIT accessibilitySummaryChanged(summary);
    }
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


static void applyFabVariant(FabSpec* spec, const Theme& theme, QtMaterialFabVariant variant)
{
    if (!spec) {
        return;
    }

    switch (variant) {
    case QtMaterialFabVariant::Primary:
        spec->containerColor = theme.colorScheme().color(ColorRole::PrimaryContainer);
        spec->iconColor = theme.colorScheme().color(ColorRole::OnPrimaryContainer);
        break;

    case QtMaterialFabVariant::Secondary:
        spec->containerColor = theme.colorScheme().color(ColorRole::SecondaryContainer);
        spec->iconColor = theme.colorScheme().color(ColorRole::OnSecondaryContainer);
        break;

    case QtMaterialFabVariant::Tertiary:
        spec->containerColor = theme.colorScheme().color(ColorRole::TertiaryContainer);
        spec->iconColor = theme.colorScheme().color(ColorRole::OnTertiaryContainer);
        break;

    case QtMaterialFabVariant::Surface:
        spec->containerColor = theme.colorScheme().color(ColorRole::SurfaceContainerHigh);
        spec->iconColor = theme.colorScheme().color(ColorRole::Primary);
        break;
    }

    spec->stateLayerColor = spec->iconColor;
}

QtMaterialFabVariant QtMaterialFab::fabVariant() const noexcept
{
    return d_ptr->m_fabVariant;
}

void QtMaterialFab::setFabVariant(QtMaterialFabVariant variant)
{
    if (d_ptr->m_fabVariant == variant) {
        return;
    }

    d_ptr->m_fabVariant = variant;
    updateGeometry();
    update();
}

ButtonSpec QtMaterialFab::resolveButtonSpec() const
{
    SpecFactory factory;
    FabSpec spec = factory.fabSpec(theme(), density());
    applyFabVariant(&spec, theme(), d_ptr->m_fabVariant);
    return fabToButtonSpec(spec);
}


} // namespace QtMaterial
