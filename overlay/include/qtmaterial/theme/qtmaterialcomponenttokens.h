#pragma once

#include <QHash>
#include <QList>
#include <QString>
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

class ThemeTextCodec;

/**
 * Typed first-party component/variant identifiers.
 *
 * Explicit numeric values are provisional stable IDs for the 0.x line and are
 * consolidated by Spec 010. Do not derive persistence names from enum spelling.
 */
enum class ComponentId : quint32 {
    Button = 0x0100,
    ButtonText = 0x0101,
    ButtonFilled = 0x0102,
    ButtonFilledTonal = 0x0103,
    ButtonOutlined = 0x0104,
    ButtonElevated = 0x0105,

    IconButton = 0x0200,
    IconButtonStandard = 0x0201,
    IconButtonFilled = 0x0202,
    IconButtonFilledTonal = 0x0203,
    IconButtonOutlined = 0x0204,

    FloatingActionButton = 0x0300,
    SmallFloatingActionButton = 0x0301,
    LargeFloatingActionButton = 0x0302,
    ExtendedFloatingActionButton = 0x0303,

    Checkbox = 0x0400,
    RadioButton = 0x0500,
    Switch = 0x0600,

    Dialog = 0x0700,
    NavigationDrawer = 0x0800,
    BottomSheet = 0x0900,
    Banner = 0x0A00,
    Card = 0x0B00,
    TopAppBar = 0x0C00,
    BottomAppBar = 0x0D00,

    Input = 0x0E00,
    TextField = 0x0E10,
    TextFieldOutlined = 0x0E11,
    TextFieldFilled = 0x0E12,
    AutoComplete = 0x0E20,
    AutoCompletePopup = 0x0E21,
    DateField = 0x0E30,

    List = 0x1000,
    ListItem = 0x1001,
    Divider = 0x1100,
    Tabs = 0x1200,
    Snackbar = 0x1300,
    ProgressIndicator = 0x1400,
    NavigationRail = 0x1500,
    Surface = 0x1600,

    Custom = 0xFFFF0000u
};

inline uint qHash(ComponentId id, uint seed = 0) noexcept
{
    return ::qHash(static_cast<quint32>(id), seed);
}

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

    // Component-local extension payload. The map is opaque to ThemeModel:
    // textual interpretation belongs to ThemeIO or component-specific adapters.
    QVariantMap custom;

    bool isEmpty() const;
};

class QTMATERIAL3_THEME_MODEL_EXPORT ComponentTokenOverrides {
public:
    ComponentTokenOverrides();
    ~ComponentTokenOverrides();

    bool contains(ComponentId componentId) const;
    ComponentTokenOverride overrideFor(ComponentId componentId) const;
    void setOverride(ComponentId componentId,
                     const ComponentTokenOverride& overrideTokens);
    void removeOverride(ComponentId componentId);

    void clear();
    QList<ComponentId> componentIds() const;

    bool semanticallyEquals(const ComponentTokenOverrides& other) const;

private:
    // Unknown serialized/plugin component names are preserved opaquely so that
    // ThemeIO can round-trip them without exposing string-key APIs to model/specs.
    QHash<ComponentId, ComponentTokenOverride> m_overrides;
    QHash<QString, ComponentTokenOverride> m_extensionOverrides;

    friend class ThemeTextCodec;
};

} // namespace QtMaterial

Q_DECLARE_METATYPE(QtMaterial::ComponentId)
