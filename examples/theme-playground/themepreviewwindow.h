#pragma once

#include <QWidget>

class QFrame;
class QLabel;
class QPushButton;
class QPlainTextEdit;

class ThemePreviewWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ThemePreviewWindow(QWidget* parent = nullptr);

private slots:
    void toggleMode();
    void chooseSeedColor();
    void exportTheme();
    void importTheme();
    void copyThemeJson();

private:
    void refreshPreview();
    void setColorChip(QFrame* chip, const QColor& fill, const QColor& border);

    QLabel* m_modeLabel {};
    QLabel* m_seedLabel {};

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
    QPushButton* m_seedButton {};
    QPushButton* m_exportButton {};
    QPushButton* m_importButton {};
    QPushButton* m_copyJsonButton {};
    QPushButton* m_confirmButton {};
    QPushButton* m_dismissButton {};

    QPlainTextEdit* m_jsonPreview {};
};
