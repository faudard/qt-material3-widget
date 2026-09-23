#pragma once

#include <QVector>
#include <QString>

#include "qtmaterial/qtmaterialglobal.h"

#if defined(QTMATERIAL3_STATIC)
#  define QTMATERIAL3_TOOLING_INTERNAL_EXPORT
#elif defined(QTMATERIAL3_TOOLING_LIBRARY)
#  define QTMATERIAL3_TOOLING_INTERNAL_EXPORT Q_DECL_EXPORT
#else
#  define QTMATERIAL3_TOOLING_INTERNAL_EXPORT Q_DECL_IMPORT
#endif

namespace QtMaterial {

enum class ComponentMaturity {
    Complete,
    Usable,
    Partial,
    Skeleton,
    Planned
};

struct QTMATERIAL3_TOOLING_INTERNAL_EXPORT ComponentDescriptor {
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

QTMATERIAL3_TOOLING_INTERNAL_EXPORT QString componentMaturityToString(ComponentMaturity maturity);
QTMATERIAL3_TOOLING_INTERNAL_EXPORT ComponentMaturity componentMaturityFromString(const QString& value);
QTMATERIAL3_TOOLING_INTERNAL_EXPORT QVector<ComponentDescriptor> defaultComponentRegistry();
QTMATERIAL3_TOOLING_INTERNAL_EXPORT const ComponentDescriptor* findComponentDescriptor(const QVector<ComponentDescriptor>& registry,
                                                                            const QString& id);
QTMATERIAL3_TOOLING_INTERNAL_EXPORT bool isReleaseBlockingMaturity(ComponentMaturity maturity) noexcept;

} // namespace QtMaterial
