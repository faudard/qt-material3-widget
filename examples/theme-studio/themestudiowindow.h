#pragma once

#include <QMainWindow>

class QAction;
class QLabel;
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
    void createToolBar();
    void createStatusBar();
    void wireJsonActions();

    ThemeStudioController* m_controller {};
    ThemeEditorPanel* m_editorPanel {};
    ThemePreviewPane* m_previewPane {};

    QLabel* m_dirtyLabel {};
    QLabel* m_presetLabel {};
    QLabel* m_modeLabel {};
    QLabel* m_seedLabel {};

    QAction* m_applyAction {};
    QAction* m_resetAction {};
    QAction* m_importAction {};
    QAction* m_exportAction {};
};
