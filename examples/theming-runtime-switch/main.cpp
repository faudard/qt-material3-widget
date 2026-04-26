#include <QApplication>
#include <QColor>
#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "qtmaterial/theme/qtmaterialthememanager.h"

using namespace QtMaterial;

namespace {

QColor themeColor(const Theme& theme, ColorRole role, const QColor& fallback)
{
    return theme.colorScheme().contains(role) ? theme.colorScheme().color(role) : fallback;
}

class RuntimeSwitchWindow final : public QWidget {
    Q_OBJECT
public:
    RuntimeSwitchWindow()
    {
        setWindowTitle(QStringLiteral("Runtime theme switch"));

        auto* lightButton = new QPushButton(QStringLiteral("Light"));
        auto* darkButton = new QPushButton(QStringLiteral("Dark"));
        auto* purpleButton = new QPushButton(QStringLiteral("Purple seed"));
        auto* blueButton = new QPushButton(QStringLiteral("Blue seed"));
        auto* highContrastButton = new QPushButton(QStringLiteral("High contrast"));

        m_revision = new QLabel;
        m_summary = new QLabel;
        m_summary->setWordWrap(true);

        auto* row = new QHBoxLayout;
        row->addWidget(lightButton);
        row->addWidget(darkButton);
        row->addWidget(purpleButton);
        row->addWidget(blueButton);
        row->addWidget(highContrastButton);
        row->addStretch(1);

        auto* layout = new QVBoxLayout(this);
        layout->addLayout(row);
        layout->addWidget(m_revision);
        layout->addWidget(m_summary);
        layout->addStretch(1);

        connect(lightButton, &QPushButton::clicked, this, [this] { setMode(ThemeMode::Light); });
        connect(darkButton, &QPushButton::clicked, this, [this] { setMode(ThemeMode::Dark); });
        connect(purpleButton, &QPushButton::clicked, this, [this] { setSeed(QColor(QStringLiteral("#6750A4"))); });
        connect(blueButton, &QPushButton::clicked, this, [this] { setSeed(QColor(QStringLiteral("#00639B"))); });
        connect(highContrastButton, &QPushButton::clicked, this, [this] {
            ThemeOptions options = ThemeManager::instance().options();
            options.contrast = ContrastMode::High;
            ThemeManager::instance().setThemeOptions(options);
        });

        connect(&ThemeManager::instance(), &ThemeManager::themeChangedWithReason,
                this, &RuntimeSwitchWindow::onThemeChanged);

        ThemeOptions options;
        options.sourceColor = QColor(QStringLiteral("#6750A4"));
        ThemeManager::instance().setThemeOptions(options);
        render(ThemeManager::instance().theme());
    }

private slots:
    void onThemeChanged(const Theme& theme, ThemeChangeReason)
    {
        render(theme);
    }

private:
    void setMode(ThemeMode mode)
    {
        ThemeOptions options = ThemeManager::instance().options();
        options.mode = mode;
        ThemeManager::instance().setThemeOptions(options);
    }

    void setSeed(const QColor& seed)
    {
        ThemeManager::instance().applySeedColor(seed);
    }

    void render(const Theme& theme)
    {
        const QColor surface = themeColor(theme, ColorRole::Surface, palette().window().color());
        const QColor onSurface = themeColor(theme, ColorRole::OnSurface, palette().windowText().color());
        const QColor primary = themeColor(theme, ColorRole::Primary, QColor(QStringLiteral("#6750A4")));
        const QColor onPrimary = themeColor(theme, ColorRole::OnPrimary, QColor(Qt::white));

        QPalette next = palette();
        next.setColor(QPalette::Window, surface);
        next.setColor(QPalette::WindowText, onSurface);
        next.setColor(QPalette::Button, primary);
        next.setColor(QPalette::ButtonText, onPrimary);
        setPalette(next);
        setAutoFillBackground(true);

        m_revision->setText(QStringLiteral("Theme revision: %1").arg(ThemeManager::instance().revision()));
        m_summary->setText(QStringLiteral("Seed %1, mode %2, primary %3, surface %4")
            .arg(theme.options().sourceColor.name(QColor::HexRgb).toUpper(),
                 theme.isDark() ? QStringLiteral("dark") : QStringLiteral("light"),
                 primary.name(QColor::HexRgb).toUpper(),
                 surface.name(QColor::HexRgb).toUpper()));
    }

    QLabel* m_revision = nullptr;
    QLabel* m_summary = nullptr;
};

} // namespace

#include "main.moc"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    RuntimeSwitchWindow window;
    window.resize(720, 360);
    window.show();
    return app.exec();
}
