#pragma once

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

enum class ThemeReadMode {
    Lenient,
    Strict,
    UpgradeIfPossible
};

class QTMATERIAL3_THEME_EXPORT ThemeSerializer {
public:
    static constexpr int kCurrentFormatVersion = 2;
    static constexpr int kMinimumReadableFormatVersion = 1;

    static QJsonObject toJsonObject(const Theme& theme);
    static QJsonDocument toJsonDocument(const Theme& theme);
    static QByteArray toJson(const Theme& theme, QJsonDocument::JsonFormat format = QJsonDocument::Indented);
    static bool writeToFile(const Theme& theme,
                            const QString& filePath,
                            QString* errorString = nullptr,
                            QJsonDocument::JsonFormat format = QJsonDocument::Indented);

    static Theme fromJsonObject(const QJsonObject& object,
                                bool* ok = nullptr,
                                QString* errorString = nullptr);
    static Theme fromJsonObject(const QJsonObject& object,
                                ThemeReadMode mode,
                                bool* ok = nullptr,
                                QString* errorString = nullptr);

    static Theme fromJsonDocument(const QJsonDocument& document,
                                  bool* ok = nullptr,
                                  QString* errorString = nullptr);
    static Theme fromJsonDocument(const QJsonDocument& document,
                                  ThemeReadMode mode,
                                  bool* ok = nullptr,
                                  QString* errorString = nullptr);

    static Theme fromJson(const QByteArray& json,
                          bool* ok = nullptr,
                          QString* errorString = nullptr);
    static Theme fromJson(const QByteArray& json,
                          ThemeReadMode mode,
                          bool* ok = nullptr,
                          QString* errorString = nullptr);

    static bool readFromFile(const QString& filePath,
                             Theme* outTheme,
                             QString* errorString = nullptr);
    static bool readFromFile(const QString& filePath,
                             Theme* outTheme,
                             ThemeReadMode mode,
                             QString* errorString = nullptr);
};

} // namespace QtMaterial
