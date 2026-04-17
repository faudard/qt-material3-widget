#pragma once
#include <QWidget>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialtheme.h"
namespace QtMaterial {
class QTMATERIAL3_CORE_EXPORT QtMaterialWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QtMaterialWidget(QWidget* parent = nullptr);
    ~QtMaterialWidget() override;
protected:
    const QtMaterial::Theme& theme() const;
    virtual void themeChangedEvent(const QtMaterial::Theme& theme);
private slots:
    void handleThemeChanged(const QtMaterial::Theme& theme);
};
} // namespace QtMaterial
