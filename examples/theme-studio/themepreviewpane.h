#pragma once

#include <QWidget>

namespace QtMaterial {
class Theme;
class ThemeOptions;
}

class QLabel;
class QPushButton;
class QFrame;
class QTabWidget;
class ThemeColorRolesView;
class ThemeJsonView;
class ThemeCompareView;

class ThemePreviewPane : public QWidget
{
    Q_OBJECT

public:
    explicit ThemePreviewPane(QWidget* parent = nullptr);

    ThemeJsonView* jsonView() const noexcept;
    ThemeColorRolesView* colorRolesView() const noexcept;

public slots:
    void applyTheme(const QtMaterial::Theme& theme);
    void setCompareOptions(const QtMaterial::ThemeOptions& options);

private:
    QWidget* createOverviewPage();
    QWidget* createColorsPage();
    QWidget* createJsonPage();
    QWidget* createComparePage();
    void setColorChip(QFrame* chip, const QColor& fill, const QColor& border);

    QTabWidget* m_tabs {};
    QWidget* m_overviewPage {};
    QWidget* m_colorsPage {};
    QWidget* m_jsonPage {};
    QWidget* m_comparePage {};

    ThemeColorRolesView* m_colorRolesView {};
    ThemeJsonView* m_jsonView {};
    ThemeCompareView* m_compareView {};

    QLabel* m_titleLabel {};
    QLabel* m_supportingLabel {};
    QFrame* m_primaryChip {};
    QFrame* m_secondaryChip {};
    QFrame* m_surfaceChip {};
    QFrame* m_errorChip {};
    QPushButton* m_filledButton {};
    QPushButton* m_tonalButton {};
    QPushButton* m_textButton {};
};
