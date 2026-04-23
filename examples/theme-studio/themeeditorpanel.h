#pragma once

#include <QWidget>

namespace QtMaterial {
enum class ThemeMode : int;
enum class ContrastMode : int;
class ThemeOptions;
}

class QPushButton;
class QComboBox;
class QCheckBox;
class QLabel;

class ThemeStudioController;

class ThemeEditorPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ThemeEditorPanel(ThemeStudioController* controller, QWidget* parent = nullptr);

private slots:
    void chooseSeedColor();
    void syncUiFromOptions(const QtMaterial::ThemeOptions& options);

private:
    ThemeStudioController* m_controller {};

    QComboBox* m_presetCombo {};
    QLabel* m_seedPreview {};
    QPushButton* m_seedButton {};
    QComboBox* m_modeCombo {};
    QComboBox* m_contrastCombo {};
    QCheckBox* m_expressiveCheck {};
    QPushButton* m_applyButton {};
    QPushButton* m_resetButton {};
    QPushButton* m_importButton {};
    QPushButton* m_exportButton {};
};
