#include "themepreviewpane.h"

#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>

#include "qtmaterial/theme/qtmaterialcolortoken.h"

using namespace QtMaterial;

ThemePreviewPane::ThemePreviewPane(QWidget* parent)
    : QWidget(parent)
    , m_tabs(new QTabWidget(this))
{
    m_overviewPage = createOverviewPage();
    m_tabs->addTab(m_overviewPage, tr("Overview"));

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->addWidget(m_tabs);
}

QWidget* ThemePreviewPane::createOverviewPage()
{
    auto* page = new QWidget(this);
    auto* root = new QVBoxLayout(page);
    root->setContentsMargins(24, 24, 24, 24);
    root->setSpacing(16);

    m_titleLabel = new QLabel(tr("Material 3 Theme Studio"), page);
    m_supportingLabel = new QLabel(
        tr("Preview of the current generated theme across a few core roles and actions."),
        page);
    m_supportingLabel->setWordWrap(true);

    auto* chipGrid = new QGridLayout();
    m_primaryChip = new QFrame(page);
    m_secondaryChip = new QFrame(page);
    m_surfaceChip = new QFrame(page);
    m_errorChip = new QFrame(page);

    for (QFrame* chip : {m_primaryChip, m_secondaryChip, m_surfaceChip, m_errorChip}) {
        chip->setFixedSize(64, 32);
    }

    chipGrid->addWidget(new QLabel(tr("Primary"), page), 0, 0);
    chipGrid->addWidget(m_primaryChip, 0, 1);
    chipGrid->addWidget(new QLabel(tr("Secondary"), page), 1, 0);
    chipGrid->addWidget(m_secondaryChip, 1, 1);
    chipGrid->addWidget(new QLabel(tr("Surface"), page), 2, 0);
    chipGrid->addWidget(m_surfaceChip, 2, 1);
    chipGrid->addWidget(new QLabel(tr("Error"), page), 3, 0);
    chipGrid->addWidget(m_errorChip, 3, 1);

    auto* actions = new QHBoxLayout();
    m_textButton = new QPushButton(tr("Text"), page);
    m_tonalButton = new QPushButton(tr("Tonal"), page);
    m_filledButton = new QPushButton(tr("Filled"), page);
    actions->addWidget(m_textButton);
    actions->addStretch(1);
    actions->addWidget(m_tonalButton);
    actions->addWidget(m_filledButton);

    root->addWidget(m_titleLabel);
    root->addWidget(m_supportingLabel);
    root->addLayout(chipGrid);
    root->addStretch(1);
    root->addLayout(actions);

    return page;
}

void ThemePreviewPane::applyTheme(const Theme& theme)
{
    const auto& scheme = theme.colorScheme();

    const QColor primary = scheme.color(ColorRole::Primary);
    const QColor onPrimary = scheme.color(ColorRole::OnPrimary);
    const QColor secondaryContainer = scheme.color(ColorRole::SecondaryContainer);
    const QColor onSecondaryContainer = scheme.color(ColorRole::OnSecondaryContainer);
    const QColor surface = scheme.color(ColorRole::Surface);
    const QColor onSurface = scheme.color(ColorRole::OnSurface);
    const QColor surfaceContainer = scheme.color(ColorRole::SurfaceContainerHigh);
    const QColor outline = scheme.color(ColorRole::OutlineVariant);
    const QColor error = scheme.color(ColorRole::Error);

    setColorChip(m_primaryChip, primary, outline);
    setColorChip(m_secondaryChip, secondaryContainer, outline);
    setColorChip(m_surfaceChip, surfaceContainer, outline);
    setColorChip(m_errorChip, error, outline);

    setStyleSheet(QStringLiteral(
        "ThemePreviewPane, QWidget {"
        " background-color: %1;"
        " color: %2;"
        "}"
        "QPushButton {"
        " border-radius: 20px;"
        " min-height: 40px;"
        " padding: 8px 16px;"
        "}"
    ).arg(surface.name(), onSurface.name()));

    m_filledButton->setStyleSheet(QStringLiteral(
        "QPushButton { background-color: %1; color: %2; border: none; }"
    ).arg(primary.name(), onPrimary.name()));

    m_tonalButton->setStyleSheet(QStringLiteral(
        "QPushButton { background-color: %1; color: %2; border: none; }"
    ).arg(secondaryContainer.name(), onSecondaryContainer.name()));

    m_textButton->setStyleSheet(QStringLiteral(
        "QPushButton { background-color: transparent; color: %1; border: none; }"
    ).arg(primary.name()));
}

void ThemePreviewPane::setColorChip(QFrame* chip, const QColor& fill, const QColor& border)
{
    chip->setStyleSheet(QStringLiteral(
        "QFrame { background-color: %1; border: 1px solid %2; border-radius: 8px; }"
    ).arg(fill.name(), border.name()));
}
