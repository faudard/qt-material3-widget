#pragma once

#include <QString>
#include <QStringList>

#include "qtmaterial/foundation/qtmaterialtokenid.h"
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"

namespace QtMaterial {

/**
 * Internal textual identifier codec used by ThemeIO.
 *
 * Specs/ThemeModel consume ComponentId only. ThemeSerializer uses this codec
 * for canonical first-party names and opaque third-party extension names.
 */
class QTMATERIAL3_THEME_IO_EXPORT ThemeTextCodec final {
public:
    static QString componentIdToString(ComponentId id);
    static bool componentIdFromString(const QString& text, ComponentId* outId);

    static QString tokenIdToString(TokenId id);
    static bool tokenIdFromString(
        TokenCategory category,
        const QString& text,
        TokenId* outId);

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
