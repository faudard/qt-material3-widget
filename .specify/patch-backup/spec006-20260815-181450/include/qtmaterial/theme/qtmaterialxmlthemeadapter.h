#pragma once

#include <QByteArray>
#include <QString>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

class QTMATERIAL3_THEME_EXPORT XmlThemeAdapter
{
public:
    static Theme fromQtMaterialXml(
        const QByteArray& xml,
        bool* ok = nullptr,
        QString* errorString = nullptr);

    static bool readQtMaterialXmlFile(
        const QString& filePath,
        Theme* outTheme,
        QString* errorString = nullptr);

    static QByteArray toQtMaterialXml(const Theme& theme);

    static bool writeQtMaterialXmlFile(
        const Theme& theme,
        const QString& filePath,
        QString* errorString = nullptr);
};

} // namespace QtMaterial
