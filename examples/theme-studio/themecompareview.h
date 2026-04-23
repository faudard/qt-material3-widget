#pragma once

#include <QWidget>

#include "qtmaterial/theme/qtmaterialthemeoptions.h"

namespace QtMaterial {
class Theme;
}

class QLabel;
class QFrame;

class ThemeCompareView : public QWidget
{
    Q_OBJECT

public:
    explicit ThemeCompareView(QWidget* parent = nullptr);

public slots:
    void applyTheme(const QtMaterial::Theme& theme);
    void setBaseOptions(const QtMaterial::ThemeOptions& options);

private:
    QWidget* createPane(const QString& title,
                        QLabel** titleLabel,
                        QFrame** primaryChip,
                        QFrame** surfaceChip,
                        QFrame** errorChip);

    void applyPaneStyles(const QtMaterial::Theme& theme,
                         QLabel* titleLabel,
                         QFrame* primaryChip,
                         QFrame* surfaceChip,
                         QFrame* errorChip);

    QtMaterial::ThemeOptions m_baseOptions;

    QLabel* m_lightTitle {};
    QLabel* m_darkTitle {};

    QFrame* m_lightPrimaryChip {};
    QFrame* m_lightSurfaceChip {};
    QFrame* m_lightErrorChip {};

    QFrame* m_darkPrimaryChip {};
    QFrame* m_darkSurfaceChip {};
    QFrame* m_darkErrorChip {};
};
