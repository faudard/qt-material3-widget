#include "qtmaterial/widgets/buttons/qtmaterialfab.h"

#include <QEvent>

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
    syncFabAccessibility();
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
    syncFabAccessibility();
}

QString QtMaterialFab::iconAccessibleName() const
{
    return m_iconAccessibleName;
}

void QtMaterialFab::setIconAccessibleName(const QString& name)
{
    const QString normalized = name.trimmed();
    if (m_iconAccessibleName == normalized) {
        return;
    }

    m_iconAccessibleName = normalized;
    syncFabAccessibility();
}

QString QtMaterialFab::effectiveAccessibleName() const
{
    const QString explicitName = accessibleName().trimmed();
    if (!explicitName.isEmpty()) {
        return explicitName;
    }

    if (!m_iconAccessibleName.trimmed().isEmpty()) {
        return m_iconAccessibleName.trimmed();
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
    return !effectiveAccessibleName().isEmpty();
}

QString QtMaterialFab::accessibilitySummary() const
{
    const QString name = effectiveAccessibleName();
    if (!name.isEmpty()) {
        return name;
    }

    return QStringLiteral("Floating action button requires an explicit accessible name");
}

void QtMaterialFab::syncFabAccessibility()
{
    const QString summary = accessibilitySummary();
    if (m_lastAccessibilitySummary != summary) {
        m_lastAccessibilitySummary = summary;
        emit accessibilitySummaryChanged(summary);
    }

    const QString effectiveName = effectiveAccessibleName();
    if (!effectiveName.isEmpty()) {
        if (accessibleName().trimmed().isEmpty() && !requiresAccessibleName()) {
            QWidget::setAccessibleName(effectiveName);
        }
        setAccessibleDescription(QStringLiteral("Floating action button"));
        return;
    }

    setAccessibleDescription(QStringLiteral(
        "Floating action button requires an explicit accessible name for assistive technologies"));
}

void QtMaterialFab::syncAccessibilityState()
{
    QtMaterialFilledButton::syncAccessibilityState();
    syncFabAccessibility();
}

void QtMaterialFab::contentChangedEvent()
{
    QtMaterialFilledButton::contentChangedEvent();
    setText(QString());
    syncFabAccessibility();
}

void QtMaterialFab::changeEvent(QEvent* event)
{
    switch (event->type()) {
    case QEvent::ToolTipChange:
    case QEvent::AccessibleNameChange:
    case QEvent::AccessibleDescriptionChange:
    case QEvent::EnabledChange:
        syncFabAccessibility();
        break;
    default:
        break;
    }

    QtMaterialFilledButton::changeEvent(event);
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
