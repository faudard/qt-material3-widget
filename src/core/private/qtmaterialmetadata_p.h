#pragma once

#include <QObject>
#include <QString>

#include "qtmaterial/foundation/qtmaterialinteractionstate.h"
#include "qtmaterial/foundation/qtmaterialmetadataproperties.h"

namespace QtMaterial {
namespace MetadataPrivate {

inline QString stringProperty(
    const QObject* object,
    const char* propertyName)
{
    return object
        ? object->property(propertyName).toString()
        : QString();
}

inline void setStringProperty(
    QObject* object,
    const char* propertyName,
    const QString& value)
{
    if (object) {
        object->setProperty(propertyName, value);
    }
}

inline void syncInteractionState(
    QObject* object,
    const QtMaterialInteractionState& state)
{
    setStringProperty(
        object,
        QtMaterialMetadataPropertyNames::state(),
        state.toPropertyString());
}

} // namespace MetadataPrivate
} // namespace QtMaterial
