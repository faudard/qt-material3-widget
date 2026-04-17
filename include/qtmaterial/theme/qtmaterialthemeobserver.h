#pragma once
#include <QObject>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialtheme.h"
namespace QtMaterial {
class QTMATERIAL3_THEME_EXPORT ThemeObserver : public QObject
{
    Q_OBJECT
public:
    explicit ThemeObserver(QObject* parent = nullptr);
    ~ThemeObserver() override;
protected slots:
    virtual void onThemeChanged(const QtMaterial::Theme& theme) = 0;
};
} // namespace QtMaterial
