#pragma once

#include <QString>
#include <QVector>

#include "qtmaterial/theme/qtmaterialthemeoptions.h"

struct ThemePreset
{
    QString id;
    QString displayName;
    QtMaterial::ThemeOptions options;
};

class ThemePresetCatalog
{
public:
    static QVector<ThemePreset> builtInPresets();
    static ThemePreset defaultPreset();
    static bool findPresetById(const QString& id, ThemePreset* outPreset);
};
