#pragma once
#include <QMetaObject>
#include <QObject>
#include <QPointer>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialtheme.h"
namespace QtMaterial {
class ThemeContext;
class QTMATERIAL3_THEME_RUNTIME_EXPORT ThemeObserver : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool themeObservationEnabled READ isThemeObservationEnabled
               WRITE setThemeObservationEnabled
               NOTIFY themeObservationEnabledChanged)
public:
    explicit ThemeObserver(QObject* parent = nullptr);
    ~ThemeObserver() override;
    void setThemeContext(ThemeContext* context);
    ThemeContext* themeContext() const noexcept;
    ThemeContext* effectiveThemeContext() const noexcept;
    bool isThemeObservationEnabled() const noexcept;
public slots:
    void setThemeObservationEnabled(bool enabled);
    void syncTheme();
signals:
    void effectiveThemeContextChanged(QtMaterial::ThemeContext* context);
    void themeObservationEnabledChanged(bool enabled);
protected slots:
    virtual void onThemeChanged(const QtMaterial::Theme& theme) = 0;
private slots:
    void handleContextDestroyed();
private:
    bool refreshConnection(bool notify);
    QPointer<ThemeContext> m_themeContext;
    QPointer<ThemeContext> m_effectiveThemeContext;
    QMetaObject::Connection m_themeConnection;
    QMetaObject::Connection m_destroyedConnection;
    bool m_themeObservationEnabled = true;
};
} // namespace QtMaterial
