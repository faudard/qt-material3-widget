#include "dashboardsettingspanel.h"

#include "ui_dashboardsettingspanel.h"

#include <QAbstractButton>
#include <QApplication>
#include <QButtonGroup>
#include <QColor>
#include <QFont>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QSignalBlocker>
#include <QToolButton>
#include <QVBoxLayout>

#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"
#include "qtmaterial/widgets/selection/qtmaterialswitch.h"
#include "qtmaterial/widgets/surfaces/qtmaterialcard.h"

namespace {

QString cssColor(const QColor& color)
{
    return color.name(QColor::HexRgb);
}

QtMaterial::QtMaterialCard* settingCard(
    const QString& symbol,
    const QString& title,
    QtMaterial::QtMaterialSwitch** outSwitch,
    QWidget* parent)
{
    auto* card = new QtMaterial::QtMaterialCard(parent);
    card->setVariant(QtMaterial::QtMaterialCard::Variant::Outlined);
    card->setMinimumHeight(138);
    card->setObjectName(QStringLiteral("settingsToggleCard"));

    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(16, 14, 16, 14);
    layout->setSpacing(8);

    auto* top = new QHBoxLayout;
    auto* icon = new QLabel(symbol, card);
    icon->setObjectName(QStringLiteral("settingsCardIcon"));
    QFont iconFont = icon->font();
    iconFont.setPointSizeF(iconFont.pointSizeF() + 7.0);
    iconFont.setBold(true);
    icon->setFont(iconFont);
    top->addWidget(icon);
    top->addStretch(1);

    auto* toggle = new QtMaterial::QtMaterialSwitch(card);
    top->addWidget(toggle);
    layout->addLayout(top);
    layout->addStretch(1);

    auto* titleLabel = new QLabel(title, card);
    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleFont.setPointSizeF(titleFont.pointSizeF() + 1.0);
    titleLabel->setFont(titleFont);
    layout->addWidget(titleLabel);

    *outSwitch = toggle;
    return card;
}

QToolButton* layoutChoice(
    const QString& icon,
    const QString& tooltip,
    QWidget* parent)
{
    auto* button = new QToolButton(parent);
    button->setText(icon);
    button->setToolTip(tooltip);
    button->setAccessibleName(tooltip);
    button->setCheckable(true);
    button->setFixedSize(88, 72);
    button->setObjectName(QStringLiteral("layoutChoice"));
    QFont font = button->font();
    font.setPointSizeF(font.pointSizeF() + 5.0);
    button->setFont(font);
    return button;
}

} // namespace

DashboardSettingsPanel::DashboardSettingsPanel(QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::DashboardSettingsPanel)
{
    m_ui->setupUi(this);
    setObjectName(QStringLiteral("dashboardSettingsPanel"));

    QFont titleFont = m_ui->titleLabel->font();
    titleFont.setBold(true);
    titleFont.setPointSizeF(titleFont.pointSizeF() + 4.0);
    m_ui->titleLabel->setFont(titleFont);

    auto* reset = new QToolButton(this);
    reset->setText(QStringLiteral("↻"));
    reset->setToolTip(QStringLiteral("Reset settings"));
    reset->setAccessibleName(QStringLiteral("Reset settings"));
    reset->setObjectName(QStringLiteral("panelActionButton"));
    reset->setFixedSize(36, 36);

    auto* close = new QToolButton(this);
    close->setText(QStringLiteral("×"));
    close->setToolTip(QStringLiteral("Close settings"));
    close->setAccessibleName(QStringLiteral("Close settings"));
    close->setObjectName(QStringLiteral("panelCloseButton"));
    close->setFixedSize(36, 36);

    m_ui->headerLayout->addWidget(reset);
    m_ui->headerLayout->addWidget(close);

    auto* modeCard = settingCard(
        QStringLiteral("☾"),
        QStringLiteral("Mode"),
        &m_modeSwitch,
        m_ui->scrollContent);
    auto* contrastCard = settingCard(
        QStringLiteral("◐"),
        QStringLiteral("Contrast"),
        &m_contrastSwitch,
        m_ui->scrollContent);
    auto* rtlCard = settingCard(
        QStringLiteral("⇤"),
        QStringLiteral("Right to left"),
        &m_rtlSwitch,
        m_ui->scrollContent);
    auto* compactCard = settingCard(
        QStringLiteral("↔"),
        QStringLiteral("Compact"),
        &m_compactSwitch,
        m_ui->scrollContent);

    m_ui->settingsGrid->addWidget(modeCard, 0, 0);
    m_ui->settingsGrid->addWidget(contrastCard, 0, 1);
    m_ui->settingsGrid->addWidget(rtlCard, 1, 0);
    m_ui->settingsGrid->addWidget(compactCard, 1, 1);
    m_ui->settingsGrid->setColumnStretch(0, 1);
    m_ui->settingsGrid->setColumnStretch(1, 1);
    m_compactSwitch->setChecked(true);

    auto* navigationCard = new QtMaterial::QtMaterialCard(m_ui->scrollContent);
    navigationCard->setVariant(QtMaterial::QtMaterialCard::Variant::Outlined);
    navigationCard->setMinimumHeight(245);
    auto* navigationCardLayout = new QVBoxLayout(navigationCard);
    navigationCardLayout->setContentsMargins(16, 16, 16, 16);
    navigationCardLayout->setSpacing(12);

    auto* navTitle = new QLabel(QStringLiteral("Navigation"), navigationCard);
    QFont navFont = navTitle->font();
    navFont.setBold(true);
    navFont.setPointSizeF(navFont.pointSizeF() + 1.0);
    navTitle->setFont(navFont);
    navigationCardLayout->addWidget(navTitle);

    auto* navSubtitle = new QLabel(QStringLiteral("Layout"), navigationCard);
    navSubtitle->setObjectName(QStringLiteral("settingsMutedLabel"));
    navigationCardLayout->addWidget(navSubtitle);

    auto* choices = new QHBoxLayout;
    auto* layoutGroup = new QButtonGroup(navigationCard);
    layoutGroup->setExclusive(true);

    auto* integrated = layoutChoice(QStringLiteral("▥"), QStringLiteral("Integrated navigation"), navigationCard);
    auto* topbar = layoutChoice(QStringLiteral("▤"), QStringLiteral("Top navigation"), navigationCard);
    auto* rail = layoutChoice(QStringLiteral("▯"), QStringLiteral("Navigation rail"), navigationCard);
    integrated->setChecked(true);
    layoutGroup->addButton(integrated, 0);
    layoutGroup->addButton(topbar, 1);
    layoutGroup->addButton(rail, 2);
    choices->addWidget(integrated);
    choices->addWidget(topbar);
    choices->addWidget(rail);
    choices->addStretch(1);
    navigationCardLayout->addLayout(choices);

    auto* colorSubtitle = new QLabel(QStringLiteral("Color"), navigationCard);
    colorSubtitle->setObjectName(QStringLiteral("settingsMutedLabel"));
    navigationCardLayout->addWidget(colorSubtitle);

    auto* colorRow = new QHBoxLayout;
    auto* integrateColor = new QtMaterial::QtMaterialFilledTonalButton(
        QStringLiteral("Integrate"),
        navigationCard);
    auto* apparentColor = new QtMaterial::QtMaterialOutlinedButton(
        QStringLiteral("Apparent"),
        navigationCard);
    colorRow->addWidget(integrateColor);
    colorRow->addWidget(apparentColor);
    colorRow->addStretch(1);
    navigationCardLayout->addLayout(colorRow);
    m_ui->navigationLayout->addWidget(navigationCard);

    auto* presetsCard = new QtMaterial::QtMaterialCard(m_ui->scrollContent);
    presetsCard->setVariant(QtMaterial::QtMaterialCard::Variant::Outlined);
    auto* presets = new QVBoxLayout(presetsCard);
    presets->setContentsMargins(16, 16, 16, 16);
    presets->setSpacing(10);

    auto* presetsTitle = new QLabel(QStringLiteral("Presets"), presetsCard);
    QFont presetsFont = presetsTitle->font();
    presetsFont.setBold(true);
    presetsFont.setPointSizeF(presetsFont.pointSizeF() + 1.0);
    presetsTitle->setFont(presetsFont);
    presets->addWidget(presetsTitle);

    auto* presetRow = new QHBoxLayout;
    const QStringList presetNames = {
        QStringLiteral("Indigo"),
        QStringLiteral("Amber"),
        QStringLiteral("Teal")
    };
    for (const QString& preset : presetNames) {
        auto* button = new QtMaterial::QtMaterialOutlinedButton(preset, presetsCard);
        presetRow->addWidget(button);
        connect(button, &QAbstractButton::clicked, this, [this, preset]() {
            auto options = QtMaterial::ThemeManager::instance().options();
            if (preset == QStringLiteral("Amber")) {
                options.sourceColor = QColor(QStringLiteral("#FFB300"));
            } else if (preset == QStringLiteral("Teal")) {
                options.sourceColor = QColor(QStringLiteral("#006A60"));
            } else {
                options.sourceColor = QColor(QStringLiteral("#4455C7"));
            }
            QtMaterial::ThemeManager::instance().setThemeOptions(options);
            emit messageRequested(QStringLiteral("%1 preset applied.").arg(preset));
        });
    }
    presets->addLayout(presetRow);
    m_ui->presetsLayout->addWidget(presetsCard);

    connect(close, &QToolButton::clicked, this, &DashboardSettingsPanel::closeRequested);
    connect(reset, &QToolButton::clicked, this, [this, integrated]() {
        auto options = QtMaterial::ThemeManager::instance().options();
        options.sourceColor = QColor(QStringLiteral("#4455C7"));
        options.mode = QtMaterial::ThemeMode::Light;
        options.preference = QtMaterial::ThemePreference::Light;
        options.contrast = QtMaterial::ContrastMode::Standard;
        QtMaterial::ThemeManager::instance().setThemeOptions(options);
        qApp->setLayoutDirection(Qt::LeftToRight);
        m_compactSwitch->setChecked(true);
        integrated->setChecked(true);
        emit compactChanged(true);
        emit messageRequested(QStringLiteral("Settings reset."));
    });

    connect(m_modeSwitch, &QAbstractButton::toggled, this, [](bool dark) {
        auto options = QtMaterial::ThemeManager::instance().options();
        options.mode = dark ? QtMaterial::ThemeMode::Dark : QtMaterial::ThemeMode::Light;
        options.preference = dark ? QtMaterial::ThemePreference::Dark : QtMaterial::ThemePreference::Light;
        QtMaterial::ThemeManager::instance().setThemeOptions(options);
    });
    connect(m_contrastSwitch, &QAbstractButton::toggled, this, [](bool high) {
        auto options = QtMaterial::ThemeManager::instance().options();
        options.contrast = high ? QtMaterial::ContrastMode::High : QtMaterial::ContrastMode::Standard;
        QtMaterial::ThemeManager::instance().setThemeOptions(options);
    });
    connect(m_rtlSwitch, &QAbstractButton::toggled, this, [](bool rtl) {
        qApp->setLayoutDirection(rtl ? Qt::RightToLeft : Qt::LeftToRight);
    });
    connect(m_compactSwitch, &QAbstractButton::toggled, this, &DashboardSettingsPanel::compactChanged);

    connect(integrated, &QToolButton::clicked, this, [this]() {
        emit messageRequested(QStringLiteral("Integrated navigation preview selected."));
    });
    connect(topbar, &QToolButton::clicked, this, [this]() {
        emit messageRequested(QStringLiteral("Top navigation preview selected."));
    });
    connect(rail, &QToolButton::clicked, this, [this]() {
        emit messageRequested(QStringLiteral("Navigation rail preview selected."));
    });
    connect(integrateColor, &QAbstractButton::clicked, this, [this]() {
        emit messageRequested(QStringLiteral("Integrated navigation color selected."));
    });
    connect(apparentColor, &QAbstractButton::clicked, this, [this]() {
        emit messageRequested(QStringLiteral("Apparent navigation color selected."));
    });

    connect(
        &QtMaterial::ThemeManager::instance(),
        &QtMaterial::ThemeManager::themeChanged,
        this,
        [this](const QtMaterial::Theme&) {
            syncFromTheme();
            applyTheme();
        });

    syncFromTheme();
    applyTheme();
}

DashboardSettingsPanel::~DashboardSettingsPanel()
{
    delete m_ui;
}

void DashboardSettingsPanel::syncFromTheme()
{
    const auto& theme = QtMaterial::ThemeManager::instance().theme();
    const auto options = QtMaterial::ThemeManager::instance().options();

    const QSignalBlocker modeBlocker(m_modeSwitch);
    const QSignalBlocker contrastBlocker(m_contrastSwitch);
    const QSignalBlocker rtlBlocker(m_rtlSwitch);

    m_modeSwitch->setChecked(theme.isDark());
    m_contrastSwitch->setChecked(options.contrast == QtMaterial::ContrastMode::High);
    m_rtlSwitch->setChecked(qApp->layoutDirection() == Qt::RightToLeft);
}

void DashboardSettingsPanel::applyTheme()
{
    const auto& scheme = QtMaterial::ThemeManager::instance().theme().colorScheme();
    const QColor surface = scheme.color(QtMaterial::ColorRole::Surface);
    const QColor surfaceContainer = scheme.color(QtMaterial::ColorRole::SurfaceContainerLow);
    const QColor onSurface = scheme.color(QtMaterial::ColorRole::OnSurface);
    const QColor onSurfaceVariant = scheme.color(QtMaterial::ColorRole::OnSurfaceVariant);
    const QColor primary = scheme.color(QtMaterial::ColorRole::Primary);
    const QColor primaryContainer = scheme.color(QtMaterial::ColorRole::PrimaryContainer);
    const QColor outline = scheme.color(QtMaterial::ColorRole::OutlineVariant);

    setStyleSheet(QStringLiteral(
        "#dashboardSettingsPanel { background:%1; border-left:1px solid %5; }"
        "#dashboardSettingsPanel QLabel { color:%2; }"
        "#dashboardSettingsPanel #settingsMutedLabel { color:%3; }"
        "#dashboardSettingsPanel #settingsCardIcon { color:%3; }"
        "#dashboardSettingsPanel #panelActionButton,"
        "#dashboardSettingsPanel #panelCloseButton { background:transparent; color:%3; border:0; border-radius:18px; font-size:22px; }"
        "#dashboardSettingsPanel #panelActionButton:hover,"
        "#dashboardSettingsPanel #panelCloseButton:hover { background:%4; }"
        "#dashboardSettingsPanel #layoutChoice { background:%4; color:%3; border:1px solid %5; border-radius:12px; }"
        "#dashboardSettingsPanel #layoutChoice:checked { background:%6; color:%2; border:2px solid %7; }"
        "QScrollArea { background:%1; border:0; }"
        "QScrollArea > QWidget > QWidget { background:%1; }")
        .arg(cssColor(surface))
        .arg(cssColor(onSurface))
        .arg(cssColor(onSurfaceVariant))
        .arg(cssColor(surfaceContainer))
        .arg(cssColor(outline))
        .arg(cssColor(primaryContainer))
        .arg(cssColor(primary)));

    QPalette panelPalette = palette();
    panelPalette.setColor(QPalette::Window, surface);
    panelPalette.setColor(QPalette::WindowText, onSurface);
    setPalette(panelPalette);
    setAutoFillBackground(true);
}
