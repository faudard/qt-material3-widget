#include "themepreviewwindow.h"

#include "qtmaterial/theme/qtmaterialtheme.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/theme/qtmaterialthemeserializer.h"

#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QClipboard>
#include <QColorDialog>
#include <QComboBox>
#include <QFileDialog>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSignalBlocker>
#include <QVariant>
#include <QScrollArea>
#include <QTabWidget>
#include <QTextStream>
#include <QTimer>
#include <QVBoxLayout>

using namespace QtMaterial;

namespace {

QString themeModeToString(ThemeMode mode) {
    return mode == ThemeMode::Dark ? QStringLiteral("Dark") : QStringLiteral("Light");
}

QString contrastToString(ContrastMode contrast) {
    switch (contrast) {
    case ContrastMode::Standard:
        return QStringLiteral("Standard");
    case ContrastMode::Medium:
        return QStringLiteral("Medium");
    case ContrastMode::High:
        return QStringLiteral("High");
    }
    return QStringLiteral("Standard");
}

QColor overlay(const QColor& base, const QColor& layer, qreal opacity) {
    const qreal a = qBound(0.0, opacity, 1.0);
    return QColor(
        static_cast<int>(base.red() * (1.0 - a) + layer.red() * a),
        static_cast<int>(base.green() * (1.0 - a) + layer.green() * a),
        static_cast<int>(base.blue() * (1.0 - a) + layer.blue() * a),
        base.alpha());
}

QLabel* sampleLabel(const QString& text, const QFont& font, const QColor& color, QWidget* parent) {
    auto* label = new QLabel(text, parent);
    label->setFont(font);
    label->setWordWrap(true);
    label->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(color.name(QColor::HexArgb)));
    return label;
}

QString roleName(ColorRole role) {
    switch (role) {
    case ColorRole::Primary: return QStringLiteral("primary");
    case ColorRole::OnPrimary: return QStringLiteral("on-primary");
    case ColorRole::PrimaryContainer: return QStringLiteral("primary-container");
    case ColorRole::OnPrimaryContainer: return QStringLiteral("on-primary-container");
    case ColorRole::Secondary: return QStringLiteral("secondary");
    case ColorRole::OnSecondary: return QStringLiteral("on-secondary");
    case ColorRole::SecondaryContainer: return QStringLiteral("secondary-container");
    case ColorRole::OnSecondaryContainer: return QStringLiteral("on-secondary-container");
    case ColorRole::Tertiary: return QStringLiteral("tertiary");
    case ColorRole::OnTertiary: return QStringLiteral("on-tertiary");
    case ColorRole::TertiaryContainer: return QStringLiteral("tertiary-container");
    case ColorRole::OnTertiaryContainer: return QStringLiteral("on-tertiary-container");
    case ColorRole::Error: return QStringLiteral("error");
    case ColorRole::OnError: return QStringLiteral("on-error");
    case ColorRole::ErrorContainer: return QStringLiteral("error-container");
    case ColorRole::OnErrorContainer: return QStringLiteral("on-error-container");
    case ColorRole::Background: return QStringLiteral("background");
    case ColorRole::OnBackground: return QStringLiteral("on-background");
    case ColorRole::Surface: return QStringLiteral("surface");
    case ColorRole::OnSurface: return QStringLiteral("on-surface");
    case ColorRole::SurfaceDim: return QStringLiteral("surface-dim");
    case ColorRole::SurfaceBright: return QStringLiteral("surface-bright");
    case ColorRole::SurfaceContainerLowest: return QStringLiteral("surface-container-lowest");
    case ColorRole::SurfaceContainerLow: return QStringLiteral("surface-container-low");
    case ColorRole::SurfaceContainer: return QStringLiteral("surface-container");
    case ColorRole::SurfaceContainerHigh: return QStringLiteral("surface-container-high");
    case ColorRole::SurfaceContainerHighest: return QStringLiteral("surface-container-highest");
    case ColorRole::SurfaceVariant: return QStringLiteral("surface-variant");
    case ColorRole::OnSurfaceVariant: return QStringLiteral("on-surface-variant");
    case ColorRole::Outline: return QStringLiteral("outline");
    case ColorRole::OutlineVariant: return QStringLiteral("outline-variant");
    case ColorRole::InverseSurface: return QStringLiteral("inverse-surface");
    case ColorRole::InverseOnSurface: return QStringLiteral("inverse-on-surface");
    case ColorRole::InversePrimary: return QStringLiteral("inverse-primary");
    case ColorRole::Shadow: return QStringLiteral("shadow");
    case ColorRole::Scrim: return QStringLiteral("scrim");
    }
    return QStringLiteral("unknown");
}

} // namespace

ThemePreviewWindow::ThemePreviewWindow(QWidget* parent)
    : QWidget(parent) {
    setObjectName(QStringLiteral("themeStudio"));
    setWindowTitle(tr("Qt Material 3 Theme Studio"));

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(24, 24, 24, 24);
    root->setSpacing(18);

    m_titleLabel = new QLabel(tr("Theme Studio"), this);
    m_titleLabel->setObjectName(QStringLiteral("studioTitle"));
    root->addWidget(m_titleLabel);

    m_statusLabel = new QLabel(this);
    m_statusLabel->setObjectName(QStringLiteral("studioStatus"));
    m_statusLabel->setWordWrap(true);
    root->addWidget(m_statusLabel);

    buildControls(root);
    buildPreviewTabs(root);

    m_jsonPreview = new QPlainTextEdit(this);
    m_jsonPreview->setObjectName(QStringLiteral("jsonPreview"));
    m_jsonPreview->setReadOnly(true);
    m_jsonPreview->setLineWrapMode(QPlainTextEdit::NoWrap);
    m_jsonPreview->setMinimumHeight(180);
    root->addWidget(panel(tr("Resolved JSON"), m_jsonPreview), 1);

    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, [this](const Theme&) {
        refreshPreview();
    });
    connect(&SystemTheme::instance(), &SystemTheme::systemThemeChanged, this, [this]() {
        if (m_followSystemRadio && m_followSystemRadio->isChecked()) {
            refreshPreview();
        }
    });

    refreshPreview();
}

void ThemePreviewWindow::buildControls(QVBoxLayout* root) {
    auto* controls = new QFrame(this);
    controls->setObjectName(QStringLiteral("controlPanel"));
    auto* layout = new QGridLayout(controls);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setHorizontalSpacing(14);
    layout->setVerticalSpacing(12);

    m_seedButton = new QPushButton(tr("Choose seed color…"), controls);
    m_seedButton->setObjectName(QStringLiteral("primaryAction"));

    m_lightRadio = new QRadioButton(tr("Light"), controls);
    m_darkRadio = new QRadioButton(tr("Dark"), controls);
    m_followSystemRadio = new QRadioButton(tr("Follow system"), controls);
    m_followSystemRadio->setChecked(true);

    auto* modeGroup = new QButtonGroup(controls);
    modeGroup->addButton(m_lightRadio);
    modeGroup->addButton(m_darkRadio);
    modeGroup->addButton(m_followSystemRadio);

    m_contrastCombo = new QComboBox(controls);
    m_contrastCombo->addItem(tr("Standard contrast"), QVariant::fromValue(static_cast<int>(ContrastMode::Standard)));
    m_contrastCombo->addItem(tr("Medium contrast"), QVariant::fromValue(static_cast<int>(ContrastMode::Medium)));
    m_contrastCombo->addItem(tr("High contrast"), QVariant::fromValue(static_cast<int>(ContrastMode::High)));

    m_platformFontCheck = new QCheckBox(tr("Use platform font"), controls);
    m_platformFontCheck->setChecked(SystemTheme::instance().usePlatformFont());

    m_exportButton = new QPushButton(tr("Export JSON…"), controls);
    m_importButton = new QPushButton(tr("Import JSON…"), controls);
    m_copyJsonButton = new QPushButton(tr("Copy JSON"), controls);
    m_copyCppButton = new QPushButton(tr("Copy C++ snippet"), controls);
    m_copyTokensButton = new QPushButton(tr("Copy token report"), controls);
    m_screenshotButton = new QPushButton(tr("Export screenshot…"), controls);

    layout->addWidget(m_seedButton, 0, 0, 1, 2);
    layout->addWidget(m_lightRadio, 0, 2);
    layout->addWidget(m_darkRadio, 0, 3);
    layout->addWidget(m_followSystemRadio, 0, 4);
    layout->addWidget(m_contrastCombo, 0, 5);
    layout->addWidget(m_platformFontCheck, 0, 6);
    layout->addWidget(m_importButton, 1, 0);
    layout->addWidget(m_exportButton, 1, 1);
    layout->addWidget(m_copyJsonButton, 1, 2);
    layout->addWidget(m_copyCppButton, 1, 3);
    layout->addWidget(m_copyTokensButton, 1, 4);
    layout->addWidget(m_screenshotButton, 1, 5);
    layout->setColumnStretch(7, 1);

    root->addWidget(controls);

    connect(m_seedButton, &QPushButton::clicked, this, &ThemePreviewWindow::chooseSeedColor);
    connect(m_lightRadio, &QRadioButton::clicked, this, &ThemePreviewWindow::updateModePreferenceFromUi);
    connect(m_darkRadio, &QRadioButton::clicked, this, &ThemePreviewWindow::updateModePreferenceFromUi);
    connect(m_followSystemRadio, &QRadioButton::clicked, this, &ThemePreviewWindow::updateModePreferenceFromUi);
    connect(m_contrastCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ThemePreviewWindow::updateContrastFromUi);
    connect(m_platformFontCheck, &QCheckBox::toggled, this, [](bool checked) {
        SystemTheme::instance().setUsePlatformFont(checked);
        SystemTheme::instance().applyToThemeManager();
    });
    connect(m_exportButton, &QPushButton::clicked, this, &ThemePreviewWindow::exportThemeJson);
    connect(m_importButton, &QPushButton::clicked, this, &ThemePreviewWindow::importThemeJson);
    connect(m_copyJsonButton, &QPushButton::clicked, this, &ThemePreviewWindow::copyThemeJson);
    connect(m_copyCppButton, &QPushButton::clicked, this, &ThemePreviewWindow::copyCppSnippet);
    connect(m_copyTokensButton, &QPushButton::clicked, this, &ThemePreviewWindow::copyTokenReport);
    connect(m_screenshotButton, &QPushButton::clicked, this, &ThemePreviewWindow::exportScreenshot);
}

void ThemePreviewWindow::buildPreviewTabs(QVBoxLayout* root) {
    m_tabs = new QTabWidget(this);
    m_tabs->setObjectName(QStringLiteral("previewTabs"));
    m_tabs->addTab(buildComponentPreview(), tr("Components"));
    m_tabs->addTab(buildTypographyPreview(), tr("Typography"));
    m_tabs->addTab(buildShapePreview(), tr("Shape"));
    m_tabs->addTab(buildElevationPreview(), tr("Elevation"));
    m_tabs->addTab(buildStateLayerPreview(), tr("State layers"));
    root->addWidget(m_tabs, 2);
}

QWidget* ThemePreviewWindow::panel(const QString& title, QWidget* body) const {
    auto* frame = new QFrame(const_cast<ThemePreviewWindow*>(this));
    frame->setObjectName(QStringLiteral("previewPanel"));
    auto* layout = new QVBoxLayout(frame);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(12);
    auto* label = new QLabel(title, frame);
    label->setObjectName(QStringLiteral("panelTitle"));
    layout->addWidget(label);
    layout->addWidget(body);
    return frame;
}

QWidget* ThemePreviewWindow::buildComponentPreview() {
    const Theme& theme = ThemeManager::instance().theme();
    const ColorScheme& scheme = theme.colorScheme();

    auto* body = new QWidget(this);
    auto* layout = new QGridLayout(body);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);

    const QColor outline = scheme.color(ColorRole::OutlineVariant);
    const QColor surface = scheme.color(ColorRole::SurfaceContainerHigh);
    const QColor onSurface = scheme.color(ColorRole::OnSurface);

    auto* chipGrid = new QWidget(body);
    auto* chipLayout = new QGridLayout(chipGrid);
    chipLayout->setContentsMargins(0, 0, 0, 0);
    chipLayout->setSpacing(10);

    const QList<QPair<QString, QPair<ColorRole, ColorRole>>> roles = {
        {tr("Primary"), {ColorRole::Primary, ColorRole::OnPrimary}},
        {tr("Primary container"), {ColorRole::PrimaryContainer, ColorRole::OnPrimaryContainer}},
        {tr("Secondary"), {ColorRole::Secondary, ColorRole::OnSecondary}},
        {tr("Tertiary"), {ColorRole::Tertiary, ColorRole::OnTertiary}},
        {tr("Error"), {ColorRole::Error, ColorRole::OnError}},
        {tr("Surface"), {ColorRole::SurfaceContainer, ColorRole::OnSurface}}
    };

    int index = 0;
    for (const auto& role : roles) {
        chipLayout->addWidget(roleChip(role.first,
                                       scheme.color(role.second.first),
                                       scheme.color(role.second.second),
                                       chipGrid),
                              index / 3,
                              index % 3);
        ++index;
    }

    auto* buttons = new QWidget(body);
    auto* buttonLayout = new QHBoxLayout(buttons);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(12);
    auto* filled = new QPushButton(tr("Filled"), buttons);
    filled->setObjectName(QStringLiteral("filledButton"));
    auto* tonal = new QPushButton(tr("Tonal"), buttons);
    tonal->setObjectName(QStringLiteral("tonalButton"));
    auto* outlined = new QPushButton(tr("Outlined"), buttons);
    outlined->setObjectName(QStringLiteral("outlinedButton"));
    auto* text = new QPushButton(tr("Text"), buttons);
    text->setObjectName(QStringLiteral("textButton"));
    buttonLayout->addWidget(filled);
    buttonLayout->addWidget(tonal);
    buttonLayout->addWidget(outlined);
    buttonLayout->addWidget(text);
    buttonLayout->addStretch(1);

    auto* form = new QWidget(body);
    auto* formLayout = new QGridLayout(form);
    formLayout->setContentsMargins(0, 0, 0, 0);
    formLayout->setSpacing(12);
    auto* checkbox = new QCheckBox(tr("Checkbox state"), form);
    checkbox->setChecked(true);
    auto* radio = new QRadioButton(tr("Radio state"), form);
    radio->setChecked(true);
    auto* combo = new QComboBox(form);
    combo->addItems({tr("Component grid"), tr("Typography"), tr("Elevation")});
    auto* field = new QPlainTextEdit(form);
    field->setPlainText(tr("Editable component preview\nState layers and typography are visible here."));
    field->setMaximumHeight(96);
    formLayout->addWidget(checkbox, 0, 0);
    formLayout->addWidget(radio, 0, 1);
    formLayout->addWidget(combo, 0, 2);
    formLayout->addWidget(field, 1, 0, 1, 3);

    layout->addWidget(panel(tr("Color roles"), chipGrid), 0, 0);
    layout->addWidget(panel(tr("Buttons"), buttons), 0, 1);
    layout->addWidget(panel(tr("Inputs and selection"), form), 1, 0, 1, 2);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 1);

    body->setStyleSheet(QStringLiteral(
        "QWidget { background: transparent; color: %1; }"
        "QPlainTextEdit, QComboBox { background-color: %2; color: %1; border: 1px solid %3; border-radius: 12px; padding: 8px; }"
    ).arg(onSurface.name(QColor::HexArgb), surface.name(QColor::HexArgb), outline.name(QColor::HexArgb)));

    return body;
}

QWidget* ThemePreviewWindow::buildTypographyPreview() {
    const Theme& theme = ThemeManager::instance().theme();
    const ColorScheme& scheme = theme.colorScheme();
    const QColor onSurface = scheme.color(ColorRole::OnSurface);

    auto* body = new QWidget(this);
    auto* layout = new QVBoxLayout(body);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    const QList<QPair<TypeRole, QString>> roles = {
        {TypeRole::DisplayLarge, tr("Display Large")},
        {TypeRole::HeadlineMedium, tr("Headline Medium")},
        {TypeRole::TitleLarge, tr("Title Large")},
        {TypeRole::BodyLarge, tr("Body Large — The quick brown fox jumps over the lazy dog.")},
        {TypeRole::BodyMedium, tr("Body Medium — Stable tokenized typography preview.")},
        {TypeRole::LabelLarge, tr("Label Large")}
    };

    for (const auto& item : roles) {
        if (!theme.typography().contains(item.first)) {
            continue;
        }
        const TypographyStyle style = theme.typography().style(item.first);
        layout->addWidget(sampleLabel(item.second, style.font, onSurface, body));
    }
    layout->addStretch(1);
    return body;
}

QWidget* ThemePreviewWindow::buildShapePreview() {
    const Theme& theme = ThemeManager::instance().theme();
    const ColorScheme& scheme = theme.colorScheme();
    auto* body = new QWidget(this);
    auto* layout = new QGridLayout(body);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);

    const QList<QPair<ShapeRole, QString>> roles = {
        {ShapeRole::ExtraSmall, tr("Extra small")},
        {ShapeRole::Small, tr("Small")},
        {ShapeRole::Medium, tr("Medium")},
        {ShapeRole::Large, tr("Large")},
        {ShapeRole::ExtraLarge, tr("Extra large")},
        {ShapeRole::Full, tr("Full")}
    };

    int i = 0;
    for (const auto& item : roles) {
        const int radius = theme.shapes().contains(item.first) ? theme.shapes().radius(item.first) : 0;
        auto* card = new QLabel(QStringLiteral("%1\n%2 px").arg(item.second).arg(radius), body);
        card->setAlignment(Qt::AlignCenter);
        card->setMinimumSize(144, 88);
        card->setStyleSheet(QStringLiteral(
            "QLabel { background-color: %1; color: %2; border: 1px solid %3; border-radius: %4px; padding: 12px; }"
        ).arg(scheme.color(ColorRole::SurfaceContainerHigh).name(QColor::HexArgb),
              scheme.color(ColorRole::OnSurface).name(QColor::HexArgb),
              scheme.color(ColorRole::OutlineVariant).name(QColor::HexArgb),
              QString::number(qMin(radius, 44))));
        layout->addWidget(card, i / 3, i % 3);
        ++i;
    }
    return body;
}

QWidget* ThemePreviewWindow::buildElevationPreview() {
    const Theme& theme = ThemeManager::instance().theme();
    const ColorScheme& scheme = theme.colorScheme();
    auto* body = new QWidget(this);
    auto* layout = new QGridLayout(body);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);

    const QList<QPair<ElevationRole, QString>> roles = {
        {ElevationRole::Level0, tr("Level 0")},
        {ElevationRole::Level1, tr("Level 1")},
        {ElevationRole::Level2, tr("Level 2")},
        {ElevationRole::Level3, tr("Level 3")},
        {ElevationRole::Level4, tr("Level 4")},
        {ElevationRole::Level5, tr("Level 5")}
    };

    int i = 0;
    for (const auto& item : roles) {
        const ElevationStyle elevation = theme.elevations().contains(item.first)
            ? theme.elevations().style(item.first)
            : ElevationStyle{};
        auto* card = new QLabel(
            QStringLiteral("%1\nblur %2 / y %3\ntonal %4")
                .arg(item.second)
                .arg(elevation.shadowBlur)
                .arg(elevation.shadowYOffset)
                .arg(elevation.tonalOverlayOpacity, 0, 'f', 2),
            body);
        card->setAlignment(Qt::AlignCenter);
        card->setMinimumSize(156, 96);
        card->setStyleSheet(QStringLiteral(
            "QLabel { background-color: %1; color: %2; border: 1px solid %3; border-radius: 18px; padding: 12px; }"
        ).arg(scheme.color(ColorRole::SurfaceContainerHigh).name(QColor::HexArgb),
              scheme.color(ColorRole::OnSurface).name(QColor::HexArgb),
              scheme.color(ColorRole::OutlineVariant).name(QColor::HexArgb)));
        layout->addWidget(card, i / 3, i % 3);
        ++i;
    }
    return body;
}

QWidget* ThemePreviewWindow::buildStateLayerPreview() {
    const Theme& theme = ThemeManager::instance().theme();
    const ColorScheme& scheme = theme.colorScheme();
    auto* body = new QWidget(this);
    auto* layout = new QHBoxLayout(body);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);

    const StateLayer state = theme.stateLayer();
    const QColor base = scheme.color(ColorRole::PrimaryContainer);
    const QColor text = scheme.color(ColorRole::OnPrimaryContainer);
    const QList<QPair<QString, qreal>> states = {
        {tr("Hover"), state.hoverOpacity},
        {tr("Focus"), state.focusOpacity},
        {tr("Press"), state.pressOpacity},
        {tr("Drag"), state.dragOpacity}
    };

    for (const auto& item : states) {
        auto* chip = new QLabel(QStringLiteral("%1\n%2").arg(item.first).arg(item.second, 0, 'f', 2), body);
        chip->setAlignment(Qt::AlignCenter);
        chip->setMinimumSize(144, 92);
        chip->setStyleSheet(QStringLiteral(
            "QLabel { background-color: %1; color: %2; border-radius: 18px; padding: 12px; font-weight: 700; }"
        ).arg(overlay(base, state.color, item.second).name(QColor::HexArgb), text.name(QColor::HexArgb)));
        layout->addWidget(chip);
    }
    layout->addStretch(1);
    return body;
}

void ThemePreviewWindow::chooseSeedColor() {
    const QColor current = ThemeManager::instance().options().sourceColor;
    const QColor chosen = QColorDialog::getColor(current, this, tr("Choose seed color"));
    if (!chosen.isValid()) {
        return;
    }

    ThemeOptions options = ThemeManager::instance().options();
    options.sourceColor = chosen;
    if (m_followSystemRadio->isChecked()) {
        options.mode = SystemTheme::instance().effectiveMode();
    }
    ThemeManager::instance().setThemeOptions(options);
}

void ThemePreviewWindow::updateModePreferenceFromUi() {
    if (m_lightRadio->isChecked()) {
        SystemTheme::instance().setPreference(ThemeModePreference::Light);
        SystemTheme::instance().setAutoApplyToThemeManager(false);
        ThemeOptions options = ThemeManager::instance().options();
        options.mode = ThemeMode::Light;
        ThemeManager::instance().setThemeOptions(options);
        return;
    }
    if (m_darkRadio->isChecked()) {
        SystemTheme::instance().setPreference(ThemeModePreference::Dark);
        SystemTheme::instance().setAutoApplyToThemeManager(false);
        ThemeOptions options = ThemeManager::instance().options();
        options.mode = ThemeMode::Dark;
        ThemeManager::instance().setThemeOptions(options);
        return;
    }

    SystemTheme::instance().setPreference(ThemeModePreference::FollowSystem);
    SystemTheme::instance().setAutoApplyToThemeManager(true);
    SystemTheme::instance().applyToThemeManager();
}

void ThemePreviewWindow::updateContrastFromUi() {
    const auto contrast = static_cast<ContrastMode>(m_contrastCombo->currentData().toInt());
    ThemeOptions options = ThemeManager::instance().options();
    options.contrast = contrast;
    ThemeManager::instance().setThemeOptions(options);
}

void ThemePreviewWindow::exportThemeJson() {
    const QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Export theme JSON"),
        QStringLiteral("theme-v2.json"),
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

void ThemePreviewWindow::importThemeJson() {
    const QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Import theme JSON"),
        QString(),
        tr("JSON files (*.json);;All files (*)"));
    if (filePath.isEmpty()) {
        return;
    }

    QString error;
#if defined(QTMATERIAL3_HAS_THEME_READ_MODE)
    Q_UNUSED(error);
#endif
    if (!ThemeManager::instance().importThemeFromFile(filePath, &error)) {
        QMessageBox::critical(this, tr("Import failed"), error);
        return;
    }
    QMessageBox::information(this, tr("Import complete"), tr("Theme imported from %1").arg(filePath));
}

void ThemePreviewWindow::copyThemeJson() {
    if (auto* clipboard = QGuiApplication::clipboard()) {
        clipboard->setText(m_jsonPreview->toPlainText());
    }
}

void ThemePreviewWindow::copyCppSnippet() {
    if (auto* clipboard = QGuiApplication::clipboard()) {
        clipboard->setText(cppSnippet());
    }
}

void ThemePreviewWindow::copyTokenReport() {
    if (auto* clipboard = QGuiApplication::clipboard()) {
        clipboard->setText(tokenReport());
    }
}

void ThemePreviewWindow::exportScreenshot() {
    const QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Export screenshot"),
        QStringLiteral("theme-studio.png"),
        tr("PNG images (*.png);;All files (*)"));
    if (filePath.isEmpty()) {
        return;
    }

    QPixmap pixmap(size());
    render(&pixmap);
    if (!pixmap.save(filePath, "PNG")) {
        QMessageBox::critical(this, tr("Screenshot failed"), tr("Could not save %1").arg(filePath));
        return;
    }
    QMessageBox::information(this, tr("Screenshot exported"), tr("Screenshot saved to %1").arg(filePath));
}

QString ThemePreviewWindow::cppSnippet() const {
    const ThemeOptions& options = ThemeManager::instance().options();
    QString text;
    QTextStream out(&text);
    out << "QtMaterial::ThemeOptions options;\n";
    out << "options.sourceColor = QColor(QStringLiteral(\"" << options.sourceColor.name(QColor::HexRgb) << "\"));\n";
    out << "options.mode = QtMaterial::ThemeMode::" << themeModeToString(options.mode) << ";\n";
    out << "options.contrast = QtMaterial::ContrastMode::" << contrastToString(options.contrast) << ";\n";
    out << "QtMaterial::ThemeManager::instance().setThemeOptions(options);\n";
    if (m_followSystemRadio && m_followSystemRadio->isChecked()) {
        out << "QtMaterial::SystemTheme::instance().setPreference(QtMaterial::ThemeModePreference::FollowSystem);\n";
        out << "QtMaterial::SystemTheme::instance().setAutoApplyToThemeManager(true);\n";
    }
    return text;
}

QString ThemePreviewWindow::tokenReport() const {
    const Theme& theme = ThemeManager::instance().theme();
    const ColorScheme& scheme = theme.colorScheme();
    QString text;
    QTextStream out(&text);
    out << "/* qt-material3 token report */\n";
    out << ":theme {\n";
    const QList<ColorRole> roles = {
        ColorRole::Primary, ColorRole::OnPrimary,
        ColorRole::PrimaryContainer, ColorRole::OnPrimaryContainer,
        ColorRole::Secondary, ColorRole::OnSecondary,
        ColorRole::Tertiary, ColorRole::OnTertiary,
        ColorRole::Error, ColorRole::OnError,
        ColorRole::Surface, ColorRole::OnSurface,
        ColorRole::SurfaceContainer, ColorRole::SurfaceContainerHigh,
        ColorRole::SurfaceContainerHighest, ColorRole::Outline,
        ColorRole::OutlineVariant
    };
    for (ColorRole role : roles) {
        out << "  --md-sys-color-" << roleName(role) << ": " << scheme.color(role).name(QColor::HexRgb) << ";\n";
    }
    out << "}\n\n";
    out << "/* shape */\n";
    for (ShapeRole role : {ShapeRole::ExtraSmall, ShapeRole::Small, ShapeRole::Medium, ShapeRole::Large, ShapeRole::ExtraLarge, ShapeRole::Full}) {
        if (theme.shapes().contains(role)) {
            out << "shape-radius-" << static_cast<int>(role) << ": " << theme.shapes().radius(role) << "px\n";
        }
    }
    return text;
}

QString ThemePreviewWindow::modeLabelText() const {
    const auto snapshot = SystemTheme::instance().snapshot();
    return QStringLiteral("Preference: %1 · Effective mode: %2 · Native scheme: %3")
        .arg(toString(snapshot.preference),
             themeModeToString(snapshot.effectiveMode),
             snapshot.hasNativeColorScheme ? tr("yes") : tr("palette fallback"));
}

QString ThemePreviewWindow::contrastLabelText() const {
    const auto snapshot = SystemTheme::instance().snapshot();
    return QStringLiteral("Contrast: %1 · System high contrast: %2 · Platform font: %3")
        .arg(contrastToString(ThemeManager::instance().options().contrast),
             snapshot.highContrast ? tr("yes") : tr("no"),
             snapshot.platformFontFamily);
}

QString ThemePreviewWindow::colorName(ColorRole role) const {
    return ThemeManager::instance().theme().colorScheme().color(role).name(QColor::HexArgb);
}

void ThemePreviewWindow::setColorChip(QFrame* chip, const QColor& fill, const QColor& border) {
    if (!chip) {
        return;
    }
    chip->setStyleSheet(QStringLiteral(
        "QFrame { background-color: %1; border: 1px solid %2; border-radius: 12px; }"
    ).arg(fill.name(QColor::HexArgb), border.name(QColor::HexArgb)));
}

QLabel* ThemePreviewWindow::roleChip(const QString& name, const QColor& fill, const QColor& text, QWidget* parent) const {
    auto* chip = new QLabel(QStringLiteral("%1\n%2").arg(name, fill.name(QColor::HexRgb)), parent);
    chip->setAlignment(Qt::AlignCenter);
    chip->setMinimumHeight(86);
    chip->setStyleSheet(QStringLiteral(
        "QLabel { background-color: %1; color: %2; border-radius: 18px; padding: 12px; font-weight: 700; }"
    ).arg(fill.name(QColor::HexArgb), text.name(QColor::HexArgb)));
    return chip;
}

void ThemePreviewWindow::applyPlaygroundStyle() {
    const Theme& theme = ThemeManager::instance().theme();
    const ColorScheme& scheme = theme.colorScheme();

    const QColor primary = scheme.color(ColorRole::Primary);
    const QColor onPrimary = scheme.color(ColorRole::OnPrimary);
    const QColor primaryContainer = scheme.color(ColorRole::PrimaryContainer);
    const QColor onPrimaryContainer = scheme.color(ColorRole::OnPrimaryContainer);
    const QColor surface = scheme.color(ColorRole::Surface);
    const QColor onSurface = scheme.color(ColorRole::OnSurface);
    const QColor surfaceContainer = scheme.color(ColorRole::SurfaceContainer);
    const QColor surfaceContainerHigh = scheme.color(ColorRole::SurfaceContainerHigh);
    const QColor surfaceContainerHighest = scheme.color(ColorRole::SurfaceContainerHighest);
    const QColor outline = scheme.color(ColorRole::OutlineVariant);

    setStyleSheet(QStringLiteral(
        "#themeStudio { background-color: %1; color: %2; }"
        "#studioTitle { color: %2; font-size: 28px; font-weight: 800; }"
        "#studioStatus { color: %2; font-size: 13px; }"
        "#controlPanel, #previewPanel { background-color: %3; border: 1px solid %4; border-radius: 22px; }"
        "#panelTitle { color: %2; font-weight: 800; font-size: 15px; }"
        "#previewTabs::pane { border: 1px solid %4; border-radius: 18px; background: %3; }"
        "QTabBar::tab { color: %2; background: %5; padding: 10px 16px; border-top-left-radius: 12px; border-top-right-radius: 12px; margin-right: 4px; }"
        "QTabBar::tab:selected { background: %6; color: %7; }"
        "QRadioButton, QCheckBox, QComboBox, QLabel { color: %2; }"
        "QComboBox { background: %5; border: 1px solid %4; border-radius: 12px; padding: 8px; }"
        "QPlainTextEdit#jsonPreview { background-color: %5; color: %2; border: 1px solid %4; border-radius: 14px; padding: 12px; }"
        "QPushButton { border-radius: 18px; padding: 8px 14px; min-height: 34px; font-weight: 700; }"
        "QPushButton#primaryAction, QPushButton#filledButton { background: %6; color: %7; border: none; }"
        "QPushButton#tonalButton { background: %8; color: %9; border: none; }"
        "QPushButton#outlinedButton, QPushButton { background: transparent; color: %6; border: 1px solid %4; }"
        "QPushButton#textButton { background: transparent; color: %6; border: none; }"
    ).arg(surface.name(QColor::HexArgb),
          onSurface.name(QColor::HexArgb),
          surfaceContainer.name(QColor::HexArgb),
          outline.name(QColor::HexArgb),
          surfaceContainerHigh.name(QColor::HexArgb),
          primary.name(QColor::HexArgb),
          onPrimary.name(QColor::HexArgb),
          primaryContainer.name(QColor::HexArgb),
          onPrimaryContainer.name(QColor::HexArgb)));
}

void ThemePreviewWindow::refreshPreview() {
    const Theme& theme = ThemeManager::instance().theme();
    const ColorScheme& scheme = theme.colorScheme();
    const ThemeOptions& options = theme.options();

    if (m_statusLabel) {
        m_statusLabel->setText(QStringLiteral("%1\n%2\nSeed: %3")
            .arg(modeLabelText(), contrastLabelText(), options.sourceColor.name(QColor::HexRgb)));
    }

    if (m_contrastCombo) {
        const int value = static_cast<int>(options.contrast);
        const int index = m_contrastCombo->findData(value);
        if (index >= 0 && m_contrastCombo->currentIndex() != index) {
            QSignalBlocker blocker(m_contrastCombo);
            m_contrastCombo->setCurrentIndex(index);
        }
    }

    setColorChip(m_primaryChip, scheme.color(ColorRole::Primary), scheme.color(ColorRole::OutlineVariant));
    setColorChip(m_secondaryChip, scheme.color(ColorRole::Secondary), scheme.color(ColorRole::OutlineVariant));
    setColorChip(m_tertiaryChip, scheme.color(ColorRole::Tertiary), scheme.color(ColorRole::OutlineVariant));
    setColorChip(m_errorChip, scheme.color(ColorRole::Error), scheme.color(ColorRole::OutlineVariant));
    setColorChip(m_surfaceChip, scheme.color(ColorRole::SurfaceContainerHighest), scheme.color(ColorRole::OutlineVariant));

    applyPlaygroundStyle();

    if (m_jsonPreview) {
        m_jsonPreview->setPlainText(QString::fromUtf8(ThemeManager::instance().exportThemeJson(QJsonDocument::Indented)));
    }

    if (m_tabs) {
        const int current = m_tabs->currentIndex();
        m_tabs->clear();
        m_tabs->addTab(buildComponentPreview(), tr("Components"));
        m_tabs->addTab(buildTypographyPreview(), tr("Typography"));
        m_tabs->addTab(buildShapePreview(), tr("Shape"));
        m_tabs->addTab(buildElevationPreview(), tr("Elevation"));
        m_tabs->addTab(buildStateLayerPreview(), tr("State layers"));
        m_tabs->setCurrentIndex(qBound(0, current, m_tabs->count() - 1));
    }
}
