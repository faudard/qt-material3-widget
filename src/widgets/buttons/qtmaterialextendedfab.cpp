#include "qtmaterial/widgets/buttons/qtmaterialextendedfab.h"

#include <QEvent>
#include <QFontMetrics>

#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {

namespace {

constexpr const char* kAutoAccessibleNameProperty = "_qtm3_auto_accessible_name";

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

QtMaterialExtendedFab::QtMaterialExtendedFab(const QIcon& icon, const QString& text, QWidget* parent)
    : QtMaterialFilledButton(parent)
{
    initializeExtendedFab();
    setIcon(icon);
    setText(text);
    syncAccessibilityState();
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

    const QString visibleText = text().trimmed();
    if (!visibleText.isEmpty()) {
        return visibleText;
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

    return QStringLiteral("Extended floating action button requires visible text or an accessible name");
}

void QtMaterialExtendedFab::syncAccessibilityState()
{
    QtMaterialFilledButton::syncAccessibilityState();

    const QString desiredAutoName = effectiveAccessibleName();
    const QString previousAutoName = property(kAutoAccessibleNameProperty).toString();
    const QString currentName = accessibleName();
    const bool currentNameIsAuto = !previousAutoName.isEmpty() && currentName == previousAutoName;

    if (desiredAutoName.isEmpty()) {
        if (currentNameIsAuto) {
            QWidget::setAccessibleName(QString());
        }
        setProperty(kAutoAccessibleNameProperty, QString());
        setAccessibleDescription(QStringLiteral(
            "Extended floating action button requires visible text or an accessible name for assistive technologies"));
        return;
    }

    if (currentName.trimmed().isEmpty() || currentNameIsAuto) {
        QWidget::setAccessibleName(desiredAutoName);
        setProperty(kAutoAccessibleNameProperty, desiredAutoName);
    }

    setAccessibleDescription(QStringLiteral("Extended floating action button"));
}

void QtMaterialExtendedFab::contentChangedEvent()
{
    QtMaterialFilledButton::contentChangedEvent();
    syncAccessibilityState();
}

void QtMaterialExtendedFab::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::ToolTipChange || event->type() == QEvent::EnabledChange) {
        syncAccessibilityState();
    }

    QtMaterialFilledButton::changeEvent(event);
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
    updateGeometry();
    update();
}

ButtonSpec QtMaterialExtendedFab::resolveButtonSpec() const
{
    SpecFactory factory;
    FabSpec spec = factory.extendedFabSpec(theme(), density());
    applyFabVariant(&spec, theme(), m_fabVariant);
    return extendedFabToButtonSpec(spec);
}

} // namespace QtMaterial
