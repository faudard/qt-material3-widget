#include "themestudiowindow.h"

#include <QDockWidget>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>

#include "themeeditorpanel.h"
#include "themepreviewpane.h"
#include "themestudiocontroller.h"

ThemeStudioWindow::ThemeStudioWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_controller(new ThemeStudioController(this))
    , m_editorPanel(new ThemeEditorPanel(m_controller, this))
    , m_previewPane(new ThemePreviewPane(this))
{
    setWindowTitle(tr("Qt Material 3 - Theme Studio"));
    setCentralWidget(m_previewPane);

    createDock();
    createMenus();
    createStatusBar();

    connect(m_controller,
            &ThemeStudioController::themeApplied,
            m_previewPane,
            &ThemePreviewPane::applyTheme);

    connect(m_controller,
            &ThemeStudioController::errorOccurred,
            this,
            [this](const QString& message) {
                QMessageBox::critical(this, tr("Theme Studio"), message);
            });

    connect(m_controller,
            &ThemeStudioController::dirtyStateChanged,
            this,
            [this](bool dirty) {
                statusBar()->showMessage(
                    dirty ? tr("Unsaved changes") : tr("Theme applied"),
                    2500);
            });

    m_previewPane->applyTheme(m_controller->currentTheme());
}

void ThemeStudioWindow::createDock()
{
    auto* dock = new QDockWidget(tr("Theme"), this);
    dock->setObjectName(QStringLiteral("themeEditorDock"));
    dock->setWidget(m_editorPanel);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
}

void ThemeStudioWindow::createMenus()
{
    auto* fileMenu = menuBar()->addMenu(tr("&File"));

    auto* importAction = fileMenu->addAction(tr("Import JSON..."));
    auto* exportAction = fileMenu->addAction(tr("Export JSON..."));
    fileMenu->addSeparator();
    auto* quitAction = fileMenu->addAction(tr("Quit"));

    connect(importAction, &QAction::triggered, this, [this]() {
        const QString path = QFileDialog::getOpenFileName(
            this, tr("Import theme"), QString(), tr("Theme JSON (*.json)"));
        if (path.isEmpty()) {
            return;
        }
        m_controller->importJsonFile(path);
    });

    connect(exportAction, &QAction::triggered, this, [this]() {
        const QString path = QFileDialog::getSaveFileName(
            this, tr("Export theme"), QStringLiteral("theme.json"), tr("Theme JSON (*.json)"));
        if (path.isEmpty()) {
            return;
        }

        QString error;
        if (!m_controller->exportJsonFile(path, &error) && !error.isEmpty()) {
            QMessageBox::critical(this, tr("Export failed"), error);
        }
    });

    connect(quitAction, &QAction::triggered, this, &QWidget::close);
}

void ThemeStudioWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}
