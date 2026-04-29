#pragma once

#include <QVector>
#include <QString>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

enum class ComponentMaturity {
    Complete,
    Usable,
    Partial,
    Skeleton,
    Planned
};

struct QTMATERIAL3_CORE_EXPORT ComponentDescriptor {
    QString id;
    QString name;
    QString family;
    QString publicHeader;
    QString cmakeTarget;
    QString specType;
    QString widgetType;
    QString docsPath;
    QString testTarget;
    QString galleryRoute;
    ComponentMaturity maturity = ComponentMaturity::Planned;
    bool accessibilityRequired = true;
    bool keyboardRequired = true;
    bool automationRequired = true;
};

QTMATERIAL3_CORE_EXPORT QString componentMaturityToString(ComponentMaturity maturity);
QTMATERIAL3_CORE_EXPORT ComponentMaturity componentMaturityFromString(const QString& value);
QTMATERIAL3_CORE_EXPORT QVector<ComponentDescriptor> defaultComponentRegistry();
QTMATERIAL3_CORE_EXPORT const ComponentDescriptor* findComponentDescriptor(const QVector<ComponentDescriptor>& registry,
                                                                            const QString& id);
QTMATERIAL3_CORE_EXPORT bool isReleaseBlockingMaturity(ComponentMaturity maturity) noexcept;

} // namespace QtMaterial
