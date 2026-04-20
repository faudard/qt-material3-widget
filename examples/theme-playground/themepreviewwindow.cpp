#include "themepreviewwindow.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "qtmaterial/theme/qtmaterialthememanager.h"
ThemePreviewWindow::ThemePreviewWindow(QWidget* parent)
    : QWidget(parent), m_label(new QLabel(this)), m_button(new QPushButton(tr("Toggle theme mode"), this)), m_dark(false)
{
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(m_label);
    layout->addWidget(m_button);
    setLayout(layout);
    connect(m_button, &QPushButton::clicked, this, &ThemePreviewWindow::toggleMode);
    refreshPreview();
}
void ThemePreviewWindow::toggleMode()
{
    QtMaterial::ThemeOptions options = QtMaterial::ThemeManager::instance().options();
    options.mode = m_dark ? QtMaterial::ThemeMode::Light : QtMaterial::ThemeMode::Dark;
    m_dark = !m_dark;
    QtMaterial::ThemeManager::instance().setThemeOptions(options);
    refreshPreview();
}
void ThemePreviewWindow::refreshPreview()
{
    const auto& theme = QtMaterial::ThemeManager::instance().theme();
    m_label->setText(QStringLiteral("Primary: %1\nSurface: %2")
        .arg(theme.colorScheme().color(QtMaterial::ColorRole::Primary).name())
        .arg(theme.colorScheme().color(QtMaterial::ColorRole::Surface).name()));
}
