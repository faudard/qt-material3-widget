#include "themepresetcatalog.h"

#include <QColor>

using namespace QtMaterial;

namespace {

ThemeOptions makeOptions(const QString& hex,
                         ThemeMode mode,
                         ContrastMode contrast = ContrastMode::Standard,
                         bool expressive = false)
{
    ThemeOptions options;
    options.sourceColor = QColor(hex);
    options.mode = mode;
    options.contrast = contrast;
    options.expressive = expressive;
    return options;
}

} // namespace

QVector<ThemePreset> ThemePresetCatalog::builtInPresets()
{
    return {
        {QStringLiteral("material-default-light"),
         QStringLiteral("Material Default Light"),
         makeOptions(QStringLiteral("#6750A4"), ThemeMode::Light)},

        {QStringLiteral("material-default-dark"),
         QStringLiteral("Material Default Dark"),
         makeOptions(QStringLiteral("#6750A4"), ThemeMode::Dark)},

        {QStringLiteral("blue-light"),
         QStringLiteral("Blue Light"),
         makeOptions(QStringLiteral("#0B57D0"), ThemeMode::Light)},

        {QStringLiteral("green-light"),
         QStringLiteral("Green Light"),
         makeOptions(QStringLiteral("#146C2E"), ThemeMode::Light)},

        {QStringLiteral("amber-dark"),
         QStringLiteral("Amber Dark"),
         makeOptions(QStringLiteral("#B06000"), ThemeMode::Dark)},

        {QStringLiteral("rose-expressive"),
         QStringLiteral("Rose Expressive"),
         makeOptions(QStringLiteral("#A73E8C"), ThemeMode::Light, ContrastMode::Medium, true)}
    };
}

ThemePreset ThemePresetCatalog::defaultPreset()
{
    return builtInPresets().constFirst();
}

bool ThemePresetCatalog::findPresetById(const QString& id, ThemePreset* outPreset)
{
    const auto presets = builtInPresets();
    for (const ThemePreset& preset : presets) {
        if (preset.id == id) {
            if (outPreset) {
                *outPreset = preset;
            }
            return true;
        }
    }
    return false;
}
