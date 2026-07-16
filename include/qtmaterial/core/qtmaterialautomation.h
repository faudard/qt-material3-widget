#pragma once

#include <QString>
#include <QStringList>

#include "qtmaterial/foundation/qtmaterialinteractionstate.h"
#include "qtmaterial/qtmaterialglobal.h"

class QObject;

namespace QtMaterial {

class QTMATERIAL3_CORE_EXPORT QtMaterialAutomation final
{
public:
    QtMaterialAutomation() = delete;

    static const char* componentPropertyName() noexcept;
    static const char* variantPropertyName() noexcept;
    static const char* rolePropertyName() noexcept;
    static const char* testIdPropertyName() noexcept;
    static const char* statePropertyName() noexcept;

    static void setComponent(QObject* object, const QString& component);
    static void setVariant(QObject* object, const QString& variant);
    static void setRole(QObject* object, const QString& role);
    static void setTestId(QObject* object, const QString& testId);
    static void setState(QObject* object, const QString& state);
    static void syncState(QObject* object, const QtMaterialInteractionState& state);

    static QString component(const QObject* object);
    static QString variant(const QObject* object);
    static QString role(const QObject* object);
    static QString testId(const QObject* object);
    static QString state(const QObject* object);
    static QStringList stateNames(const QObject* object);
};

} // namespace QtMaterial
