#pragma once

#include <QList>
#include <QString>
#include <QStringList>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolorscheme.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

enum class ThemeValidationSeverity {
    Warning,
    Error
};

struct QTMATERIAL3_THEME_EXPORT ThemeValidationIssue {
    ThemeValidationSeverity severity = ThemeValidationSeverity::Error;
    QString path;
    QString message;
};

struct QTMATERIAL3_THEME_EXPORT ThemeValidationResult {
    QList<ThemeValidationIssue> issues;

    bool isValid() const;
    bool hasErrors() const;
    QStringList errors() const;
    QStringList warnings() const;
};

struct QTMATERIAL3_THEME_EXPORT ThemeValidationPolicy {
    bool requireCompleteColorScheme = true;
    bool validateContrast = true;
    bool validateTokenRanges = true;
    double minimumTextContrastRatio = 4.5;
    double minimumUiContrastRatio = 3.0;
};

QTMATERIAL3_THEME_EXPORT QString colorRoleName(ColorRole role);
QTMATERIAL3_THEME_EXPORT double colorContrastRatio(const QColor& foreground, const QColor& background);
QTMATERIAL3_THEME_EXPORT ThemeValidationResult validateColorScheme(
    const ColorScheme& scheme,
    const ThemeValidationPolicy& policy = ThemeValidationPolicy{});
QTMATERIAL3_THEME_EXPORT ThemeValidationResult validateTheme(
    const Theme& theme,
    const ThemeValidationPolicy& policy = ThemeValidationPolicy{});

} // namespace QtMaterial
