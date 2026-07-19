#pragma once

#include <QObject>
#include <QPointer>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialtheme.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"

class QWidget;

namespace QtMaterial {

/**
 * Composition-based ThemeContext integration for widgets that already need a
 * Qt base class such as QTableView or QListWidget.
 *
 * Resolution order is identical to QtMaterialWidget:
 * explicit context -> nearest ThemeContextHost parent -> global context.
 */
class QTMATERIAL3_CORE_EXPORT QtMaterialThemeContextBinding final
    : public QObject
{
    Q_OBJECT

public:
    explicit QtMaterialThemeContextBinding(
        QWidget* host,
        QObject* parent = nullptr);
    ~QtMaterialThemeContextBinding() override;

    void setThemeContext(ThemeContext* context);
    ThemeContext* themeContext() const noexcept;
    ThemeContext* effectiveThemeContext() const noexcept;
    const Theme& theme() const noexcept;

signals:
    void effectiveThemeContextChanged(QtMaterial::ThemeContext* context);
    void themeChanged(const QtMaterial::Theme& theme);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void handleThemeChanged(const QtMaterial::Theme& theme);
    void handleThemeContextDestroyed();

private:
    bool refreshEffectiveContext(bool notify);
    void notifyDescendants();

    QPointer<QWidget> m_host;
    QPointer<ThemeContext> m_themeContext;
    QPointer<ThemeContext> m_effectiveThemeContext;
    QMetaObject::Connection m_themeChangedConnection;
    QMetaObject::Connection m_themeDestroyedConnection;
};

} // namespace QtMaterial
