#include <QApplication>
#include <QClipboard>
#include <QColor>
#include <QComboBox>
#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPalette>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QVBoxLayout>
#include <QWidget>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/theme/qtmaterialthemeserializer.h"

using namespace QtMaterial;

namespace {

QColor colorOrFallback(const Theme& theme, ColorRole role, const QColor& fallback)
{
    return theme.colorScheme().contains(role) ? theme.colorScheme().color(role) : fallback;
}

QString cssColor(const QColor& color)
{
    return color.isValid() ? color.name(QColor::HexRgb) : QStringLiteral("#000000");
}

QString contrastName(ContrastMode contrast)
{
    switch (contrast) {
    case ContrastMode::Standard:
        return QStringLiteral("Standard");
    case ContrastMode::Medium:
        return QStringLiteral("Medium");
    case ContrastMode::High:
        return QStringLiteral("High");
    }
    return QStringLiteral("Unknown");
}

ThemeMode modeFromIndex(int index)
{
    return index == 1 ? ThemeMode::Dark : ThemeMode::Light;
}

ContrastMode contrastFromIndex(int index)
{
    switch (index) {
    case 1:
        return ContrastMode::Medium;
    case 2:
        return ContrastMode::High;
    default:
        return ContrastMode::Standard;
    }
}

QFrame* makeSwatch(const QString& title, const QColor& color, const QColor& onColor)
{
    auto* frame = new QFrame;
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setMinimumHeight(72);
    frame->setStyleSheet(QStringLiteral(
        "QFrame { background:%1; color:%2; border-radius: 10px; border: 1px solid rgba(0,0,0,40); }"
        "QLabel { background: transparent; color:%2; }")
        .arg(cssColor(color), cssColor(onColor)));

    auto* layout = new QVBoxLayout(frame);
    auto* label = new QLabel(title);
    auto* value = new QLabel(cssColor(color).toUpper());
    layout->addWidget(label);
    layout->addWidget(value);
    return frame;
}

class SeedWorkflowWindow final : public QWidget {
    Q_OBJECT
public:
    SeedWorkflowWindow()
    {
        setWindowTitle(QStringLiteral("Material 3 seed workflow"));

        m_seedEdit = new QLineEdit(QStringLiteral("#6750A4"));
        m_seedEdit->setPlaceholderText(QStringLiteral("#RRGGBB"));

        m_modeCombo = new QComboBox;
        m_modeCombo->addItem(QStringLiteral("Light"));
        m_modeCombo->addItem(QStringLiteral("Dark"));

        m_contrastCombo = new QComboBox;
        m_contrastCombo->addItem(QStringLiteral("Standard"));
        m_contrastCombo->addItem(QStringLiteral("Medium"));
        m_contrastCombo->addItem(QStringLiteral("High"));

        auto* applyButton = new QPushButton(QStringLiteral("Apply theme"));
        auto* copyButton = new QPushButton(QStringLiteral("Copy JSON"));

        auto* controls = new QFormLayout;
        controls->addRow(QStringLiteral("Seed color"), m_seedEdit);
        controls->addRow(QStringLiteral("Mode"), m_modeCombo);
        controls->addRow(QStringLiteral("Contrast"), m_contrastCombo);

        auto* buttons = new QHBoxLayout;
        buttons->addWidget(applyButton);
        buttons->addWidget(copyButton);
        buttons->addStretch(1);

        m_status = new QLabel;
        m_swatchGrid = new QGridLayout;
        m_jsonPreview = new QPlainTextEdit;
        m_jsonPreview->setReadOnly(true);
        m_jsonPreview->setMinimumHeight(260);

        auto* layout = new QVBoxLayout(this);
        layout->addLayout(controls);
        layout->addLayout(buttons);
        layout->addWidget(m_status);
        layout->addLayout(m_swatchGrid);
        layout->addWidget(new QLabel(QStringLiteral("Resolved theme JSON")));
        layout->addWidget(m_jsonPreview);

        connect(applyButton, &QPushButton::clicked, this, &SeedWorkflowWindow::applyFromControls);
        connect(copyButton, &QPushButton::clicked, this, [this] {
            QApplication::clipboard()->setText(m_jsonPreview->toPlainText());
        });
        connect(&ThemeManager::instance(), &ThemeManager::themeChanged,
                this, &SeedWorkflowWindow::renderTheme);

        applyFromControls();
    }

private slots:
    void applyFromControls()
    {
        const QColor seed(m_seedEdit->text().trimmed());
        if (!seed.isValid()) {
            m_status->setText(QStringLiteral("Invalid seed color. Use #RRGGBB."));
            return;
        }

        ThemeOptions options;
        options.sourceColor = seed;
        options.mode = modeFromIndex(m_modeCombo->currentIndex());
        options.contrast = contrastFromIndex(m_contrastCombo->currentIndex());
        ThemeManager::instance().setThemeOptions(options);
    }

    void renderTheme(const Theme& theme)
    {
        const QColor surface = colorOrFallback(theme, ColorRole::Surface, palette().window().color());
        const QColor onSurface = colorOrFallback(theme, ColorRole::OnSurface, palette().windowText().color());
        const QColor primary = colorOrFallback(theme, ColorRole::Primary, QColor(QStringLiteral("#6750A4")));
        const QColor onPrimary = colorOrFallback(theme, ColorRole::OnPrimary, QColor(Qt::white));

        QPalette nextPalette = palette();
        nextPalette.setColor(QPalette::Window, surface);
        nextPalette.setColor(QPalette::WindowText, onSurface);
        nextPalette.setColor(QPalette::Base, colorOrFallback(theme, ColorRole::SurfaceContainerLowest, surface));
        nextPalette.setColor(QPalette::Text, onSurface);
        nextPalette.setColor(QPalette::Button, primary);
        nextPalette.setColor(QPalette::ButtonText, onPrimary);
        setPalette(nextPalette);
        setAutoFillBackground(true);

        clearSwatches();
        addSwatch(0, 0, QStringLiteral("Primary"), ColorRole::Primary, ColorRole::OnPrimary);
        addSwatch(0, 1, QStringLiteral("Secondary"), ColorRole::Secondary, ColorRole::OnSecondary);
        addSwatch(0, 2, QStringLiteral("Tertiary"), ColorRole::Tertiary, ColorRole::OnTertiary);
        addSwatch(1, 0, QStringLiteral("Surface"), ColorRole::Surface, ColorRole::OnSurface);
        addSwatch(1, 1, QStringLiteral("Primary container"), ColorRole::PrimaryContainer, ColorRole::OnPrimaryContainer);
        addSwatch(1, 2, QStringLiteral("Error"), ColorRole::Error, ColorRole::OnError);

        m_jsonPreview->setPlainText(QString::fromUtf8(ThemeSerializer::toJson(theme)));
        m_status->setText(QStringLiteral("Applied %1 theme, %2 contrast, seed %3")
            .arg(theme.isDark() ? QStringLiteral("dark") : QStringLiteral("light"),
                 contrastName(theme.contrastMode()),
                 theme.options().sourceColor.name(QColor::HexRgb).toUpper()));
    }

private:
    void clearSwatches()
    {
        while (QLayoutItem* item = m_swatchGrid->takeAt(0)) {
            if (QWidget* widget = item->widget()) {
                widget->deleteLater();
            }
            delete item;
        }
    }

    void addSwatch(int row, int column, const QString& title, ColorRole role, ColorRole onRole)
    {
        const Theme& theme = ThemeManager::instance().theme();
        m_swatchGrid->addWidget(makeSwatch(title,
                                           colorOrFallback(theme, role, QColor(Qt::gray)),
                                           colorOrFallback(theme, onRole, QColor(Qt::black))),
                                 row,
                                 column);
    }

    QLineEdit* m_seedEdit = nullptr;
    QComboBox* m_modeCombo = nullptr;
    QComboBox* m_contrastCombo = nullptr;
    QLabel* m_status = nullptr;
    QGridLayout* m_swatchGrid = nullptr;
    QPlainTextEdit* m_jsonPreview = nullptr;
};

} // namespace

#include "main.moc"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    SeedWorkflowWindow window;
    window.resize(900, 720);
    window.show();
    return app.exec();
}
