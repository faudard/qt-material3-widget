#include "qtmaterial/theme/qtmaterialthemecontext.h"

#include <QJsonDocument>

#include "qtmaterial/theme/qtmaterialthemeserializer.h"

namespace QtMaterial {

ThemeContext::ThemeContext(QObject* parent)
    : ThemeContext(Theme(), parent)
{
}

ThemeContext::ThemeContext(const Theme& theme, QObject* parent)
    : QObject(parent)
    , m_theme(theme)
    , m_themeFingerprint(stableFingerprint(theme))
{
}

const Theme& ThemeContext::theme() const noexcept
{
    return m_theme;
}

quint64 ThemeContext::revision() const noexcept
{
    return m_revision;
}

bool ThemeContext::setTheme(const Theme& theme)
{
    const QByteArray nextFingerprint = stableFingerprint(theme);
    if (nextFingerprint == m_themeFingerprint) {
        return false;
    }

    m_theme = theme;
    m_themeFingerprint = nextFingerprint;
    ++m_revision;

    emit themeChanged(m_theme);
    emit revisionChanged(m_revision);
    return true;
}

QByteArray ThemeContext::stableFingerprint(const Theme& theme)
{
    return ThemeSerializer::toJson(theme, QJsonDocument::Compact);
}

} // namespace QtMaterial
