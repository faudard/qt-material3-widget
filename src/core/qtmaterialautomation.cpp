#include "qtmaterial/core/qtmaterialautomation.h"
#include "qtmaterial/foundation/qtmaterialmetadataproperties.h"

#include <QObject>
#include <QVariant>

namespace QtMaterial {

namespace {

QString propertyString(const QObject* object, const char* name)
{
    return object ? object->property(name).toString() : QString();
}

void setStringProperty(QObject* object, const char* name, const QString& value)
{
    if (!object) {
        return;
    }

    object->setProperty(name, value);
}

QString interactionStatePropertyString(const QtMaterialInteractionState& state)
{
    QStringList names;
    names.reserve(14);

    names << (state.isEnabled()
                  ? QStringLiteral("enabled")
                  : QStringLiteral("disabled"));

    if (state.isHovered()) {
        names << QStringLiteral("hovered");
    }
    if (state.isFocused()) {
        names << QStringLiteral("focused");
    }
    if (state.isPressed()) {
        names << QStringLiteral("pressed");
    }
    if (state.isCheckable()) {
        names << QStringLiteral("checkable");
    }
    if (state.isChecked()) {
        names << QStringLiteral("checked");
    }
    if (state.isSelected()) {
        names << QStringLiteral("selected");
    }
    if (state.hasError()) {
        names << QStringLiteral("error");
    }
    if (state.isReadOnly()) {
        names << QStringLiteral("readOnly");
    }
    if (state.isIndeterminate()) {
        names << QStringLiteral("indeterminate");
    }
    if (state.isDragged()) {
        names << QStringLiteral("dragged");
    }
    if (state.isBusy()) {
        names << QStringLiteral("busy");
    }
    if (state.isExpanded()) {
        names << QStringLiteral("expanded");
    }
    if (state.isInvalid()) {
        names << QStringLiteral("invalid");
    }

    return names.join(QLatin1Char(' '));
}

} // namespace

const char* QtMaterialAutomation::componentPropertyName() noexcept
{
    return QtMaterialMetadataPropertyNames::component();
}

const char* QtMaterialAutomation::variantPropertyName() noexcept
{
    return QtMaterialMetadataPropertyNames::variant();
}

const char* QtMaterialAutomation::rolePropertyName() noexcept
{
    return QtMaterialMetadataPropertyNames::role();
}

const char* QtMaterialAutomation::testIdPropertyName() noexcept
{
    return QtMaterialMetadataPropertyNames::testId();
}

const char* QtMaterialAutomation::statePropertyName() noexcept
{
    return QtMaterialMetadataPropertyNames::state();
}

void QtMaterialAutomation::setComponent(QObject* object,
                                        const QString& component)
{
    setStringProperty(object, componentPropertyName(), component);
}

void QtMaterialAutomation::setVariant(QObject* object,
                                      const QString& variant)
{
    setStringProperty(object, variantPropertyName(), variant);
}

void QtMaterialAutomation::setRole(QObject* object, const QString& role)
{
    setStringProperty(object, rolePropertyName(), role);
}

void QtMaterialAutomation::setTestId(QObject* object,
                                     const QString& testId)
{
    setStringProperty(object, testIdPropertyName(), testId);
}

void QtMaterialAutomation::setState(QObject* object, const QString& state)
{
    setStringProperty(object, statePropertyName(), state);
}

void QtMaterialAutomation::syncState(
    QObject* object,
    const QtMaterialInteractionState& state)
{
    setState(object, interactionStatePropertyString(state));
}

QString QtMaterialAutomation::component(const QObject* object)
{
    return propertyString(object, componentPropertyName());
}

QString QtMaterialAutomation::variant(const QObject* object)
{
    return propertyString(object, variantPropertyName());
}

QString QtMaterialAutomation::role(const QObject* object)
{
    return propertyString(object, rolePropertyName());
}

QString QtMaterialAutomation::testId(const QObject* object)
{
    return propertyString(object, testIdPropertyName());
}

QString QtMaterialAutomation::state(const QObject* object)
{
    return propertyString(object, statePropertyName());
}

QStringList QtMaterialAutomation::stateNames(const QObject* object)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    return state(object).split(QLatin1Char(' '), Qt::SkipEmptyParts);
#else
    return state(object).split(QLatin1Char(' '), QString::SkipEmptyParts);
#endif
}

} // namespace QtMaterial
