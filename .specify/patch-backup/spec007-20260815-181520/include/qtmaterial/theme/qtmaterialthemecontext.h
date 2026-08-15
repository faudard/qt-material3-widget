#pragma once

#include <QObject>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

/**
 * Runtime theme source shared by a widget subtree.
 *
 * ThemeContext owns a copied Theme snapshot and exposes it as const state.
 * Re-applying a structurally equal snapshot is ignored.
 */
class QTMATERIAL3_THEME_RUNTIME_EXPORT ThemeContext final : public QObject
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
    Theme m_theme;
    quint64 m_revision = 0;
};

} // namespace QtMaterial
