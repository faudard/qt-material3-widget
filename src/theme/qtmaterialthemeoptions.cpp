#include "qtmaterial/theme/qtmaterialthemeoptions.h"

namespace QtMaterial {

ThemeOptions::ThemeOptions()
    : sourceColor(QStringLiteral("#6750A4"))
    , mode(ThemeMode::Light)
    , contrast(ContrastMode::Standard)
    , expressive(false)
    , useMaterialColorUtilities(true)
{}

ThemeOptions::~ThemeOptions() = default;

} // namespace QtMaterial
