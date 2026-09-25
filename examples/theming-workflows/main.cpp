#include <QColor>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QTextStream>

#include "qtmaterial/theme/qtmaterialcolorbackend.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/theme/qtmaterialthemeserializer.h"

using namespace QtMaterial;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    QTextStream out(stdout);
    QTextStream err(stderr);

    ThemeOptions options;
    options.sourceColor = QColor(QStringLiteral("#6750A4"));
    options.mode = ThemeMode::Light;
    options.contrast = ContrastMode::Standard;

    ThemeBuilder builder;
    const ThemeColorBackendStatus backend = builder.colorBackendStatus(options);

    Theme theme = builder.build(options);

    ComponentTokenOverride filledButton;
    filledButton.colors.insert(
        ColorRole::Primary, QColor(QStringLiteral("#004A77")));
    filledButton.colors.insert(
        ColorRole::OnPrimary, QColor(QStringLiteral("#FFFFFF")));
    filledButton.density.insert(DensityRole::Default, 2);
    theme.componentOverrides().setOverride(
        ComponentId::ButtonFilled, filledButton);

    const QByteArray json =
        ThemeSerializer::toJson(theme, QJsonDocument::Indented);

    bool ok = false;
    QString error;
    const Theme restored = ThemeSerializer::fromJson(
        json, ThemeReadMode::Strict, &ok, &error);
    if (!ok) {
        err << "Strict Theme JSON read failed: " << error << Qt::endl;
        return 1;
    }

    if (app.arguments().size() > 1
        && !ThemeSerializer::writeToFile(
            restored,
            app.arguments().at(1),
            &error,
            QJsonDocument::Indented)) {
        err << "Could not write theme snapshot: " << error << Qt::endl;
        return 2;
    }

    ThemeManager& manager = ThemeManager::instance();
    manager.setTheme(restored, ThemeChangeReason::External);
    const quint64 lightRevision = manager.revision();

    ThemeOptions dark = manager.options();
    dark.mode = ThemeMode::Dark;
    manager.setThemeOptions(dark);

    out << "Theme JSON format: "
        << ThemeSerializer::kCurrentFormatVersion << Qt::endl;
    out << "Seed: "
        << restored.options().sourceColor.name(QColor::HexRgb).toUpper()
        << Qt::endl;
    out << "Effective backend: "
        << themeColorBackendToString(backend.effectiveBackend) << Qt::endl;
    out << "Backend diagnostic: " << backend.diagnostic << Qt::endl;
    out << "Runtime revisions: " << lightRevision
        << " -> " << manager.revision() << Qt::endl;
    out << "Strict JSON round-trip: OK" << Qt::endl;

    return 0;
}
