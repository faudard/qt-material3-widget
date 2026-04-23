#pragma once

#include <QWidget>

#include "qtmaterial/theme/qtmaterialthemeoptions.h"

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
