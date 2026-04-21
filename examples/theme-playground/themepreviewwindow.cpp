#include "themepreviewwindow.h"

#include <QColor>
#include <QColorDialog>
#include <QFileDialog>
#include <QFrame>
#include <QGridLayout>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStyle>
#include <QVBoxLayout>
#include <QClipboard>

#include "qtmaterial/theme/qtmaterialthememanager.h"

using namespace QtMaterial;

ThemePreviewWindow::ThemePreviewWindow(QWidget* parent)
    : QWidget(parent)
    , m_modeLabel(new QLabel(this))
    , m_seedLabel(new QLabel(this))
    , m_card(new QFrame(this))
    , m_eyebrowLabel(new QLabel(tr("Design system preview"), this))
    , m_titleLabel(new QLabel(tr("Material 3 theme playground"), this))
    , m_supportingLabel(new QLabel(
          tr("This playground previews core roles, lets you switch mode, apply a seed color, "
             "and import or export a full serialized theme."),
          this))
    , m_paletteLabel(new QLabel(this))
    , m_primaryChip(new QFrame(this))
    , m_secondaryChip(new QFrame(this))
    , m_surfaceChip(new QFrame(this))
    , m_errorChip(new QFrame(this))
    , m_toggleButton(new QPushButton(tr("Toggle theme"), this))
    , m_seedButton(new QPushButton(tr("Choose seed"), this))
    , m_exportButton(new QPushButton(tr("Export JSON…"), this))
    , m_importButton(new QPushButton(tr("Import JSON…"), this))
    , m_copyJsonButton(new QPushButton(tr("Copy JSON"), this))
    , m_confirmButton(new QPushButton(tr("Confirm"), this))
    , m_dismissButton(new QPushButton(tr("Dismiss"), this))
    , m_jsonPreview(new QPlainTextEdit(this))
{
    setObjectName(QStringLiteral("themePreviewWindow"));
    m_card->setObjectName(QStringLiteral("previewCard"));
    m_modeLabel->setObjectName(QStringLiteral("modeLabel"));
    m_seedLabel->setObjectName(QStringLiteral("seedLabel"));
    m_eyebrowLabel->setObjectName(QStringLiteral("eyebrowLabel"));
    m_titleLabel->setObjectName(QStringLiteral("titleLabel"));
    m_supportingLabel->setObjectName(QStringLiteral("supportingLabel"));
    m_paletteLabel->setObjectName(QStringLiteral("paletteLabel"));
    m_jsonPreview->setObjectName(QStringLiteral("jsonPreview"));

    m_toggleButton->setObjectName(QStringLiteral("filledButton"));
    m_seedButton->setObjectName(QStringLiteral("outlinedButton"));
    m_exportButton->setObjectName(QStringLiteral("outlinedButton"));
    m_importButton->setObjectName(QStringLiteral("outlinedButton"));
    m_copyJsonButton->setObjectName(QStringLiteral("textButton"));
    m_confirmButton->setObjectName(QStringLiteral("tonalButton"));
    m_dismissButton->setObjectName(QStringLiteral("textButton"));

    m_eyebrowLabel->setText(tr("Design system preview").toUpper());
    m_titleLabel->setWordWrap(true);
    m_supportingLabel->setWordWrap(true);
    m_paletteLabel->setWordWrap(true);

    m_jsonPreview->setReadOnly(true);
    m_jsonPreview->setMinimumHeight(220);
    m_jsonPreview->setLineWrapMode(QPlainTextEdit::NoWrap);

    for (QFrame* chip : {m_primaryChip, m_secondaryChip, m_surfaceChip, m_errorChip}) {
        chip->setFixedSize(64, 36);
    }

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(24, 24, 24, 24);
    root->setSpacing(18);

    root->addWidget(m_modeLabel);
    root->addWidget(m_seedLabel);

    auto* cardLayout = new QVBoxLayout(m_card);
    cardLayout->setContentsMargins(24, 24, 24, 24);
    cardLayout->setSpacing(16);
    cardLayout->addWidget(m_eyebrowLabel);
    cardLayout->addWidget(m_titleLabel);
    cardLayout->addWidget(m_supportingLabel);

    auto* chipGrid = new QGridLayout();
    chipGrid->setHorizontalSpacing(12);
    chipGrid->setVerticalSpacing(10);
    chipGrid->addWidget(new QLabel(tr("Primary"), this), 0, 0);
    chipGrid->addWidget(m_primaryChip, 0, 1);
    chipGrid->addWidget(new QLabel(tr("Secondary"), this), 1, 0);
    chipGrid->addWidget(m_secondaryChip, 1, 1);
    chipGrid->addWidget(new QLabel(tr("Surface"), this), 2, 0);
    chipGrid->addWidget(m_surfaceChip, 2, 1);
    chipGrid->addWidget(new QLabel(tr("Error"), this), 3, 0);
    chipGrid->addWidget(m_errorChip, 3, 1);
    chipGrid->setColumnStretch(2, 1);
    cardLayout->addLayout(chipGrid);
    cardLayout->addWidget(m_paletteLabel);

    auto* actionLayout = new QHBoxLayout();
    actionLayout->setSpacing(12);
    actionLayout->addWidget(m_dismissButton, 0);
    actionLayout->addStretch(1);
    actionLayout->addWidget(m_confirmButton, 0);
    actionLayout->addWidget(m_toggleButton, 0);
    cardLayout->addLayout(actionLayout);

    root->addWidget(m_card);

    auto* toolsLayout = new QHBoxLayout();
    toolsLayout->setSpacing(12);
    toolsLayout->addWidget(m_seedButton);
    toolsLayout->addWidget(m_importButton);
    toolsLayout->addWidget(m_exportButton);
    toolsLayout->addStretch(1);
    toolsLayout->addWidget(m_copyJsonButton);
    root->addLayout(toolsLayout);

    root->addWidget(m_jsonPreview, 1);

    connect(m_toggleButton, &QPushButton::clicked, this, &ThemePreviewWindow::toggleMode);
    connect(m_seedButton, &QPushButton::clicked, this, &ThemePreviewWindow::chooseSeedColor);
    connect(m_exportButton, &QPushButton::clicked, this, &ThemePreviewWindow::exportTheme);
    connect(m_importButton, &QPushButton::clicked, this, &ThemePreviewWindow::importTheme);
    connect(m_copyJsonButton, &QPushButton::clicked, this, &ThemePreviewWindow::copyThemeJson);
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, [this](const Theme&) {
        refreshPreview();
    });

    refreshPreview();
}

void ThemePreviewWindow::toggleMode()
{
    ThemeOptions options = ThemeManager::instance().options();
    options.mode = (options.mode == ThemeMode::Dark) ? ThemeMode::Light : ThemeMode::Dark;
    ThemeManager::instance().setThemeOptions(options);
}

void ThemePreviewWindow::chooseSeedColor()
{
    const QColor current = ThemeManager::instance().options().sourceColor;
    const QColor chosen = QColorDialog::getColor(current, this, tr("Choose seed color"));
    if (!chosen.isValid()) {
        return;
    }

    ThemeManager::instance().applySeedColor(chosen);
}

void ThemePreviewWindow::exportTheme()
{
    const QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Export theme JSON"),
        QStringLiteral("theme.json"),
        tr("JSON files (*.json);;All files (*)"));

    if (filePath.isEmpty()) {
        return;
    }

    QString error;
    if (!ThemeManager::instance().exportThemeToFile(filePath, &error)) {
        QMessageBox::critical(this, tr("Export failed"), error);
        return;
    }

    QMessageBox::information(this, tr("Export complete"), tr("Theme exported to %1").arg(filePath));
}

void ThemePreviewWindow::importTheme()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Import theme JSON"),
        QString(),
        tr("JSON files (*.json);;All files (*)"));

    if (filePath.isEmpty()) {
        return;
    }

    QString error;
    if (!ThemeManager::instance().importThemeFromFile(filePath, &error)) {
        QMessageBox::critical(this, tr("Import failed"), error);
        return;
    }

    QMessageBox::information(this, tr("Import complete"), tr("Theme imported from %1").arg(filePath));
}

void ThemePreviewWindow::copyThemeJson()
{
    if (auto* clipboard = QGuiApplication::clipboard()) {
        clipboard->setText(m_jsonPreview->toPlainText());
    }
}

void ThemePreviewWindow::setColorChip(QFrame* chip, const QColor& fill, const QColor& border)
{
    chip->setStyleSheet(QStringLiteral(
                           "QFrame {"
                           " background-color: %1;"
                           " border: 1px solid %2;"
                           " border-radius: 8px;"
                           "}")
                           .arg(fill.name(QColor::HexArgb), border.name(QColor::HexArgb)));
}

void ThemePreviewWindow::refreshPreview()
{
    const ThemeManager& manager = ThemeManager::instance();
    const Theme& theme = manager.theme();
    const ColorScheme& scheme = theme.colorScheme();

    const bool dark = theme.mode() == ThemeMode::Dark;

    const QColor primary = scheme.color(ColorRole::Primary);
    const QColor onPrimary = scheme.color(ColorRole::OnPrimary);
    const QColor primaryContainer = scheme.color(ColorRole::PrimaryContainer);
    const QColor onPrimaryContainer = scheme.color(ColorRole::OnPrimaryContainer);
    const QColor secondary = scheme.color(ColorRole::Secondary);
    const QColor secondaryContainer = scheme.color(ColorRole::SecondaryContainer);
    const QColor onSecondaryContainer = scheme.color(ColorRole::OnSecondaryContainer);
    const QColor surface = scheme.color(ColorRole::Surface);
    const QColor onSurface = scheme.color(ColorRole::OnSurface);
    const QColor surfaceContainerHigh = scheme.color(ColorRole::SurfaceContainerHigh);
    const QColor surfaceContainerHighest = scheme.color(ColorRole::SurfaceContainerHighest);
    const QColor outline = scheme.color(ColorRole::Outline);
    const QColor outlineVariant = scheme.color(ColorRole::OutlineVariant);
    const QColor error = scheme.color(ColorRole::Error);
    const QColor seed = theme.options().sourceColor;

    m_modeLabel->setText(dark ? tr("Current mode: Dark") : tr("Current mode: Light"));
    m_seedLabel->setText(tr("Seed color: %1").arg(seed.name(QColor::HexArgb)));
    m_paletteLabel->setText(QStringLiteral(
                               "Primary: %1\n"
                               "Secondary: %2\n"
                               "Surface: %3\n"
                               "Error: %4")
                               .arg(primary.name(QColor::HexArgb),
                                    secondary.name(QColor::HexArgb),
                                    surface.name(QColor::HexArgb),
                                    error.name(QColor::HexArgb)));

    setColorChip(m_primaryChip, primary, outlineVariant);
    setColorChip(m_secondaryChip, secondary, outlineVariant);
    setColorChip(m_surfaceChip, surfaceContainerHighest, outlineVariant);
    setColorChip(m_errorChip, error, outlineVariant);

    const QColor filledHover = primary.lighter(dark ? 112 : 106);
    const QColor filledPressed = primary.darker(dark ? 108 : 112);
    const QColor tonalHover = primaryContainer.lighter(dark ? 108 : 104);
    const QColor tonalPressed = primaryContainer.darker(dark ? 106 : 110);
    const QColor textHover = surfaceContainerHighest;
    const QColor textPressed = surfaceContainerHigh;

    setStyleSheet(QStringLiteral(
                      "#themePreviewWindow {"
                      " background-color: %1;"
                      "}"
                      "#previewCard {"
                      " background-color: %2;"
                      " border: 1px solid %3;"
                      " border-radius: 28px;"
                      "}"
                      "QLabel {"
                      " color: %4;"
                      " background-color: transparent;"
                      "}"
                      "#modeLabel, #seedLabel, #eyebrowLabel, #paletteLabel {"
                      " color: %5;"
                      " font-size: 12px;"
                      " font-weight: 600;"
                      "}"
                      "#titleLabel {"
                      " color: %4;"
                      " font-size: 24px;"
                      " font-weight: 700;"
                      "}"
                      "#supportingLabel {"
                      " color: %4;"
                      " font-size: 14px;"
                      "}"
                      "#jsonPreview {"
                      " background-color: %2;"
                      " color: %4;"
                      " border: 1px solid %3;"
                      " border-radius: 16px;"
                      " padding: 12px;"
                      " selection-background-color: %6;"
                      " selection-color: %7;"
                      "}"
                      "#filledButton {"
                      " background-color: %6;"
                      " color: %7;"
                      " border: none;"
                      " border-radius: 20px;"
                      " padding: 10px 18px;"
                      " min-height: 40px;"
                      " font-weight: 700;"
                      "}"
                      "#filledButton:hover { background-color: %8; }"
                      "#filledButton:pressed { background-color: %9; }"
                      "#tonalButton {"
                      " background-color: %10;"
                      " color: %11;"
                      " border: none;"
                      " border-radius: 20px;"
                      " padding: 10px 18px;"
                      " min-height: 40px;"
                      " font-weight: 700;"
                      "}"
                      "#tonalButton:hover { background-color: %12; }"
                      "#tonalButton:pressed { background-color: %13; }"
                      "#textButton {"
                      " background-color: transparent;"
                      " color: %6;"
                      " border: none;"
                      " border-radius: 20px;"
                      " padding: 10px 14px;"
                      " min-height: 40px;"
                      " font-weight: 700;"
                      "}"
                      "#textButton:hover { background-color: %14; }"
                      "#textButton:pressed { background-color: %15; }"
                      "#outlinedButton {"
                      " background-color: transparent;"
                      " color: %6;"
                      " border: 1px solid %3;"
                      " border-radius: 20px;"
                      " padding: 10px 16px;"
                      " min-height: 40px;"
                      " font-weight: 600;"
                      "}"
                      "#outlinedButton:hover { background-color: %14; }"
                      "#outlinedButton:pressed { background-color: %15; }")
                      .arg(surface.name(QColor::HexArgb),
                           surfaceContainerHigh.name(QColor::HexArgb),
                           outlineVariant.name(QColor::HexArgb),
                           onSurface.name(QColor::HexArgb),
                           outline.name(QColor::HexArgb),
                           primary.name(QColor::HexArgb),
                           onPrimary.name(QColor::HexArgb),
                           filledHover.name(QColor::HexArgb),
                           filledPressed.name(QColor::HexArgb),
                           secondaryContainer.name(QColor::HexArgb),
                           onSecondaryContainer.name(QColor::HexArgb),
                           tonalHover.name(QColor::HexArgb),
                           tonalPressed.name(QColor::HexArgb),
                           textHover.name(QColor::HexArgb),
                           textPressed.name(QColor::HexArgb)));

    m_jsonPreview->setPlainText(QString::fromUtf8(
        manager.exportThemeJson(QJsonDocument::Indented)));
}
