#include "themeeditorpanel.h"

#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSignalBlocker>
#include <QVBoxLayout>

#include "themepresetcatalog.h"
#include "themestudiocontroller.h"
#include "qtmaterial/theme/qtmaterialthemeoptions.h"

using namespace QtMaterial;

namespace {

QString chipStyle(const QColor& color)
{
    return QStringLiteral(
               "QLabel {"
               " background-color: %1;"
               " border: 1px solid %2;"
               " border-radius: 8px;"
               " min-height: 28px;"
               "}")
        .arg(color.name(), color.darker(130).name());
}

} // namespace

ThemeEditorPanel::ThemeEditorPanel(ThemeStudioController* controller, QWidget* parent)
    : QWidget(parent)
    , m_controller(controller)
    , m_presetCombo(new QComboBox(this))
    , m_seedPreview(new QLabel(this))
    , m_seedButton(new QPushButton(tr("Choose color"), this))
    , m_modeCombo(new QComboBox(this))
    , m_contrastCombo(new QComboBox(this))
    , m_expressiveCheck(new QCheckBox(tr("Expressive"), this))
    , m_applyButton(new QPushButton(tr("Apply"), this))
    , m_resetButton(new QPushButton(tr("Reset"), this))
    , m_importButton(new QPushButton(tr("Import JSON"), this))
    , m_exportButton(new QPushButton(tr("Export JSON"), this))
{
    m_seedPreview->setMinimumWidth(72);

    m_presetCombo->addItem(tr("Custom"), QString());
    for (const ThemePreset& preset : ThemePresetCatalog::builtInPresets()) {
        m_presetCombo->addItem(preset.displayName, preset.id);
    }

    m_modeCombo->addItem(tr("Light"), static_cast<int>(ThemeMode::Light));
    m_modeCombo->addItem(tr("Dark"), static_cast<int>(ThemeMode::Dark));

    m_contrastCombo->addItem(tr("Standard"), static_cast<int>(ContrastMode::Standard));
    m_contrastCombo->addItem(tr("Medium"), static_cast<int>(ContrastMode::Medium));
    m_contrastCombo->addItem(tr("High"), static_cast<int>(ContrastMode::High));

    auto* presetBox = new QGroupBox(tr("Presets"), this);
    auto* presetLayout = new QVBoxLayout(presetBox);
    presetLayout->addWidget(m_presetCombo);

    auto* generationBox = new QGroupBox(tr("Generation"), this);
    auto* generationLayout = new QGridLayout(generationBox);
    generationLayout->addWidget(new QLabel(tr("Seed"), this), 0, 0);
    generationLayout->addWidget(m_seedPreview, 0, 1);
    generationLayout->addWidget(m_seedButton, 0, 2);
    generationLayout->addWidget(new QLabel(tr("Mode"), this), 1, 0);
    generationLayout->addWidget(m_modeCombo, 1, 1, 1, 2);
    generationLayout->addWidget(new QLabel(tr("Contrast"), this), 2, 0);
    generationLayout->addWidget(m_contrastCombo, 2, 1, 1, 2);
    generationLayout->addWidget(m_expressiveCheck, 3, 0, 1, 3);

    auto* actionsBox = new QGroupBox(tr("Actions"), this);
    auto* actionsLayout = new QVBoxLayout(actionsBox);
    actionsLayout->addWidget(m_applyButton);
    actionsLayout->addWidget(m_resetButton);
    actionsLayout->addWidget(m_importButton);
    actionsLayout->addWidget(m_exportButton);

    auto* root = new QVBoxLayout(this);
    root->addWidget(presetBox);
    root->addWidget(generationBox);
    root->addWidget(actionsBox);
    root->addStretch(1);

    connect(m_presetCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            [this](int index) {
                const QString presetId = m_presetCombo->itemData(index).toString();
                if (!presetId.isEmpty()) {
                    m_controller->applyPreset(presetId);
                }
            });

    connect(m_seedButton, &QPushButton::clicked, this, &ThemeEditorPanel::chooseSeedColor);

    connect(m_modeCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            [this](int index) {
                m_controller->setMode(
                    static_cast<ThemeMode>(m_modeCombo->itemData(index).toInt()));
            });

    connect(m_contrastCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            [this](int index) {
                m_controller->setContrast(
                    static_cast<ContrastMode>(m_contrastCombo->itemData(index).toInt()));
            });

    connect(m_expressiveCheck,
            &QCheckBox::toggled,
            m_controller,
            &ThemeStudioController::setExpressive);

    connect(m_applyButton,
            &QPushButton::clicked,
            m_controller,
            &ThemeStudioController::applyPending);
    connect(m_resetButton,
            &QPushButton::clicked,
            m_controller,
            &ThemeStudioController::resetToDefaults);

    connect(m_importButton, &QPushButton::clicked, this, [this]() {
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

    connect(m_exportButton, &QPushButton::clicked, this, [this]() {
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

    connect(m_controller,
            &ThemeStudioController::pendingOptionsChanged,
            this,
            &ThemeEditorPanel::syncUiFromOptions);

    syncUiFromOptions(m_controller->pendingOptions());
}

void ThemeEditorPanel::chooseSeedColor()
{
    const QColor color = QColorDialog::getColor(
        m_controller->pendingOptions().sourceColor,
        this,
        tr("Choose seed color"));

    if (!color.isValid()) {
        return;
    }

    m_controller->setSeedColor(color);
}

void ThemeEditorPanel::syncUiFromOptions(const ThemeOptions& options)
{
    const QSignalBlocker presetBlocker(m_presetCombo);
    const QSignalBlocker modeBlocker(m_modeCombo);
    const QSignalBlocker contrastBlocker(m_contrastCombo);
    const QSignalBlocker expressiveBlocker(m_expressiveCheck);

    m_seedPreview->setText(options.sourceColor.name().toUpper());
    m_seedPreview->setAlignment(Qt::AlignCenter);
    m_seedPreview->setStyleSheet(chipStyle(options.sourceColor));

    const QString presetId = m_controller->currentPresetId();
    const int presetIndex = presetId.isEmpty() ? 0 : m_presetCombo->findData(presetId);
    m_presetCombo->setCurrentIndex(presetIndex >= 0 ? presetIndex : 0);

    m_modeCombo->setCurrentIndex(options.mode == ThemeMode::Dark ? 1 : 0);

    switch (options.contrast) {
    case ContrastMode::Standard:
        m_contrastCombo->setCurrentIndex(0);
        break;
    case ContrastMode::Medium:
        m_contrastCombo->setCurrentIndex(1);
        break;
    case ContrastMode::High:
        m_contrastCombo->setCurrentIndex(2);
        break;
    }

    m_expressiveCheck->setChecked(options.expressive);
}
