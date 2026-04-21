#pragma once

#include <QWidget>

class QLabel;
class QPushButton;
class QFrame;

class ThemePreviewWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ThemePreviewWindow(QWidget* parent = nullptr);

private slots:
    void toggleMode();

private:
    void refreshPreview();
    void setColorChip(QFrame* chip, const QColor& fill, const QColor& border);

    QLabel* m_modeLabel {};

    QFrame* m_card {};
    QLabel* m_eyebrowLabel {};
    QLabel* m_titleLabel {};
    QLabel* m_supportingLabel {};
    QLabel* m_paletteLabel {};

    QFrame* m_primaryChip {};
    QFrame* m_secondaryChip {};
    QFrame* m_surfaceChip {};
    QFrame* m_errorChip {};

    QPushButton* m_toggleButton {};
    QPushButton* m_confirmButton {};
    QPushButton* m_dismissButton {};
};