#pragma once

#include <QMainWindow>

class QAction;
class QLabel;
class QCloseEvent;
class ThemeStudioController;
class ThemeEditorPanel;
class ThemePreviewPane;

class ThemeStudioWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ThemeStudioWindow(QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void createDock();
    void createMenus();
    void createToolBar();
    void createStatusBar();
    void wireJsonActions();
    void updateApplyState(bool dirty);

    ThemeStudioController* m_controller {};
    ThemeEditorPanel* m_editorPanel {};
    ThemePreviewPane* m_previewPane {};

    QLabel* m_dirtyLabel {};
    QLabel* m_presetLabel {};
    QLabel* m_modeLabel {};
    QLabel* m_seedLabel {};
    QLabel* m_contrastLabel {};

    QAction* m_applyAction {};
    QAction* m_resetAction {};
    QAction* m_importAction {};
    QAction* m_exportAction {};
};
