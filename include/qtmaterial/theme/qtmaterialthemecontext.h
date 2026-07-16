#pragma once

#include <QByteArray>
#include <QObject>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

/**
 * Runtime theme source that can be shared by a widget subtree.
 *
 * ThemeContext owns a resolved Theme snapshot and a monotonically increasing
 * revision. Re-applying an identical snapshot is ignored.
 */
class QTMATERIAL3_THEME_EXPORT ThemeContext final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint64 revision READ revision NOTIFY revisionChanged)

public:
    explicit ThemeContext(QObject* parent = nullptr);
    explicit ThemeContext(const Theme& theme, QObject* parent = nullptr);

    const Theme& theme() const noexcept;
    quint64 revision() const noexcept;

    bool setTheme(const Theme& theme);

signals:
    void themeChanged(const QtMaterial::Theme& theme);
    void revisionChanged(quint64 revision);

private:
    static QByteArray stableFingerprint(const Theme& theme);

    Theme m_theme;
    QByteArray m_themeFingerprint;
    quint64 m_revision = 0;
};

} // namespace QtMaterial
