#include "themepreviewwindow.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"

namespace {
QString roleName(QtMaterial::ColorRole role) {
    switch (role) {
    case QtMaterial::ColorRole::Primary: return "Primary";
    case QtMaterial::ColorRole::OnPrimary: return "OnPrimary";
    case QtMaterial::ColorRole::PrimaryContainer: return "PrimaryContainer";
    case QtMaterial::ColorRole::OnPrimaryContainer: return "OnPrimaryContainer";
    case QtMaterial::ColorRole::Secondary: return "Secondary";
    case QtMaterial::ColorRole::OnSecondary: return "OnSecondary";
    case QtMaterial::ColorRole::SecondaryContainer: return "SecondaryContainer";
    case QtMaterial::ColorRole::OnSecondaryContainer: return "OnSecondaryContainer";
    case QtMaterial::ColorRole::Surface: return "Surface";
    case QtMaterial::ColorRole::OnSurface: return "OnSurface";
    case QtMaterial::ColorRole::SurfaceVariant: return "SurfaceVariant";
    case QtMaterial::ColorRole::OnSurfaceVariant: return "OnSurfaceVariant";
    case QtMaterial::ColorRole::Outline: return "Outline";
    case QtMaterial::ColorRole::OutlineVariant: return "OutlineVariant";
    default: return "Other";
    }
}
} // namespace

ThemePreviewWindow::ThemePreviewWindow(QWidget* parent)
    : QWidget(parent)
    , m_modeCombo(new QComboBox(this))
    , m_applyButton(new QPushButton(tr("Apply"), this))
    , m_rolesList(new QListWidget(this))
    , m_titleLabel(new QLabel(tr("qt-material3-widgets theme playground"), this))
{
    m_modeCombo->addItem(tr("Light"));
    m_modeCombo->addItem(tr("Dark"));

    auto* controlsLayout = new QHBoxLayout;
    controlsLayout->addWidget(new QLabel(tr("Theme mode:"), this));
    controlsLayout->addWidget(m_modeCombo);
    controlsLayout->addWidget(m_applyButton);
    controlsLayout->addStretch();

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(m_titleLabel);
    layout->addLayout(controlsLayout);
    layout->addWidget(m_rolesList, 1);

    connect(m_applyButton, &QPushButton::clicked, this, &ThemePreviewWindow::rebuildTheme);
    rebuildTheme();
}

void ThemePreviewWindow::rebuildTheme() {
    QtMaterial::ThemeOptions options = QtMaterial::ThemeManager::instance().options();
    options.mode = (m_modeCombo->currentIndex() == 1) ? QtMaterial::ThemeMode::Dark : QtMaterial::ThemeMode::Light;
    QtMaterial::ThemeManager::instance().setThemeOptions(options);
    populateRoles();
}

void ThemePreviewWindow::populateRoles() {
    m_rolesList->clear();
    const QtMaterial::Theme& theme = QtMaterial::ThemeManager::instance().theme();
    const QtMaterial::ColorScheme& scheme = theme.colorScheme();
    const QtMaterial::ColorRole roles[] = {
        QtMaterial::ColorRole::Primary,
        QtMaterial::ColorRole::OnPrimary,
        QtMaterial::ColorRole::PrimaryContainer,
        QtMaterial::ColorRole::OnPrimaryContainer,
        QtMaterial::ColorRole::Secondary,
        QtMaterial::ColorRole::OnSecondary,
        QtMaterial::ColorRole::SecondaryContainer,
        QtMaterial::ColorRole::OnSecondaryContainer,
        QtMaterial::ColorRole::Surface,
        QtMaterial::ColorRole::OnSurface,
        QtMaterial::ColorRole::SurfaceVariant,
        QtMaterial::ColorRole::OnSurfaceVariant,
        QtMaterial::ColorRole::Outline,
        QtMaterial::ColorRole::OutlineVariant
    };

    for (QtMaterial::ColorRole role : roles) {
        const QColor color = scheme.color(role);
        const QString line = QStringLiteral("%1  ->  %2").arg(roleName(role)).arg(color.name(QColor::HexArgb));
        m_rolesList->addItem(line);
    }
}
