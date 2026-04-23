#include "themestudiowindow.h"

#include <QDockWidget>
#include <QFileDialog>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>

#include "themeeditorpanel.h"
#include "themejsonview.h"
#include "themepreviewpane.h"
#include "themestudiocontroller.h"
#include "qtmaterial/theme/qtmaterialthemeoptions.h"

using namespace QtMaterial;

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
    createToolBar();
    createStatusBar();
    wireJsonActions();

    connect(m_controller,
            &ThemeStudioController::themeApplied,
            m_previewPane,
            &ThemePreviewPane::applyTheme);

    connect(m_controller,
            &ThemeStudioController::pendingOptionsChanged,
            m_previewPane,
            &ThemePreviewPane::setCompareOptions);

    connect(m_controller,
            &ThemeStudioController::pendingOptionsChanged,
            this,
            [this](const ThemeOptions& options) {
                m_modeLabel->setText(options.mode == ThemeMode::Dark ? tr("Mode: Dark")
                                                                    : tr("Mode: Light"));
                m_seedLabel->setText(tr("Seed: %1").arg(options.sourceColor.name().toUpper()));
            });

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
                m_dirtyLabel->setText(dirty ? tr("Modified") : tr("Saved"));
                setWindowTitle(dirty
                    ? tr("Qt Material 3 - Theme Studio *")
                    : tr("Qt Material 3 - Theme Studio"));
                if (m_applyAction) {
                    m_applyAction->setEnabled(dirty);
                }
                statusBar()->showMessage(
                    dirty ? tr("Unsaved changes") : tr("Theme applied"),
                    2500);
            });

    connect(m_controller,
            &ThemeStudioController::currentFilePathChanged,
            this,
            [this](const QString& path) {
                if (!path.isEmpty()) {
                    statusBar()->showMessage(tr("Current file: %1").arg(path), 4000);
                }
            });

    connect(m_controller,
            &ThemeStudioController::currentPresetChanged,
            this,
            [this](const QString& presetId) {
                m_presetLabel->setText(presetId.isEmpty()
                    ? tr("Preset: Custom")
                    : tr("Preset: %1").arg(presetId));
            });

    m_previewPane->applyTheme(m_controller->currentTheme());
    m_previewPane->setCompareOptions(m_controller->pendingOptions());
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

    m_importAction = fileMenu->addAction(tr("Import JSON..."));
    m_exportAction = fileMenu->addAction(tr("Export JSON..."));
    fileMenu->addSeparator();
    auto* quitAction = fileMenu->addAction(tr("Quit"));

    connect(m_importAction, &QAction::triggered, this, [this]() {
        const QString path = QFileDialog::getOpenFileName(
            this, tr("Import theme"), QString(), tr("Theme JSON (*.json)"));
        if (path.isEmpty()) {
            return;
        }

        QString error;
        if (!m_controller->importJsonFile(path, &error) && !error.isEmpty()) {
            QMessageBox::critical(this, tr("Import failed"), error);
        }
    });

    connect(m_exportAction, &QAction::triggered, this, [this]() {
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

void ThemeStudioWindow::createToolBar()
{
    auto* toolBar = addToolBar(tr("Theme actions"));
    toolBar->setMovable(false);

    m_applyAction = toolBar->addAction(tr("Apply"));
    m_resetAction = toolBar->addAction(tr("Reset"));
    toolBar->addSeparator();
    toolBar->addAction(m_importAction);
    toolBar->addAction(m_exportAction);

    connect(m_applyAction, &QAction::triggered, m_controller, &ThemeStudioController::applyPending);
    connect(m_resetAction, &QAction::triggered, m_controller, &ThemeStudioController::resetToDefaults);

    m_applyAction->setEnabled(m_controller->isDirty());
}

void ThemeStudioWindow::createStatusBar()
{
    m_dirtyLabel = new QLabel(tr("Saved"), this);
    m_presetLabel = new QLabel(tr("Preset: Custom"), this);
    m_modeLabel = new QLabel(tr("Mode: Light"), this);
    m_seedLabel = new QLabel(tr("Seed: --"), this);

    statusBar()->addPermanentWidget(m_dirtyLabel);
    statusBar()->addPermanentWidget(m_presetLabel);
    statusBar()->addPermanentWidget(m_modeLabel);
    statusBar()->addPermanentWidget(m_seedLabel);
    statusBar()->showMessage(tr("Ready"));
}

void ThemeStudioWindow::wireJsonActions()
{
    if (!m_previewPane->jsonView()) {
        return;
    }

    connect(m_previewPane->jsonView(),
            &ThemeJsonView::importRequested,
            m_importAction,
            &QAction::trigger);

    connect(m_previewPane->jsonView(),
            &ThemeJsonView::exportRequested,
            m_exportAction,
            &QAction::trigger);
}
