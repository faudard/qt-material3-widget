#include "qtmaterial/widgets/buttons/qtmaterialfab.h"

#include <QEvent>
#include "qtmaterial/specs/qtmaterialactionbuttonspecresolver.h"
#include "qtmaterial/specs/qtmaterialbuttonspecresolver.h"


namespace QtMaterial {

class QtMaterialFabPrivate final
{
public:
    QString iconAccessibleName;
    QString lastAccessibilitySummary;
    bool requiresAccessibleName = true;
    QtMaterialFabVariant fabVariant = QtMaterialFabVariant::Primary;
    QtMaterialFabSize fabSize = QtMaterialFabSize::Standard;
};

namespace {
FabColorVariant toFabColorVariant(QtMaterialFabVariant variant) noexcept
{
    switch (variant) {
    case QtMaterialFabVariant::Secondary:
        return FabColorVariant::Secondary;
    case QtMaterialFabVariant::Tertiary:
        return FabColorVariant::Tertiary;
    case QtMaterialFabVariant::Surface:
        return FabColorVariant::Surface;
    case QtMaterialFabVariant::Primary:
    default:
        return FabColorVariant::Primary;
    }
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
}


QtMaterialFabVariant QtMaterialFab::fabVariant() const noexcept
{
    return d_ptr->fabVariant;
}

void QtMaterialFab::setFabVariant(QtMaterialFabVariant variant)
{
    if (d_ptr->fabVariant == variant) {
        return;
    }

    d_ptr->fabVariant = variant;
    invalidateResolvedSpec();
    updateGeometry();
    update();
}

QtMaterialFabSize QtMaterialFab::fabSize() const noexcept
{
    return d_ptr->fabSize;
}

void QtMaterialFab::setFabSize(QtMaterialFabSize size)
{
    if (d_ptr->fabSize == size) {
        return;
    }

    d_ptr->fabSize = size;
    invalidateResolvedSpec();
    updateGeometry();
    update();
    emit fabSizeChanged(size);
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
    return d_ptr->requiresAccessibleName;
}

void QtMaterialFab::setRequiresAccessibleName(bool required)
{
    if (d_ptr->requiresAccessibleName == required) {
        return;
    }

    d_ptr->requiresAccessibleName = required;
    syncFabAccessibility();
}

QString QtMaterialFab::iconAccessibleName() const
{
    return d_ptr->iconAccessibleName;
}

void QtMaterialFab::setIconAccessibleName(const QString& name)
{
    const QString normalized = name.trimmed();
    if (d_ptr->iconAccessibleName == normalized) {
        return;
    }

    d_ptr->iconAccessibleName = normalized;
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

    if (!d_ptr->iconAccessibleName.trimmed().isEmpty()) {
        return d_ptr->iconAccessibleName.trimmed();
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
    if (d_ptr->lastAccessibilitySummary != summary) {
        d_ptr->lastAccessibilitySummary = summary;
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
        density(),
        toFabColorVariant(d_ptr->fabVariant));
}

void QtMaterialFab::applyExpressiveSpec(ButtonSpec& spec) const
{
    int diameter = 56;
    int iconSize = 24;
    qreal roundRadius = 16.0;

    switch (d_ptr->fabSize) {
    case QtMaterialFabSize::Small:
        diameter = 40;
        iconSize = 24;
        roundRadius = 12.0;
        break;
    case QtMaterialFabSize::Medium:
        diameter = 80;
        iconSize = 28;
        roundRadius = 20.0;
        break;
    case QtMaterialFabSize::Large:
        diameter = 96;
        iconSize = 36;
        roundRadius = 28.0;
        break;
    case QtMaterialFabSize::Standard:
    default:
        break;
    }

    const qreal squareRadius = qMax<qreal>(8.0, roundRadius * 0.5);
    const bool round = expressiveShape() == QtMaterialButtonShape::Round;

    spec.containerHeight = diameter;
    spec.touchTarget = QSize(qMax(48, diameter), qMax(48, diameter));
    spec.iconSize = iconSize;
    spec.horizontalPadding = 0;
    spec.iconSpacing = 0;
    spec.cornerRadius = round ? roundRadius : squareRadius;
    spec.pressedCornerRadius = round ? squareRadius : roundRadius;
    spec.selectedCornerRadius = spec.pressedCornerRadius;
    spec.hasStateShapeMorph = true;
    spec.motionToken = MotionToken::SpatialFast;
    ButtonSpecResolver().resolveRuntimeValues(theme(), &spec);
}

QSize QtMaterialFab::sizeHint() const
{
    ensureSpecResolved();
    const ButtonSpec& spec = currentButtonSpec();
    return spec.touchTarget.expandedTo(QSize(spec.containerHeight, spec.containerHeight));
}

QSize QtMaterialFab::minimumSizeHint() const
{
    return sizeHint();
}

} // namespace QtMaterial
