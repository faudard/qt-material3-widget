#pragma once

#include <QVector>
#include <QWidget>

#include "qtmaterial/theme/qtmaterialcolortoken.h"

namespace QtMaterial {
class Theme;
}

class QLabel;
class QFrame;
class QGridLayout;

class ThemeColorRolesView : public QWidget
{
    Q_OBJECT

public:
    explicit ThemeColorRolesView(QWidget* parent = nullptr);

public slots:
    void applyTheme(const QtMaterial::Theme& theme);

private:
    struct RowWidgets
    {
        QtMaterial::ColorRole role;
        QLabel* roleLabel {};
        QFrame* swatchFrame {};
        QLabel* hexLabel {};
    };

    static QString roleName(QtMaterial::ColorRole role);
    static QString swatchStyle(const QColor& fill, const QColor& border);

    QVector<RowWidgets> m_rows;
    QGridLayout* m_grid {};
};
