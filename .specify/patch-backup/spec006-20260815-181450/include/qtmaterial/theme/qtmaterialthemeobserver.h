#pragma once

#include <QMetaObject>
#include <QObject>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

class QTMATERIAL3_THEME_EXPORT ThemeObserver : public QObject {
    Q_OBJECT
public:
    explicit ThemeObserver(QObject* parent = nullptr);
    ~ThemeObserver() override;

    bool isThemeObservationEnabled() const noexcept;

public slots:
    void setThemeObservationEnabled(bool enabled);
    void syncTheme();

protected slots:
    virtual void onThemeChanged(const QtMaterial::Theme& theme) = 0;

private:
    QMetaObject::Connection m_themeConnection;
    bool m_themeObservationEnabled;
};

} // namespace QtMaterial
