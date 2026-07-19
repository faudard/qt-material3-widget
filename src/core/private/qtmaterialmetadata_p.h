#pragma once

#include <QObject>
#include <QString>
#include <QVariant>
#include <QStringList>

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

inline QString interactionStatePropertyString(
    const QtMaterialInteractionState& state)
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

inline void syncInteractionState(
    QObject* object,
    const QtMaterialInteractionState& state)
{
    setStringProperty(
        object,
        QtMaterialMetadataPropertyNames::state(),
        interactionStatePropertyString(state));
}

} // namespace MetadataPrivate
} // namespace QtMaterial
