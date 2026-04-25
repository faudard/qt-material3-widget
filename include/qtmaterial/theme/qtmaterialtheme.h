#pragma once

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolorscheme.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialdensitytokens.h"
#include "qtmaterial/theme/qtmaterialelevationscale.h"
#include "qtmaterial/theme/qtmaterialiconsizetokens.h"
#include "qtmaterial/theme/qtmaterialmotiontokens.h"
#include "qtmaterial/theme/qtmaterialshapescale.h"
#include "qtmaterial/theme/qtmaterialstatelayer.h"
#include "qtmaterial/theme/qtmaterialthemeoptions.h"
#include "qtmaterial/theme/qtmaterialtypographyscale.h"

namespace QtMaterial {

class QTMATERIAL3_THEME_EXPORT Theme {
public:
    Theme();
    explicit Theme(const ThemeOptions& options);
    ~Theme();

    ThemeMode mode() const noexcept;
    void setMode(ThemeMode mode) noexcept;

    ContrastMode contrastMode() const noexcept;
    void setContrastMode(ContrastMode contrast) noexcept;

    bool isDark() const noexcept;

    const ThemeOptions& options() const noexcept;
    void setOptions(const ThemeOptions& options);

    const ColorScheme& colorScheme() const noexcept;
    ColorScheme& colorScheme() noexcept;

    const TypographyScale& typography() const noexcept;
    TypographyScale& typography() noexcept;

    const ShapeScale& shapes() const noexcept;
    ShapeScale& shapes() noexcept;

    const ElevationScale& elevations() const noexcept;
    ElevationScale& elevations() noexcept;

    const MotionTokens& motion() const noexcept;
    MotionTokens& motion() noexcept;

    const StateLayer& stateLayer() const noexcept;
    StateLayer& stateLayer() noexcept;

    const DensityTokens& density() const noexcept;
    DensityTokens& density() noexcept;

    const IconSizeTokens& iconSizes() const noexcept;
    IconSizeTokens& iconSizes() noexcept;

    const ComponentTokenOverrides& componentOverrides() const noexcept;
    ComponentTokenOverrides& componentOverrides() noexcept;

private:
    ThemeMode m_mode;
    ContrastMode m_contrast;
    ThemeOptions m_options;
    ColorScheme m_colorScheme;
    TypographyScale m_typography;
    ShapeScale m_shapes;
    ElevationScale m_elevations;
    MotionTokens m_motion;
    StateLayer m_stateLayer;
    DensityTokens m_density;
    IconSizeTokens m_iconSizes;
    ComponentTokenOverrides m_componentOverrides;
};

} // namespace QtMaterial
