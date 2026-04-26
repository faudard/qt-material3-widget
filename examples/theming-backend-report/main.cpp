#include <QCoreApplication>
#include <QColor>
#include <QTextStream>

#include "qtmaterial/theme/qtmaterialcolorbackend.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    Q_UNUSED(app);

    QTextStream out(stdout);

    ThemeOptions fallbackOptions;
    fallbackOptions.sourceColor = QColor(QStringLiteral("#6750A4"));
    fallbackOptions.useMaterialColorUtilities = false;

    ThemeOptions mcuOptions = fallbackOptions;
    mcuOptions.useMaterialColorUtilities = true;

    ThemeBuilder builder;
    const ThemeColorBackendStatus fallbackStatus = builder.colorBackendStatus(fallbackOptions);
    const ThemeColorBackendStatus mcuStatus = builder.colorBackendStatus(mcuOptions);

    out << "Compiled backend: " << themeColorBackendToString(ThemeBuilder::compiledColorBackend()) << Qt::endl;
    out << "MCU available: " << (ThemeBuilder::isMaterialColorUtilitiesAvailable() ? "yes" : "no") << Qt::endl;
    out << Qt::endl;

    out << "Fallback request:" << Qt::endl;
    out << "  effective backend: " << themeColorBackendToString(fallbackStatus.effectiveBackend) << Qt::endl;
    out << "  diagnostic: " << fallbackStatus.diagnostic << Qt::endl;
    out << Qt::endl;

    out << "MCU request:" << Qt::endl;
    out << "  effective backend: " << themeColorBackendToString(mcuStatus.effectiveBackend) << Qt::endl;
    out << "  fallback used: " << (mcuStatus.fallbackUsed ? "yes" : "no") << Qt::endl;
    out << "  diagnostic: " << mcuStatus.diagnostic << Qt::endl;
    return 0;
}
