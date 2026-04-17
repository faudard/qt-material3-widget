#include "qtmaterial/theme/qtmaterialtheme.h"
namespace QtMaterial {
Theme::Theme() : m_mode(ThemeMode::Light), m_contrast(ContrastMode::Standard), m_options(), m_colorScheme(), m_typography(), m_shapes(), m_elevations(), m_motion(), m_stateLayer() {}
Theme::Theme(const ThemeOptions& options) : m_mode(options.mode), m_contrast(options.contrast), m_options(options), m_colorScheme(), m_typography(), m_shapes(), m_elevations(), m_motion(), m_stateLayer() {}
Theme::~Theme() = default;
ThemeMode Theme::mode() const noexcept { return m_mode; }
void Theme::setMode(ThemeMode mode) noexcept { m_mode = mode; m_options.mode = mode; }
ContrastMode Theme::contrastMode() const noexcept { return m_contrast; }
void Theme::setContrastMode(ContrastMode contrast) noexcept { m_contrast = contrast; m_options.contrast = contrast; }
bool Theme::isDark() const noexcept { return m_mode == ThemeMode::Dark; }
const ThemeOptions& Theme::options() const noexcept { return m_options; }
void Theme::setOptions(const ThemeOptions& options) { m_options = options; m_mode = options.mode; m_contrast = options.contrast; }
const ColorScheme& Theme::colorScheme() const noexcept { return m_colorScheme; }
ColorScheme& Theme::colorScheme() noexcept { return m_colorScheme; }
const TypographyScale& Theme::typography() const noexcept { return m_typography; }
TypographyScale& Theme::typography() noexcept { return m_typography; }
const ShapeScale& Theme::shapes() const noexcept { return m_shapes; }
ShapeScale& Theme::shapes() noexcept { return m_shapes; }
const ElevationScale& Theme::elevations() const noexcept { return m_elevations; }
ElevationScale& Theme::elevations() noexcept { return m_elevations; }
const MotionTokens& Theme::motion() const noexcept { return m_motion; }
MotionTokens& Theme::motion() noexcept { return m_motion; }
const StateLayer& Theme::stateLayer() const noexcept { return m_stateLayer; }
StateLayer& Theme::stateLayer() noexcept { return m_stateLayer; }
} // namespace QtMaterial
