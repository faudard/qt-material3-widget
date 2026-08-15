#pragma once

#include <QString>
#include <QStringList>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/foundation/qtmaterialtokenid.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"

namespace QtMaterial {

class QTMATERIAL3_THEME_IO_EXPORT ThemeTextCodec final {
public:
    // Component identity boundary.
    static QString componentIdToString(ComponentId id);
    static bool componentIdFromString(const QString& text, ComponentId* outId);

    // Universal token identity boundary.
    static QString tokenIdToString(TokenId id);
    static bool tokenIdFromString(
        TokenCategory category,
        const QString& text,
        TokenId* outId);

    // Opaque extension component names.
    static QStringList extensionComponentNames(
        const ComponentTokenOverrides& overrides);
    static ComponentTokenOverride extensionOverrideFor(
        const ComponentTokenOverrides& overrides,
        const QString& extensionName);
    static void setExtensionOverride(
        ComponentTokenOverrides* overrides,
        const QString& extensionName,
        const ComponentTokenOverride& overrideTokens);
};

} // namespace QtMaterial
