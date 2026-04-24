#pragma once

#include <QVector>
#include <QWidget>

#include "qtmaterial/theme/qtmaterialcolortoken.h"

namespace QtMaterial {
class Theme;
}

class QLabel;
class QPushButton;
class QGridLayout;

class ThemeColorRolesView : public QWidget
{
    Q_OBJECT

public:
    explicit ThemeColorRolesView(QWidget* parent = nullptr);

public slots:
    void applyTheme(const QtMaterial::Theme& theme);

signals:
    void colorCopied(const QString& roleName, const QString& hex);

private:
    struct RowWidgets
    {
        QtMaterial::ColorRole role;
        QLabel* roleLabel {};
        QPushButton* swatchButton {};
        QLabel* hexLabel {};
        QString hexValue;
    };

    static QString roleName(QtMaterial::ColorRole role);
    static QString swatchStyle(const QColor& fill, const QColor& border);

    QVector<RowWidgets> m_rows;
    QGridLayout* m_grid {};
};
