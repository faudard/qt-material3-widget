#pragma once

#include <QHash>
#include <QString>
#include <QStringList>
#include <QVariantMap>

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

/** Stable ids for first-party components. String overloads remain the extension point. */
enum class ComponentId {
    Button,
    IconButton,
    FloatingActionButton,
    Checkbox,
    RadioButton,
    Switch,
    Dialog,
    NavigationDrawer,
    BottomSheet,
    Banner,
    Card,
    TopAppBar,
    BottomAppBar,
    TextField,
    AutoCompletePopup,
    DateField,
    List,
    ListItem,
    Divider,
    Tabs,
    Snackbar,
    ProgressIndicator,
    Custom
};

QTMATERIAL3_THEME_EXPORT QString componentIdToString(ComponentId id);
QTMATERIAL3_THEME_EXPORT bool componentIdFromString(const QString& name, ComponentId* outId);

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
    bool contains(ComponentId componentId) const;

    ComponentTokenOverride overrideFor(const QString& componentName) const;
    ComponentTokenOverride overrideFor(ComponentId componentId) const;

    void setOverride(const QString& componentName, const ComponentTokenOverride& overrideTokens);
    void setOverride(ComponentId componentId, const ComponentTokenOverride& overrideTokens);

    void removeOverride(const QString& componentName);
    void removeOverride(ComponentId componentId);

    void clear();
    QStringList componentNames() const;

private:
    QHash<QString, ComponentTokenOverride> m_overrides;
};

} // namespace QtMaterial

Q_DECLARE_METATYPE(QtMaterial::ComponentId)
