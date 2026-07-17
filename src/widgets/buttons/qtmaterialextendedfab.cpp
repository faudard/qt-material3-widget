#include "qtmaterial/widgets/buttons/qtmaterialextendedfab.h"

#include <QEvent>
#include <QFontMetrics>
#include "qtmaterial/specs/qtmaterialactionbuttonspecresolver.h"


namespace QtMaterial {

namespace {
FabColorVariant resolverVariant(
    QtMaterialFabVariant variant)
{
    switch (variant) {
    case QtMaterialFabVariant::Primary:
        return FabColorVariant::Primary;
    case QtMaterialFabVariant::Secondary:
        return FabColorVariant::Secondary;
    case QtMaterialFabVariant::Tertiary:
        return FabColorVariant::Tertiary;
    case QtMaterialFabVariant::Surface:
        return FabColorVariant::Surface;
    }

    return FabColorVariant::Primary;
}


constexpr const char* kAutoAccessibleNameProperty = "_qtm3_auto_accessible_name";
constexpr const char* kAutoAccessibleDescriptionProperty = "_qtm3_auto_accessible_description";

QString normalizedButtonText(QString value)
{
    value = value.trimmed();
    value.remove(QLatin1Char('&'));
    return value;
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
    constexpr const char* defaultAccessibilityDescription =
        "Extended floating action button";

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
    const QString previousAutoDescription =
        property(kAutoAccessibleDescriptionProperty).toString().trimmed();

    const bool currentDescriptionIsPreviousAuto =
        !previousAutoDescription.isEmpty() && currentDescription == previousAutoDescription;
    const bool currentDescriptionIsLegacyAutoWarning =
        currentDescription == QStringLiteral(
            "Extended floating action button requires visible text or an accessible name for assistive technologies")
        || currentDescription == QString::fromLatin1(missingAccessibilityDescription);
    const bool currentDescriptionIsDefaultAuto =
        currentDescription == QString::fromLatin1(defaultAccessibilityDescription);

    const bool currentDescriptionIsAuto =
        currentDescription.isEmpty()
        || currentDescriptionIsPreviousAuto
        || currentDescriptionIsLegacyAutoWarning
        || currentDescriptionIsDefaultAuto;

    QString desiredDescription;
    const QString tooltipDescription = toolTip().trimmed();
    if (!tooltipDescription.isEmpty()) {
        desiredDescription = tooltipDescription;
    } else if (hasUsableAccessibleName()) {
        desiredDescription = QString::fromLatin1(defaultAccessibilityDescription);
    } else {
        desiredDescription = QString::fromLatin1(missingAccessibilityDescription);
    }

    if (currentDescriptionIsAuto && currentDescription != desiredDescription) {
        setAccessibleDescription(desiredDescription);
        setProperty(kAutoAccessibleDescriptionProperty, desiredDescription);
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
    return ActionButtonSpecResolver()
        .extendedFabButtonSpec(
            theme(),
            density(),
            resolverVariant(m_fabVariant));
}

QSize QtMaterialExtendedFab::sizeHint() const
{
    const ButtonSpec spec = resolveButtonSpec();
    const QFont labelFont = spec.hasResolvedLabelFont
        ? spec.labelFont
        : font();
    const QFontMetrics fm(labelFont);
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
