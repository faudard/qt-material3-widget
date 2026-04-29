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

QString colorBackendPolicyToString(ColorBackendPolicy policy)
{
    switch (policy) {
    case ColorBackendPolicy::Auto:
        return QStringLiteral("auto");
    case ColorBackendPolicy::PreferMaterialColorUtilities:
        return QStringLiteral("preferMaterialColorUtilities");
    case ColorBackendPolicy::ForceMaterialColorUtilities:
        return QStringLiteral("forceMaterialColorUtilities");
    case ColorBackendPolicy::ForceFallback:
        return QStringLiteral("forceFallback");
    }
    return QStringLiteral("auto");
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
    status.requestedPolicy = options.backendPolicy;
    status.materialColorUtilitiesCompiledIn = isMaterialColorUtilitiesCompiledIn();
    status.materialColorUtilitiesRequested = colorBackendPolicyAllowsMaterialColorUtilities(options.backendPolicy);
    status.requestedBackend = status.materialColorUtilitiesRequested
        ? ThemeColorBackend::MaterialColorUtilities
        : ThemeColorBackend::Fallback;

    if (options.backendPolicy == ColorBackendPolicy::ForceFallback) {
        status.effectiveBackend = ThemeColorBackend::Fallback;
        status.fallbackUsed = true;
        status.diagnostic = QStringLiteral("Deterministic fallback backend forced by ThemeOptions.");
        return status;
    }

    if (status.materialColorUtilitiesCompiledIn) {
        status.effectiveBackend = ThemeColorBackend::MaterialColorUtilities;
        status.fallbackUsed = false;
        status.diagnostic = QStringLiteral("Material Color Utilities backend selected.");
        return status;
    }

    status.effectiveBackend = ThemeColorBackend::Fallback;
    status.fallbackUsed = true;

    if (options.backendPolicy == ColorBackendPolicy::ForceMaterialColorUtilities) {
        status.diagnostic = QStringLiteral(
            "ThemeOptions require Material Color Utilities, but this build does not include the MCU adapter; "
            "using the deterministic fallback backend.");
    } else if (options.backendPolicy == ColorBackendPolicy::PreferMaterialColorUtilities) {
        status.diagnostic = QStringLiteral(
            "ThemeOptions prefer Material Color Utilities, but this build does not include the MCU adapter; "
            "using the deterministic fallback backend.");
    } else {
        status.diagnostic = QStringLiteral(
            "Material Color Utilities is not compiled into this build; Auto selected the deterministic fallback backend.");
    }

    return status;
}

} // namespace QtMaterial
