#include "qtmaterial/theme/qtmaterialthemeoptions.h"

namespace QtMaterial {

ThemeOptions::ThemeOptions()
    : sourceColor(QStringLiteral("#6750A4"))
    , mode(ThemeMode::Light)
    , preference(ThemePreference::Light)
    , contrast(ContrastMode::Standard)
    , variant(ThemeVariant::TonalSpot)
    , backendPolicy(ColorBackendPolicy::Auto)
{
}

ThemeOptions::~ThemeOptions() = default;

} // namespace QtMaterial
