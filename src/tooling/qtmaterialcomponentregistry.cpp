#include "qtmaterial/tooling/qtmaterialcomponentregistry.h"

#include <QStringList>

namespace QtMaterial {

QString componentMaturityToString(ComponentMaturity maturity)
{
    switch (maturity) {
    case ComponentMaturity::Complete: return QStringLiteral("complete");
    case ComponentMaturity::Usable: return QStringLiteral("usable");
    case ComponentMaturity::Partial: return QStringLiteral("partial");
    case ComponentMaturity::Skeleton: return QStringLiteral("skeleton");
    case ComponentMaturity::Planned: return QStringLiteral("planned");
    }
    return QStringLiteral("planned");
}

ComponentMaturity componentMaturityFromString(const QString& value)
{
    const QString normalized = value.trimmed().toLower();
    if (normalized == QLatin1String("complete")) return ComponentMaturity::Complete;
    if (normalized == QLatin1String("usable")) return ComponentMaturity::Usable;
    if (normalized == QLatin1String("partial")) return ComponentMaturity::Partial;
    if (normalized == QLatin1String("skeleton")) return ComponentMaturity::Skeleton;
    return ComponentMaturity::Planned;
}

bool isReleaseBlockingMaturity(ComponentMaturity maturity) noexcept
{
    return maturity == ComponentMaturity::Skeleton || maturity == ComponentMaturity::Planned;
}

QVector<ComponentDescriptor> defaultComponentRegistry()
{
    return QVector<ComponentDescriptor>{
#include "qtmaterialcomponentregistry_generated.inc"
    };
}

const ComponentDescriptor* findComponentDescriptor(const QVector<ComponentDescriptor>& registry, const QString& id)
{
    for (const ComponentDescriptor& descriptor : registry) {
        if (descriptor.id == id) {
            return &descriptor;
        }
    }
    return nullptr;
}

} // namespace QtMaterial
