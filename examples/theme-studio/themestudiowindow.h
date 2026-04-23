#pragma once

#include <QMainWindow>

class ThemeStudioController;
class ThemeEditorPanel;
class ThemePreviewPane;

class ThemeStudioWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ThemeStudioWindow(QWidget* parent = nullptr);

private:
    void createDock();
    void createMenus();
    void createStatusBar();

    ThemeStudioController* m_controller {};
    ThemeEditorPanel* m_editorPanel {};
    ThemePreviewPane* m_previewPane {};
};
