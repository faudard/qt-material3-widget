#include "qtmaterial/theme/qtmaterialsystemtheme.h"
#include "qtmaterial/theme/qtmaterialaccessibilitytokens.h"

#include "qtmaterial/theme/qtmaterialtheme.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"

#include <QEvent>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QMetaObject>
#include <QPalette>
#include <QStyleHints>

#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
#include <QAccessibilityHints>
#endif

namespace QtMaterial {
namespace {

ThemeMode modeFromPalette() {
    const QPalette palette = QGuiApplication::palette();
    const QColor window = palette.color(QPalette::Window);
    const QColor text = palette.color(QPalette::WindowText);
    return window.lightnessF() < text.lightnessF() ? ThemeMode::Dark : ThemeMode::Light;
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
ThemeMode modeFromQtColorScheme(Qt::ColorScheme scheme) {
    if (scheme == Qt::ColorScheme::Dark) {
        return ThemeMode::Dark;
    }
    if (scheme == Qt::ColorScheme::Light) {
        return ThemeMode::Light;
    }
    return modeFromPalette();
}
#endif

void assignFamily(TypographyScale& typography, TypeRole role, const QString& family) {
    if (!typography.contains(role)) {
        return;
    }
    TypographyStyle style = typography.style(role);
    style.font.setFamily(family);
    typography.setStyle(role, style);
}

} // namespace

SystemTheme& SystemTheme::instance() {
    static SystemTheme bridge;
    return bridge;
}

SystemTheme::SystemTheme(QObject* parent)
    : QObject(parent)
    , m_lastSnapshot(snapshot()) {
    if (auto* app = QCoreApplication::instance()) {
        app->installEventFilter(this);
    }
    connectPlatformSignals();
}

ThemeModePreference SystemTheme::preference() const noexcept {
    return m_preference;
}

void SystemTheme::setPreference(ThemeModePreference preference) {
    if (m_preference == preference) {
        return;
    }

    const SystemThemeSnapshot before = snapshot();
    m_preference = preference;
    const SystemThemeSnapshot after = snapshot();
    m_lastSnapshot = after;
    emitDelta(before, after);

    if (m_autoApplyToThemeManager) {
        applyToThemeManager();
    }
}

bool SystemTheme::autoApplyToThemeManager() const noexcept {
    return m_autoApplyToThemeManager;
}

void SystemTheme::setAutoApplyToThemeManager(bool enabled) {
    if (m_autoApplyToThemeManager == enabled) {
        return;
    }
    m_autoApplyToThemeManager = enabled;
    if (m_autoApplyToThemeManager) {
        applyToThemeManager();
    }
}

bool SystemTheme::usePlatformFont() const noexcept {
    return m_usePlatformFont;
}

void SystemTheme::setUsePlatformFont(bool enabled) {
    if (m_usePlatformFont == enabled) {
        return;
    }
    m_usePlatformFont = enabled;
    if (m_autoApplyToThemeManager) {
        applyToThemeManager();
    }
}

SystemThemeSnapshot SystemTheme::snapshot() const {
    SystemThemeSnapshot value;
    value.preference = m_preference;
    value.effectiveMode = effectiveMode();
    value.effectiveContrast = effectiveContrast();
    value.highContrast = isHighContrastEnabled();
    value.hasNativeColorScheme = hasNativeColorScheme();
    value.platformFontFamily = platformFont().family();
    return value;
}

ThemeMode SystemTheme::effectiveMode() const {
    if (m_preference == ThemeModePreference::Light) {
        return ThemeMode::Light;
    }
    if (m_preference == ThemeModePreference::Dark) {
        return ThemeMode::Dark;
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    if (const auto* hints = QGuiApplication::styleHints()) {
        return modeFromQtColorScheme(hints->colorScheme());
    }
#endif

    return modeFromPalette();
}

ContrastMode SystemTheme::effectiveContrast() const {
    return isHighContrastEnabled() ? ContrastMode::High : ContrastMode::Standard;
}

bool SystemTheme::isHighContrastEnabled() const {
#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
    if (const auto* hints = QGuiApplication::styleHints()) {
        if (const QAccessibilityHints* accessibility = hints->accessibility()) {
            return accessibility->contrastPreference() == Qt::ContrastPreference::HighContrast;
        }
    }
#endif
    return false;
}

bool SystemTheme::hasNativeColorScheme() const {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    if (const auto* hints = QGuiApplication::styleHints()) {
        return hints->colorScheme() != Qt::ColorScheme::Unknown;
    }
#endif
    return false;
}



bool SystemTheme::isHighContrastAvailable() const {
#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
    return QGuiApplication::styleHints() && QGuiApplication::styleHints()->accessibility();
#else
    return false;
#endif
}

bool SystemTheme::isReducedMotionEnabled() const {
    // Qt Widgets does not expose a universal reduced-motion platform setting.
    // Keep this as an explicit policy hook for applications and future Qt APIs.
    return false;
}

void SystemTheme::applyAccessibilityPreferencesToTheme(Theme& theme) const {
    AccessibilityTokens& accessibility = theme.accessibility();
    accessibility.highContrast = isHighContrastEnabled();
    accessibility.reducedMotion = isReducedMotionEnabled();
    if (accessibility.highContrast) {
        accessibility.minimumTextContrastRatio = qMax<qreal>(accessibility.minimumTextContrastRatio, 7.0);
        accessibility.minimumUiContrastRatio = qMax<qreal>(accessibility.minimumUiContrastRatio, 4.5);
        accessibility.focusRing.width = qMax(accessibility.focusRing.width, 3);
        accessibility.focusRing.offset = qMax(accessibility.focusRing.offset, 3);
    }
    applyReducedMotion(&theme.motion(), accessibility.reducedMotion);
}

QFont SystemTheme::platformFont() const {
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
    return QFontDatabase::systemFont(QFontDatabase::GeneralFont);
#else
    return QGuiApplication::font();
#endif
}

void SystemTheme::applyPlatformFontToTheme(Theme& theme) const {
    const QString family = platformFont().family();
    if (family.isEmpty()) {
        return;
    }

    auto& typography = theme.typography();
    assignFamily(typography, TypeRole::DisplayLarge, family);
    assignFamily(typography, TypeRole::DisplayMedium, family);
    assignFamily(typography, TypeRole::DisplaySmall, family);
    assignFamily(typography, TypeRole::HeadlineLarge, family);
    assignFamily(typography, TypeRole::HeadlineMedium, family);
    assignFamily(typography, TypeRole::HeadlineSmall, family);
    assignFamily(typography, TypeRole::TitleLarge, family);
    assignFamily(typography, TypeRole::TitleMedium, family);
    assignFamily(typography, TypeRole::TitleSmall, family);
    assignFamily(typography, TypeRole::BodyLarge, family);
    assignFamily(typography, TypeRole::BodyMedium, family);
    assignFamily(typography, TypeRole::BodySmall, family);
    assignFamily(typography, TypeRole::LabelLarge, family);
    assignFamily(typography, TypeRole::LabelMedium, family);
    assignFamily(typography, TypeRole::LabelSmall, family);
}

void SystemTheme::refresh() {
    const SystemThemeSnapshot before = m_lastSnapshot;
    const SystemThemeSnapshot after = snapshot();
    m_lastSnapshot = after;
    emitDelta(before, after);

    if (m_autoApplyToThemeManager) {
        applyToThemeManager();
    }
}

void SystemTheme::applyToThemeManager() {
    ThemeOptions options = ThemeManager::instance().options();
    options.mode = effectiveMode();
    options.contrast = effectiveContrast();

    if (!m_usePlatformFont) {
        ThemeManager::instance().setThemeOptions(options);
        return;
    }

    ThemeBuilder builder;
    Theme theme = builder.build(options);
    applyPlatformFontToTheme(theme);
    ThemeManager::instance().setTheme(theme);
}

bool SystemTheme::eventFilter(QObject* watched, QEvent* event) {
    Q_UNUSED(watched);
    if (!event) {
        return false;
    }

    switch (event->type()) {
    case QEvent::ApplicationPaletteChange:
    case QEvent::PaletteChange:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    case QEvent::ThemeChange:
#endif
        refresh();
        break;
    default:
        break;
    }

    return false;
}

void SystemTheme::connectPlatformSignals() {
    if (auto* hints = QGuiApplication::styleHints()) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        connect(hints, &QStyleHints::colorSchemeChanged, this, [this](Qt::ColorScheme) {
            refresh();
        });
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
        if (const QAccessibilityHints* accessibility = hints->accessibility()) {
            connect(accessibility,
                    &QAccessibilityHints::contrastPreferenceChanged,
                    this,
                    [this](Qt::ContrastPreference) { refresh(); });
        }
#endif
    }
}

void SystemTheme::emitDelta(
    const SystemThemeSnapshot& before,
    const SystemThemeSnapshot& after) {
    if (before.effectiveMode != after.effectiveMode) {
        emit effectiveModeChanged(after.effectiveMode);
    }
    if (before.highContrast != after.highContrast) {
        emit highContrastChanged(after.highContrast);
    }
    if (before.effectiveMode != after.effectiveMode
        || before.effectiveContrast != after.effectiveContrast
        || before.highContrast != after.highContrast
        || before.platformFontFamily != after.platformFontFamily
        || before.preference != after.preference) {
        emit systemThemeChanged();
    }
}

QString toString(ThemeModePreference preference) {
    switch (preference) {
    case ThemeModePreference::Light:
        return QStringLiteral("light");
    case ThemeModePreference::Dark:
        return QStringLiteral("dark");
    case ThemeModePreference::FollowSystem:
        return QStringLiteral("followSystem");
    }
    return QStringLiteral("followSystem");
}

ThemeModePreference themeModePreferenceFromString(const QString& value, bool* ok) {
    const QString normalized = value.trimmed().toLower();
    if (normalized == QLatin1String("light")) {
        if (ok) *ok = true;
        return ThemeModePreference::Light;
    }
    if (normalized == QLatin1String("dark")) {
        if (ok) *ok = true;
        return ThemeModePreference::Dark;
    }
    if (normalized == QLatin1String("followsystem")
        || normalized == QLatin1String("system")
        || normalized == QLatin1String("auto")) {
        if (ok) *ok = true;
        return ThemeModePreference::FollowSystem;
    }

    if (ok) *ok = false;
    return ThemeModePreference::FollowSystem;
}

} // namespace QtMaterial
