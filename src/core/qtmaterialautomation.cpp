#include "qtmaterial/core/qtmaterialautomation.h"

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
} // namespace

const char* QtMaterialAutomation::componentPropertyName() noexcept { return "materialComponent"; }
const char* QtMaterialAutomation::variantPropertyName() noexcept { return "materialVariant"; }
const char* QtMaterialAutomation::rolePropertyName() noexcept { return "materialRole"; }
const char* QtMaterialAutomation::testIdPropertyName() noexcept { return "materialTestId"; }
const char* QtMaterialAutomation::statePropertyName() noexcept { return "materialState"; }

void QtMaterialAutomation::setComponent(QObject* object, const QString& component)
{
    setStringProperty(object, componentPropertyName(), component);
}

void QtMaterialAutomation::setVariant(QObject* object, const QString& variant)
{
    setStringProperty(object, variantPropertyName(), variant);
}

void QtMaterialAutomation::setRole(QObject* object, const QString& role)
{
    setStringProperty(object, rolePropertyName(), role);
}

void QtMaterialAutomation::setTestId(QObject* object, const QString& testId)
{
    setStringProperty(object, testIdPropertyName(), testId);
}

void QtMaterialAutomation::setState(QObject* object, const QString& state)
{
    setStringProperty(object, statePropertyName(), state);
}

void QtMaterialAutomation::syncState(QObject* object, const QtMaterialInteractionState& state)
{
    setState(object, state.toPropertyString());
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
