#include "qtmaterial/widgets/buttons/qtmaterialextendedfab.h"

#include <QFontMetrics>
#include <Qt>

#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {
namespace {

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
    : QtMaterialFilledButton(parent)
{
    initializeExtendedFab();
}

QtMaterialExtendedFab::QtMaterialExtendedFab(const QString& text, QWidget* parent)
    : QtMaterialFilledButton(parent)
{
    initializeExtendedFab();
    setText(text);
    syncAccessibilityState();
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
    syncAccessibilityState();
}

QString QtMaterialExtendedFab::effectiveAccessibleName() const
{
    if (!accessibleName().trimmed().isEmpty()) {
        return accessibleName().trimmed();
    }

    if (!text().trimmed().isEmpty()) {
        return text().trimmed();
    }

    if (!toolTip().trimmed().isEmpty()) {
        return toolTip().trimmed();
    }

    return QString();
}

bool QtMaterialExtendedFab::hasUsableAccessibleName() const
{
    return !effectiveAccessibleName().trimmed().isEmpty();
}

QString QtMaterialExtendedFab::accessibilitySummary() const
{
    const QString name = effectiveAccessibleName();
    if (!name.isEmpty()) {
        return name;
    }

    return QStringLiteral("Extended floating action button requires text or an accessible name");
}

void QtMaterialExtendedFab::syncAccessibilityState()
{
    if (accessibleName().trimmed().isEmpty() && !text().trimmed().isEmpty()) {
        setAccessibleName(text().trimmed());
    }

    if (hasUsableAccessibleName()) {
        setAccessibleDescription(QStringLiteral("Extended floating action button"));
    } else {
        setAccessibleDescription(QStringLiteral(
            "Extended floating action button requires text or an accessible name for assistive technologies"));
    }
}

ButtonSpec QtMaterialExtendedFab::resolveButtonSpec() const
{
    SpecFactory factory;
    return extendedFabToButtonSpec(factory.extendedFabSpec(theme(), density()));
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
