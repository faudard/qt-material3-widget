#include "qtmaterial/widgets/buttons/qtmaterialfab.h"

#include <Qt>

#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {
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
{
    initializeFab();
}

QtMaterialFab::QtMaterialFab(const QIcon& icon, QWidget* parent)
    : QtMaterialFilledButton(parent)
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
    syncAccessibilityState();
}

bool QtMaterialFab::requiresAccessibleName() const noexcept
{
    return m_requiresAccessibleName;
}

void QtMaterialFab::setRequiresAccessibleName(bool required)
{
    if (m_requiresAccessibleName == required) {
        return;
    }

    m_requiresAccessibleName = required;
    syncAccessibilityState();
    update();
}

QString QtMaterialFab::iconAccessibleName() const
{
    return m_iconAccessibleName;
}

void QtMaterialFab::setIconAccessibleName(const QString& name)
{
    if (m_iconAccessibleName == name) {
        return;
    }

    m_iconAccessibleName = name;
    syncAccessibilityState();
    update();
}

QString QtMaterialFab::effectiveAccessibleName() const
{
    if (!accessibleName().trimmed().isEmpty()) {
        return accessibleName().trimmed();
    }

    if (!m_iconAccessibleName.trimmed().isEmpty()) {
        return m_iconAccessibleName.trimmed();
    }

    if (!toolTip().trimmed().isEmpty()) {
        return toolTip().trimmed();
    }

    return m_requiresAccessibleName ? QString() : QStringLiteral("Floating action button");
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

    if (accessibleName().trimmed().isEmpty() && !m_iconAccessibleName.trimmed().isEmpty()) {
        setAccessibleName(m_iconAccessibleName.trimmed());
    }

    if (hasUsableAccessibleName()) {
        setAccessibleDescription(QStringLiteral("Floating action button"));
    } else {
        setAccessibleDescription(QStringLiteral(
            "Floating action button requires an accessible name for assistive technologies"));
    }

    if (m_lastAccessibilitySummary != summary) {
        m_lastAccessibilitySummary = summary;
        Q_EMIT accessibilitySummaryChanged(summary);
    }
}

ButtonSpec QtMaterialFab::resolveButtonSpec() const
{
    SpecFactory factory;
    return fabToButtonSpec(factory.fabSpec(theme(), density()));
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
