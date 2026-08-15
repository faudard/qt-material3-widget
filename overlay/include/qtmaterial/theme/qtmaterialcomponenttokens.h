#pragma once

#include <QHash>
#include <QList>
#include <QString>
#include <QVariantMap>

#include "qtmaterial/foundation/qtmaterialcomponentid.h"
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolorscheme.h"
#include "qtmaterial/theme/qtmaterialdensitytokens.h"
#include "qtmaterial/theme/qtmaterialelevationscale.h"
#include "qtmaterial/theme/qtmaterialiconsizetokens.h"
#include "qtmaterial/theme/qtmaterialmotiontokens.h"
#include "qtmaterial/theme/qtmaterialshapescale.h"
#include "qtmaterial/theme/qtmaterialstatelayer.h"
#include "qtmaterial/theme/qtmaterialtypographyscale.h"

namespace QtMaterial {

class ThemeTextCodec;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
inline size_t qHash(ComponentId id, size_t seed = 0) noexcept
{
    return ::qHash(static_cast<std::uint32_t>(id), seed);
}
#else
inline uint qHash(ComponentId id, uint seed = 0) noexcept
{
    return ::qHash(static_cast<std::uint32_t>(id), seed);
}
#endif

struct QTMATERIAL3_THEME_MODEL_EXPORT ComponentTokenOverride {
    QHash<ColorRole, QColor> colors;
    QHash<TypeRole, TypographyStyle> typography;
    QHash<ShapeRole, int> shapes;
    QHash<ElevationRole, ElevationStyle> elevations;
    QHash<MotionToken, MotionStyle> motion;
    QHash<DensityRole, int> density;
    QHash<IconSizeRole, int> iconSizes;
    StateLayer stateLayer;
    bool hasStateLayer = false;
    QVariantMap custom;

    bool isEmpty() const;
};

class QTMATERIAL3_THEME_MODEL_EXPORT ComponentTokenOverrides {
public:
    ComponentTokenOverrides();
    ~ComponentTokenOverrides();

    bool contains(ComponentId componentId) const;
    ComponentTokenOverride overrideFor(ComponentId componentId) const;
    void setOverride(
        ComponentId componentId,
        const ComponentTokenOverride& overrideTokens);
    void removeOverride(ComponentId componentId);

    void clear();
    QList<ComponentId> componentIds() const;
    bool semanticallyEquals(const ComponentTokenOverrides& other) const;

private:
    QHash<ComponentId, ComponentTokenOverride> m_overrides;

    // Opaque third-party serialized names. Only ThemeIO may interpret/enumerate.
    QHash<QString, ComponentTokenOverride> m_extensionOverrides;

    friend class ThemeTextCodec;
};

} // namespace QtMaterial

Q_DECLARE_METATYPE(QtMaterial::ComponentId)
