#pragma once

#include <QPalette>
#include <QProxyStyle>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

class QTMATERIAL3_CORE_EXPORT QtMaterialProxyStyle : public QProxyStyle {
public:
    explicit QtMaterialProxyStyle(QStyle* baseStyle = nullptr);
    explicit QtMaterialProxyStyle(const Theme& theme, QStyle* baseStyle = nullptr);
    ~QtMaterialProxyStyle() override;

    void setTheme(const Theme& theme);
    QPalette standardPalette() const override;

private:
    QPalette m_palette;
};

} // namespace QtMaterial
