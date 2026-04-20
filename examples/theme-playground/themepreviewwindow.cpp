#include "themepreviewwindow.h"

#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "qtmaterial/theme/qtmaterialthememanager.h"

using namespace QtMaterial;

ThemePreviewWindow::ThemePreviewWindow(QWidget* parent)
    : QWidget(parent)
    , m_modeLabel(new QLabel(this))
    , m_card(new QFrame(this))
    , m_eyebrowLabel(new QLabel(tr("Design system preview"), this))
    , m_titleLabel(new QLabel(tr("Material 3 card"), this))
    , m_supportingLabel(new QLabel(
          tr("This preview shows headline, supporting text, actions, and core color roles. "
             "Press the main action to switch between light and dark mode."), this))
    , m_paletteLabel(new QLabel(this))
    , m_primaryChip(new QFrame(this))
    , m_secondaryChip(new QFrame(this))
    , m_surfaceChip(new QFrame(this))
    , m_errorChip(new QFrame(this))
    , m_toggleButton(new QPushButton(tr("Toggle theme"), this))
    , m_confirmButton(new QPushButton(tr("Confirm"), this))
    , m_dismissButton(new QPushButton(tr("Dismiss"), this))
{
    setObjectName(QStringLiteral("themePreviewWindow"));
    m_card->setObjectName(QStringLiteral("previewCard"));

    m_modeLabel->setObjectName(QStringLiteral("modeLabel"));
    m_eyebrowLabel->setObjectName(QStringLiteral("eyebrowLabel"));
    m_eyebrowLabel->setText(tr("Design system preview").toUpper());
    m_titleLabel->setObjectName(QStringLiteral("titleLabel"));
    m_supportingLabel->setObjectName(QStringLiteral("supportingLabel"));
    m_paletteLabel->setObjectName(QStringLiteral("paletteLabel"));

    m_toggleButton->setObjectName(QStringLiteral("filledButton"));
    m_confirmButton->setObjectName(QStringLiteral("tonalButton"));
    m_dismissButton->setObjectName(QStringLiteral("textButton"));

    m_titleLabel->setWordWrap(true);
    m_supportingLabel->setWordWrap(true);
    m_paletteLabel->setWordWrap(true);

    for (QFrame* chip : {m_primaryChip, m_secondaryChip, m_surfaceChip, m_errorChip}) {
        chip->setFixedSize(56, 32);
    }

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(24, 24, 24, 24);
    root->setSpacing(18);

    root->addWidget(m_modeLabel);

    auto* cardLayout = new QVBoxLayout(m_card);
    cardLayout->setContentsMargins(24, 24, 24, 24);
    cardLayout->setSpacing(16);

    cardLayout->addWidget(m_eyebrowLabel);
    cardLayout->addWidget(m_titleLabel);
    cardLayout->addWidget(m_supportingLabel);

    auto* chipGrid = new QGridLayout();
    chipGrid->setHorizontalSpacing(12);
    chipGrid->setVerticalSpacing(10);

    chipGrid->addWidget(new QLabel(tr("Primary"), this),   0, 0);
    chipGrid->addWidget(m_primaryChip,                     0, 1);
    chipGrid->addWidget(new QLabel(tr("Secondary"), this), 1, 0);
    chipGrid->addWidget(m_secondaryChip,                   1, 1);
    chipGrid->addWidget(new QLabel(tr("Surface"), this),   2, 0);
    chipGrid->addWidget(m_surfaceChip,                     2, 1);
    chipGrid->addWidget(new QLabel(tr("Error"), this),     3, 0);
    chipGrid->addWidget(m_errorChip,                       3, 1);

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
    root->addStretch(1);

    resize(620, 420);

    connect(m_toggleButton, &QPushButton::clicked,
            this, &ThemePreviewWindow::toggleMode);

    connect(&ThemeManager::instance(),
            &ThemeManager::themeChanged,
            this,
            [this](const Theme&) { refreshPreview(); });

    refreshPreview();
}

void ThemePreviewWindow::toggleMode()
{
    ThemeOptions options = ThemeManager::instance().options();
    options.mode = (options.mode == ThemeMode::Dark)
                       ? ThemeMode::Light
                       : ThemeMode::Dark;

    ThemeManager::instance().setThemeOptions(options);
}

void ThemePreviewWindow::setColorChip(QFrame* chip, const QColor& fill, const QColor& border)
{
    chip->setStyleSheet(QStringLiteral(
                            "QFrame {"
                            "  background-color: %1;"
                            "  border: 1px solid %2;"
                            "  border-radius: 8px;"
                            "}").arg(fill.name(), border.name()));
}

void ThemePreviewWindow::refreshPreview()
{
    const auto& theme = ThemeManager::instance().theme();
    const auto& scheme = theme.colorScheme();

    const bool dark = ThemeManager::instance().options().mode == ThemeMode::Dark;

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
    const QColor onError = scheme.color(ColorRole::OnError);

    m_modeLabel->setText(dark ? tr("Current mode: Dark") : tr("Current mode: Light"));

    m_paletteLabel->setText(QStringLiteral(
                                "Primary: %1\n"
                                "Secondary: %2\n"
                                "Surface: %3\n"
                                "Error: %4")
                                .arg(primary.name(),
                                     secondary.name(),
                                     surface.name(),
                                     error.name()));

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
                      "  background-color: %1;"
                      "}"

                      "#previewCard {"
                      "  background-color: %2;"
                      "  border: 1px solid %3;"
                      "  border-radius: 28px;"
                      "}"

                      "QLabel {"
                      "  color: %4;"
                      "  background-color: transparent;"
                      "}"

                      "#modeLabel {"
                      "  color: %5;"
                      "  font-size: 12px;"
                      "  font-weight: bold;"
                      "}"

                      "#eyebrowLabel {"
                      "  color: %5;"
                      "  font-size: 12px;"
                      "  font-weight: bold;"
                      "}"

                      "#titleLabel {"
                      "  color: %4;"
                      "  font-size: 24px;"
                      "  font-weight: bold;"
                      "}"

                      "#supportingLabel {"
                      "  color: %4;"
                      "  font-size: 14px;"
                      "}"

                      "#paletteLabel {"
                      "  color: %5;"
                      "  font-size: 12px;"
                      "}"

                      "#filledButton {"
                      "  background-color: %6;"
                      "  color: %7;"
                      "  border: none;"
                      "  border-radius: 20px;"
                      "  padding: 10px 18px;"
                      "  min-height: 40px;"
                      "  font-weight: bold;"
                      "}"
                      "#filledButton:hover {"
                      "  background-color: %8;"
                      "}"
                      "#filledButton:pressed {"
                      "  background-color: %9;"
                      "}"

                      "#tonalButton {"
                      "  background-color: %10;"
                      "  color: %11;"
                      "  border: none;"
                      "  border-radius: 20px;"
                      "  padding: 10px 18px;"
                      "  min-height: 40px;"
                      "  font-weight: bold;"
                      "}"
                      "#tonalButton:hover {"
                      "  background-color: %12;"
                      "}"
                      "#tonalButton:pressed {"
                      "  background-color: %13;"
                      "}"

                      "#textButton {"
                      "  background-color: transparent;"
                      "  color: %6;"
                      "  border: none;"
                      "  border-radius: 20px;"
                      "  padding: 10px 14px;"
                      "  min-height: 40px;"
                      "  font-weight: bold;"
                      "}"
                      "#textButton:hover {"
                      "  background-color: %14;"
                      "}"
                      "#textButton:pressed {"
                      "  background-color: %15;"
                      "}").arg(
                          surface.name(),
                          surfaceContainerHigh.name(),
                          outlineVariant.name(),
                          onSurface.name(),
                          outline.name(),
                          primary.name(),
                          onPrimary.name(),
                          filledHover.name(),
                          filledPressed.name(),
                          secondaryContainer.name(),
                          onSecondaryContainer.name(),
                          tonalHover.name(),
                          tonalPressed.name(),
                          textHover.name(),
                          textPressed.name()
                          ));

    m_confirmButton->setText(tr("Confirm"));
    m_dismissButton->setText(tr("Dismiss"));
}