#pragma once

#include <QWidget>

#include "qtmaterial/theme/qtmaterialtheme.h"

class QLabel;
class QPushButton;
class QFrame;
class QTabWidget;

class ThemePreviewPane : public QWidget
{
    Q_OBJECT

public:
    explicit ThemePreviewPane(QWidget* parent = nullptr);

public slots:
    void applyTheme(const QtMaterial::Theme& theme);

private:
    QWidget* createOverviewPage();
    void setColorChip(QFrame* chip, const QColor& fill, const QColor& border);

    QTabWidget* m_tabs {};

    QWidget* m_overviewPage {};
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
