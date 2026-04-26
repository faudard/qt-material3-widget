#include "qtmaterial/theme/qtmaterialcolorbackend.h"

namespace QtMaterial {

QString themeColorBackendToString(ThemeColorBackend backend)
{
    switch (backend) {
    case ThemeColorBackend::Fallback:
        return QStringLiteral("fallback");
    case ThemeColorBackend::MaterialColorUtilities:
        return QStringLiteral("materialColorUtilities");
    }
    return QStringLiteral("unknown");
}

ThemeColorBackend compiledThemeColorBackend() noexcept
{
#if defined(QTMATERIAL3_HAS_MCU) && QTMATERIAL3_HAS_MCU
    return ThemeColorBackend::MaterialColorUtilities;
#else
    return ThemeColorBackend::Fallback;
#endif
}

bool isMaterialColorUtilitiesCompiledIn() noexcept
{
    return compiledThemeColorBackend() == ThemeColorBackend::MaterialColorUtilities;
}

double contrastModeToMaterialColorUtilitiesLevel(ContrastMode contrast) noexcept
{
    switch (contrast) {
    case ContrastMode::Standard:
        return 0.0;
    case ContrastMode::Medium:
        return 0.5;
    case ContrastMode::High:
        return 1.0;
    }
    return 0.0;
}

ThemeColorBackendStatus resolveThemeColorBackend(const ThemeOptions& options)
{
    ThemeColorBackendStatus status;
    status.materialColorUtilitiesRequested = options.useMaterialColorUtilities;
    status.materialColorUtilitiesCompiledIn = isMaterialColorUtilitiesCompiledIn();
    status.requestedBackend = status.materialColorUtilitiesRequested
        ? ThemeColorBackend::MaterialColorUtilities
        : ThemeColorBackend::Fallback;

    if (status.materialColorUtilitiesRequested && status.materialColorUtilitiesCompiledIn) {
        status.effectiveBackend = ThemeColorBackend::MaterialColorUtilities;
        status.fallbackUsed = false;
        status.diagnostic = QStringLiteral("Material Color Utilities backend selected.");
        return status;
    }

    status.effectiveBackend = ThemeColorBackend::Fallback;
    status.fallbackUsed = true;

    if (status.materialColorUtilitiesRequested) {
        status.diagnostic = QStringLiteral(
            "ThemeOptions requested Material Color Utilities, but this build does not include the MCU adapter; "
            "using the deterministic fallback backend.");
    } else {
        status.diagnostic = QStringLiteral("Deterministic fallback backend selected by ThemeOptions.");
    }

    return status;
}

} // namespace QtMaterial
