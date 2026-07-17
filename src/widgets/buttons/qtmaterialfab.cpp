#include "qtmaterial/widgets/buttons/qtmaterialfab.h"
#include "qtmaterial/theme/qtmaterialcolorscheme.h"

#include <QEvent>
#include "qtmaterial/specs/qtmaterialactionbuttonspecresolver.h"


namespace QtMaterial {

namespace {

void applyFabVariant(ButtonSpec& spec, const Theme& theme, QtMaterialFabVariant variant)
{
    const ColorScheme& colors = theme.colorScheme();

    switch (variant) {
    case QtMaterialFabVariant::Primary:
        break;
    case QtMaterialFabVariant::Secondary:
        spec.containerColor = colors.color(ColorRole::SecondaryContainer);
        spec.labelColor = colors.color(ColorRole::OnSecondaryContainer);
        spec.iconColor = colors.color(ColorRole::OnSecondaryContainer);
        spec.stateLayerColor = colors.color(ColorRole::OnSecondaryContainer);
        spec.focusRingColor = colors.color(ColorRole::Secondary);
        break;
    case QtMaterialFabVariant::Tertiary:
        spec.containerColor = colors.color(ColorRole::TertiaryContainer);
        spec.labelColor = colors.color(ColorRole::OnTertiaryContainer);
        spec.iconColor = colors.color(ColorRole::OnTertiaryContainer);
        spec.stateLayerColor = colors.color(ColorRole::OnTertiaryContainer);
        spec.focusRingColor = colors.color(ColorRole::Tertiary);
        break;
    case QtMaterialFabVariant::Surface:
        spec.containerColor = colors.color(ColorRole::SurfaceContainerHigh);
        spec.labelColor = colors.color(ColorRole::Primary);
        spec.iconColor = colors.color(ColorRole::Primary);
        spec.stateLayerColor = colors.color(ColorRole::Primary);
        spec.focusRingColor = colors.color(ColorRole::Primary);
        break;
    }
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


QtMaterialFabVariant QtMaterialFab::fabVariant() const noexcept
{
    return m_fabVariant;
}

void QtMaterialFab::setFabVariant(QtMaterialFabVariant variant)
{
    if (m_fabVariant == variant) {
        return;
    }

    m_fabVariant = variant;
    invalidateResolvedSpec();
    updateGeometry();
    update();
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
    constexpr const char* autoAccessibleNameProperty = "_qtm3_auto_accessible_name";

    const QString currentName = accessibleName().trimmed();
    const QString previousAutoName = property(autoAccessibleNameProperty).toString().trimmed();
    if (!currentName.isEmpty() && currentName != previousAutoName) {
        return currentName;
    }

    if (!m_iconAccessibleName.trimmed().isEmpty()) {
        return m_iconAccessibleName.trimmed();
    }

    const QString tooltipName = toolTip().trimmed();
    if (!tooltipName.isEmpty()) {
        return tooltipName;
    }

    if (!requiresAccessibleName()) {
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
    constexpr const char* autoAccessibleNameProperty = "_qtm3_auto_accessible_name";

    const QString summary = accessibilitySummary();
    if (m_lastAccessibilitySummary != summary) {
        m_lastAccessibilitySummary = summary;
        emit accessibilitySummaryChanged(summary);
    }

    const QString effectiveName = effectiveAccessibleName();
    const QString currentName = accessibleName().trimmed();
    const QString previousAutoName = property(autoAccessibleNameProperty).toString().trimmed();
    const bool currentNameIsAuto = currentName.isEmpty()
                                   || (!previousAutoName.isEmpty() && currentName == previousAutoName);

    if (!effectiveName.isEmpty()) {
        if (currentNameIsAuto) {
            QWidget::setAccessibleName(effectiveName);
            setProperty(autoAccessibleNameProperty, effectiveName);
        }
        setAccessibleDescription(QStringLiteral("Floating action button"));
        return;
    }

    if (currentNameIsAuto && !previousAutoName.isEmpty()) {
        QWidget::setAccessibleName(QString());
        setProperty(autoAccessibleNameProperty, QString());
    }

    setAccessibleDescription(QStringLiteral(
        "Floating action button requires an accessible name, tooltip, or icon accessible name for assistive technologies"));
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
    return ActionButtonSpecResolver().fabButtonSpec(
        theme(),
        density());
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
