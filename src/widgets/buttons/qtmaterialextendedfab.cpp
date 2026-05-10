#include "qtmaterial/widgets/buttons/qtmaterialextendedfab.h"
#include "qtmaterial/theme/qtmaterialcolorscheme.h"

#include <QEvent>
#include <QFontMetrics>

#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {

namespace {

constexpr const char* kAutoAccessibleNameProperty = "_qtm3_auto_accessible_name";
constexpr const char* kAutoAccessibleDescriptionProperty = "_qtm3_auto_accessible_description";

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

QString normalizedButtonText(QString value)
{
    value = value.trimmed();
    value.remove(QLatin1Char('&'));
    return value;
}


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

QtMaterialExtendedFab::QtMaterialExtendedFab(QWidget* parent)
    : QtMaterialExtendedFab(QIcon(), QString(), parent)
{
}

QtMaterialExtendedFab::QtMaterialExtendedFab(const QString& text, QWidget* parent)
    : QtMaterialExtendedFab(QIcon(), text, parent)
{
}

QtMaterialExtendedFab::QtMaterialExtendedFab(const QIcon& icon, const QString& text, QWidget* parent)
    : QtMaterialFilledButton(parent)
{
    initializeExtendedFab();
    setIcon(icon);
    setText(text);
    syncExtendedFabAccessibility();
}


QtMaterialFabVariant QtMaterialExtendedFab::fabVariant() const noexcept
{
    return m_fabVariant;
}

void QtMaterialExtendedFab::setFabVariant(QtMaterialFabVariant variant)
{
    if (m_fabVariant == variant) {
        return;
    }

    m_fabVariant = variant;
    invalidateResolvedSpec();
    updateGeometry();
    update();
}

QtMaterialExtendedFab::~QtMaterialExtendedFab() = default;

void QtMaterialExtendedFab::initializeExtendedFab()
{
    setCheckable(false);
    setFocusPolicy(Qt::StrongFocus);
    setMaterialComponent(QStringLiteral("button"));
    setMaterialVariant(QStringLiteral("extended-fab"));
    setMaterialRole(QStringLiteral("action"));
}

QString QtMaterialExtendedFab::effectiveAccessibleName() const
{
    const QString explicitName = accessibleName().trimmed();
    const QString previousAutoName = property(kAutoAccessibleNameProperty).toString().trimmed();

    if (!explicitName.isEmpty() && explicitName != previousAutoName) {
        return explicitName;
    }

    const QString visibleName = normalizedButtonText(text());
    if (!visibleName.isEmpty()) {
        return visibleName;
    }

    const QString tooltipName = toolTip().trimmed();
    if (!tooltipName.isEmpty()) {
        return tooltipName;
    }

    return QString();
}

bool QtMaterialExtendedFab::hasUsableAccessibleName() const
{
    return !effectiveAccessibleName().isEmpty();
}

QString QtMaterialExtendedFab::accessibilitySummary() const
{
    const QString name = effectiveAccessibleName();
    if (!name.isEmpty()) {
        return name;
    }

    return QStringLiteral("Extended floating action button requires text or an accessible name");
}

void QtMaterialExtendedFab::syncExtendedFabAccessibility()
{
    constexpr const char* missingAccessibilityDescription =
        "Extended floating action button requires text or an accessible name for assistive technologies";

    const QString currentName = accessibleName().trimmed();
    const QString previousAutoName = property(kAutoAccessibleNameProperty).toString().trimmed();
    const bool currentNameIsAuto = !previousAutoName.isEmpty() && currentName == previousAutoName;
    const bool currentNameIsExplicit = !currentName.isEmpty() && !currentNameIsAuto;
    const QString desiredAutoName = normalizedButtonText(text());

    if (!currentNameIsExplicit && !desiredAutoName.isEmpty()) {
        setAccessibleName(desiredAutoName);
        setProperty(kAutoAccessibleNameProperty, desiredAutoName);
    } else if (!currentNameIsExplicit && desiredAutoName.isEmpty() && currentNameIsAuto) {
        setAccessibleName(QString());
        setProperty(kAutoAccessibleNameProperty, QString());
    }

    const QString currentDescription = accessibleDescription().trimmed();
    const QString previousAutoDescription = property(kAutoAccessibleDescriptionProperty).toString().trimmed();
    const bool currentDescriptionIsPreviousAuto = !previousAutoDescription.isEmpty()
        && currentDescription == previousAutoDescription;
    const bool currentDescriptionIsLegacyAutoWarning =
        currentDescription == QStringLiteral("Extended floating action button requires visible text or an accessible name for assistive technologies")
        || currentDescription == QString::fromLatin1(missingAccessibilityDescription);
    const bool currentDescriptionIsAuto = currentDescription.isEmpty()
        || currentDescriptionIsPreviousAuto
        || currentDescriptionIsLegacyAutoWarning;
    const QString tooltipDescription = toolTip().trimmed();

    if (!tooltipDescription.isEmpty()) {
        if (currentDescriptionIsAuto) {
            setAccessibleDescription(tooltipDescription);
            setProperty(kAutoAccessibleDescriptionProperty, tooltipDescription);
        }
    } else if (currentDescriptionIsPreviousAuto) {
        setAccessibleDescription(QString());
        setProperty(kAutoAccessibleDescriptionProperty, QString());
    } else if (currentDescriptionIsLegacyAutoWarning && hasUsableAccessibleName()) {
        setAccessibleDescription(QString());
        setProperty(kAutoAccessibleDescriptionProperty, QString());
    }

    if (!hasUsableAccessibleName() && accessibleDescription().trimmed().isEmpty()) {
        setAccessibleDescription(QString::fromLatin1(missingAccessibilityDescription));
        setProperty(kAutoAccessibleDescriptionProperty, QString::fromLatin1(missingAccessibilityDescription));
    }
}

void QtMaterialExtendedFab::syncAccessibilityState()
{
    QtMaterialFilledButton::syncAccessibilityState();
    syncExtendedFabAccessibility();
}

void QtMaterialExtendedFab::contentChangedEvent()
{
    QtMaterialFilledButton::contentChangedEvent();
    syncExtendedFabAccessibility();
}

void QtMaterialExtendedFab::changeEvent(QEvent* event)
{
    QtMaterialFilledButton::changeEvent(event);

    switch (event->type()) {
    case QEvent::EnabledChange:
    case QEvent::ToolTipChange:
        syncExtendedFabAccessibility();
        update();
        break;
    default:
        break;
    }
}

ButtonSpec QtMaterialExtendedFab::resolveButtonSpec() const
{
    SpecFactory factory;
    ButtonSpec spec = extendedFabToButtonSpec(factory.extendedFabSpec(theme(), density()));
    applyFabVariant(spec, theme(), m_fabVariant);
    return spec;
}

QSize QtMaterialExtendedFab::sizeHint() const
{
    const ButtonSpec spec = resolveButtonSpec();
    const QFontMetrics fm(font());
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
