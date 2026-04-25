#pragma once

#include <QHash>
#include <QVariantMap>
#include <QStringList>

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

struct QTMATERIAL3_THEME_EXPORT ComponentTokenOverride {
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

class QTMATERIAL3_THEME_EXPORT ComponentTokenOverrides {
public:
    ComponentTokenOverrides();
    ~ComponentTokenOverrides();

    bool contains(const QString& componentName) const;
    ComponentTokenOverride overrideFor(const QString& componentName) const;
    void setOverride(const QString& componentName, const ComponentTokenOverride& overrideTokens);
    void removeOverride(const QString& componentName);
    void clear();
    QStringList componentNames() const;

private:
    QHash<QString, ComponentTokenOverride> m_overrides;
};

} // namespace QtMaterial
