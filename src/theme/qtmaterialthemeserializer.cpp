#include "qtmaterial/theme/qtmaterialthemeserializer.h"

#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonParseError>
#include <QtCore/QSaveFile>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"

namespace QtMaterial {

namespace {

template <typename Enum>
struct EnumName
{
    Enum value;
    const char* name;
};

constexpr EnumName<ThemeMode> kThemeModes[] = {
    { ThemeMode::Light, "light" },
    { ThemeMode::Dark, "dark" },
};

constexpr EnumName<ContrastMode> kContrastModes[] = {
    { ContrastMode::Standard, "standard" },
    { ContrastMode::Medium, "medium" },
    { ContrastMode::High, "high" },
};

constexpr EnumName<ColorRole> kColorRoles[] = {
    { ColorRole::Primary, "Primary" },
    { ColorRole::OnPrimary, "OnPrimary" },
    { ColorRole::PrimaryContainer, "PrimaryContainer" },
    { ColorRole::OnPrimaryContainer, "OnPrimaryContainer" },
    { ColorRole::Secondary, "Secondary" },
    { ColorRole::OnSecondary, "OnSecondary" },
    { ColorRole::SecondaryContainer, "SecondaryContainer" },
    { ColorRole::OnSecondaryContainer, "OnSecondaryContainer" },
    { ColorRole::Tertiary, "Tertiary" },
    { ColorRole::OnTertiary, "OnTertiary" },
    { ColorRole::TertiaryContainer, "TertiaryContainer" },
    { ColorRole::OnTertiaryContainer, "OnTertiaryContainer" },
    { ColorRole::Error, "Error" },
    { ColorRole::OnError, "OnError" },
    { ColorRole::ErrorContainer, "ErrorContainer" },
    { ColorRole::OnErrorContainer, "OnErrorContainer" },
    { ColorRole::Background, "Background" },
    { ColorRole::OnBackground, "OnBackground" },
    { ColorRole::Surface, "Surface" },
    { ColorRole::OnSurface, "OnSurface" },
    { ColorRole::SurfaceDim, "SurfaceDim" },
    { ColorRole::SurfaceBright, "SurfaceBright" },
    { ColorRole::SurfaceContainerLowest, "SurfaceContainerLowest" },
    { ColorRole::SurfaceContainerLow, "SurfaceContainerLow" },
    { ColorRole::SurfaceContainer, "SurfaceContainer" },
    { ColorRole::SurfaceContainerHigh, "SurfaceContainerHigh" },
    { ColorRole::SurfaceContainerHighest, "SurfaceContainerHighest" },
    { ColorRole::SurfaceVariant, "SurfaceVariant" },
    { ColorRole::OnSurfaceVariant, "OnSurfaceVariant" },
    { ColorRole::Outline, "Outline" },
    { ColorRole::OutlineVariant, "OutlineVariant" },
    { ColorRole::InverseSurface, "InverseSurface" },
    { ColorRole::InverseOnSurface, "InverseOnSurface" },
    { ColorRole::InversePrimary, "InversePrimary" },
    { ColorRole::Shadow, "Shadow" },
    { ColorRole::Scrim, "Scrim" },
};

constexpr EnumName<TypeRole> kTypeRoles[] = {
    { TypeRole::DisplayLarge, "DisplayLarge" },
    { TypeRole::DisplayMedium, "DisplayMedium" },
    { TypeRole::DisplaySmall, "DisplaySmall" },
    { TypeRole::HeadlineLarge, "HeadlineLarge" },
    { TypeRole::HeadlineMedium, "HeadlineMedium" },
    { TypeRole::HeadlineSmall, "HeadlineSmall" },
    { TypeRole::TitleLarge, "TitleLarge" },
    { TypeRole::TitleMedium, "TitleMedium" },
    { TypeRole::TitleSmall, "TitleSmall" },
    { TypeRole::BodyLarge, "BodyLarge" },
    { TypeRole::BodyMedium, "BodyMedium" },
    { TypeRole::BodySmall, "BodySmall" },
    { TypeRole::LabelLarge, "LabelLarge" },
    { TypeRole::LabelMedium, "LabelMedium" },
    { TypeRole::LabelSmall, "LabelSmall" },
};

constexpr EnumName<ShapeRole> kShapeRoles[] = {
    { ShapeRole::None, "None" },
    { ShapeRole::ExtraSmall, "ExtraSmall" },
    { ShapeRole::Small, "Small" },
    { ShapeRole::Medium, "Medium" },
    { ShapeRole::Large, "Large" },
    { ShapeRole::ExtraLarge, "ExtraLarge" },
    { ShapeRole::Full, "Full" },
};

constexpr EnumName<ElevationRole> kElevationRoles[] = {
    { ElevationRole::Level0, "Level0" },
    { ElevationRole::Level1, "Level1" },
    { ElevationRole::Level2, "Level2" },
    { ElevationRole::Level3, "Level3" },
    { ElevationRole::Level4, "Level4" },
    { ElevationRole::Level5, "Level5" },
};

constexpr EnumName<MotionToken> kMotionTokens[] = {
    { MotionToken::Short1, "Short1" },
    { MotionToken::Short2, "Short2" },
    { MotionToken::Short3, "Short3" },
    { MotionToken::Short4, "Short4" },
    { MotionToken::Medium1, "Medium1" },
    { MotionToken::Medium2, "Medium2" },
    { MotionToken::Medium3, "Medium3" },
    { MotionToken::Medium4, "Medium4" },
    { MotionToken::Long1, "Long1" },
    { MotionToken::Long2, "Long2" },
    { MotionToken::Long3, "Long3" },
    { MotionToken::Long4, "Long4" },
};

template <typename Enum, int N>
QString enumToString(Enum value, const EnumName<Enum> (&pairs)[N])
{
    for (const auto& pair : pairs) {
        if (pair.value == value) {
            return QString::fromLatin1(pair.name);
        }
    }
    return QString::number(static_cast<int>(value));
}

template <typename Enum, int N>
bool enumFromString(const QString& text, const EnumName<Enum> (&pairs)[N], Enum* outValue)
{
    const QString normalized = text.trimmed();

    for (const auto& pair : pairs) {
        if (normalized.compare(QLatin1String(pair.name), Qt::CaseInsensitive) == 0) {
            *outValue = pair.value;
            return true;
        }
    }

    bool ok = false;
    const int numericValue = normalized.toInt(&ok);
    if (ok) {
        *outValue = static_cast<Enum>(numericValue);
        return true;
    }

    return false;
}

QString colorToString(const QColor& color)
{
    return color.name(QColor::HexArgb);
}

QColor colorFromValue(const QJsonValue& value)
{
    if (!value.isString()) {
        return QColor();
    }

    const QColor color(value.toString());
    return color;
}

QJsonObject fontToJsonObject(const QFont& font)
{
    QJsonObject object;
    object.insert(QStringLiteral("family"), font.family());
    object.insert(QStringLiteral("pointSize"), font.pointSizeF());
    object.insert(QStringLiteral("weight"), font.weight());
    object.insert(QStringLiteral("italic"), font.italic());
    object.insert(QStringLiteral("underline"), font.underline());
    object.insert(QStringLiteral("strikeOut"), font.strikeOut());
    object.insert(QStringLiteral("kerning"), font.kerning());
    return object;
}

QFont fontFromJsonObject(const QJsonObject& object)
{
    QFont font;

    if (object.contains(QStringLiteral("family"))) {
        font.setFamily(object.value(QStringLiteral("family")).toString());
    }
    if (object.contains(QStringLiteral("pointSize"))) {
        font.setPointSizeF(object.value(QStringLiteral("pointSize")).toDouble(font.pointSizeF()));
    }
    if (object.contains(QStringLiteral("weight"))) {
        font.setWeight(object.value(QStringLiteral("weight")).toInt(font.weight()));
    }
    if (object.contains(QStringLiteral("italic"))) {
        font.setItalic(object.value(QStringLiteral("italic")).toBool(font.italic()));
    }
    if (object.contains(QStringLiteral("underline"))) {
        font.setUnderline(object.value(QStringLiteral("underline")).toBool(font.underline()));
    }
    if (object.contains(QStringLiteral("strikeOut"))) {
        font.setStrikeOut(object.value(QStringLiteral("strikeOut")).toBool(font.strikeOut()));
    }
    if (object.contains(QStringLiteral("kerning"))) {
        font.setKerning(object.value(QStringLiteral("kerning")).toBool(font.kerning()));
    }

    return font;
}

QJsonObject optionsToJsonObject(const ThemeOptions& options)
{
    QJsonObject object;
    object.insert(QStringLiteral("sourceColor"), colorToString(options.sourceColor));
    object.insert(QStringLiteral("mode"), enumToString(options.mode, kThemeModes));
    object.insert(QStringLiteral("contrast"), enumToString(options.contrast, kContrastModes));
    object.insert(QStringLiteral("expressive"), options.expressive);
    object.insert(QStringLiteral("useMaterialColorUtilities"), options.useMaterialColorUtilities);
    return object;
}

ThemeOptions optionsFromJsonObject(const QJsonObject& object,
                                   bool* ok,
                                   QString* errorString)
{
    ThemeOptions options;

    if (object.isEmpty()) {
        if (ok) {
            *ok = true;
        }
        return options;
    }

    if (object.contains(QStringLiteral("sourceColor"))) {
        const QColor parsed = colorFromValue(object.value(QStringLiteral("sourceColor")));
        if (!parsed.isValid()) {
            if (ok) {
                *ok = false;
            }
            if (errorString) {
                *errorString = QStringLiteral("Invalid options.sourceColor value.");
            }
            return ThemeOptions();
        }
        options.sourceColor = parsed;
    }

    if (object.contains(QStringLiteral("mode"))) {
        ThemeMode mode = ThemeMode::Light;
        if (!enumFromString(object.value(QStringLiteral("mode")).toString(), kThemeModes, &mode)) {
            if (ok) {
                *ok = false;
            }
            if (errorString) {
                *errorString = QStringLiteral("Invalid options.mode value.");
            }
            return ThemeOptions();
        }
        options.mode = mode;
    }

    if (object.contains(QStringLiteral("contrast"))) {
        ContrastMode contrast = ContrastMode::Standard;
        if (!enumFromString(object.value(QStringLiteral("contrast")).toString(), kContrastModes, &contrast)) {
            if (ok) {
                *ok = false;
            }
            if (errorString) {
                *errorString = QStringLiteral("Invalid options.contrast value.");
            }
            return ThemeOptions();
        }
        options.contrast = contrast;
    }

    if (object.contains(QStringLiteral("expressive"))) {
        options.expressive = object.value(QStringLiteral("expressive")).toBool(options.expressive);
    }

    if (object.contains(QStringLiteral("useMaterialColorUtilities"))) {
        options.useMaterialColorUtilities =
            object.value(QStringLiteral("useMaterialColorUtilities")).toBool(options.useMaterialColorUtilities);
    }

    if (ok) {
        *ok = true;
    }
    return options;
}

QJsonObject colorSchemeToJsonObject(const ColorScheme& scheme)
{
    QJsonObject object;
    for (const auto& pair : kColorRoles) {
        if (scheme.contains(pair.value)) {
            object.insert(QString::fromLatin1(pair.name), colorToString(scheme.color(pair.value)));
        }
    }
    return object;
}

void applyColorSchemeFromJsonObject(const QJsonObject& object, Theme* theme)
{
    auto& scheme = theme->colorScheme();
    for (const auto& pair : kColorRoles) {
        if (!object.contains(QString::fromLatin1(pair.name))) {
            continue;
        }

        const QColor parsed = colorFromValue(object.value(QString::fromLatin1(pair.name)));
        if (parsed.isValid()) {
            scheme.setColor(pair.value, parsed);
        }
    }
}

QJsonObject typographyToJsonObject(const TypographyScale& typography)
{
    QJsonObject object;
    for (const auto& pair : kTypeRoles) {
        if (!typography.contains(pair.value)) {
            continue;
        }

        const TypographyStyle style = typography.style(pair.value);
        QJsonObject styleObject;
        styleObject.insert(QStringLiteral("font"), fontToJsonObject(style.font));
        styleObject.insert(QStringLiteral("lineHeight"), style.lineHeight);
        styleObject.insert(QStringLiteral("letterSpacing"), style.letterSpacing);
        object.insert(QString::fromLatin1(pair.name), styleObject);
    }
    return object;
}

void applyTypographyFromJsonObject(const QJsonObject& object, Theme* theme)
{
    auto& typography = theme->typography();
    for (const auto& pair : kTypeRoles) {
        const QString key = QString::fromLatin1(pair.name);
        if (!object.contains(key) || !object.value(key).isObject()) {
            continue;
        }

        const QJsonObject styleObject = object.value(key).toObject();
        TypographyStyle style = typography.style(pair.value);

        if (styleObject.contains(QStringLiteral("font")) && styleObject.value(QStringLiteral("font")).isObject()) {
            style.font = fontFromJsonObject(styleObject.value(QStringLiteral("font")).toObject());
        }
        if (styleObject.contains(QStringLiteral("lineHeight"))) {
            style.lineHeight = styleObject.value(QStringLiteral("lineHeight")).toDouble(style.lineHeight);
        }
        if (styleObject.contains(QStringLiteral("letterSpacing"))) {
            style.letterSpacing = styleObject.value(QStringLiteral("letterSpacing")).toDouble(style.letterSpacing);
        }

        typography.setStyle(pair.value, style);
    }
}

QJsonObject shapesToJsonObject(const ShapeScale& shapes)
{
    QJsonObject object;
    for (const auto& pair : kShapeRoles) {
        if (shapes.contains(pair.value)) {
            object.insert(QString::fromLatin1(pair.name), shapes.radius(pair.value));
        }
    }
    return object;
}

void applyShapesFromJsonObject(const QJsonObject& object, Theme* theme)
{
    auto& shapes = theme->shapes();
    for (const auto& pair : kShapeRoles) {
        const QString key = QString::fromLatin1(pair.name);
        if (object.contains(key)) {
            shapes.setRadius(pair.value, object.value(key).toInt(shapes.radius(pair.value)));
        }
    }
}

QJsonObject elevationsToJsonObject(const ElevationScale& elevations)
{
    QJsonObject object;
    for (const auto& pair : kElevationRoles) {
        if (!elevations.contains(pair.value)) {
            continue;
        }

        const ElevationStyle style = elevations.style(pair.value);
        QJsonObject styleObject;
        styleObject.insert(QStringLiteral("shadowBlur"), style.shadowBlur);
        styleObject.insert(QStringLiteral("shadowYOffset"), style.shadowYOffset);
        styleObject.insert(QStringLiteral("tonalOverlayOpacity"), style.tonalOverlayOpacity);
        object.insert(QString::fromLatin1(pair.name), styleObject);
    }
    return object;
}

void applyElevationsFromJsonObject(const QJsonObject& object, Theme* theme)
{
    auto& elevations = theme->elevations();
    for (const auto& pair : kElevationRoles) {
        const QString key = QString::fromLatin1(pair.name);
        if (!object.contains(key) || !object.value(key).isObject()) {
            continue;
        }

        ElevationStyle style = elevations.style(pair.value);
        const QJsonObject styleObject = object.value(key).toObject();
        if (styleObject.contains(QStringLiteral("shadowBlur"))) {
            style.shadowBlur = styleObject.value(QStringLiteral("shadowBlur")).toInt(style.shadowBlur);
        }
        if (styleObject.contains(QStringLiteral("shadowYOffset"))) {
            style.shadowYOffset = styleObject.value(QStringLiteral("shadowYOffset")).toInt(style.shadowYOffset);
        }
        if (styleObject.contains(QStringLiteral("tonalOverlayOpacity"))) {
            style.tonalOverlayOpacity =
                styleObject.value(QStringLiteral("tonalOverlayOpacity")).toDouble(style.tonalOverlayOpacity);
        }
        elevations.setStyle(pair.value, style);
    }
}

QJsonObject motionToJsonObject(const MotionTokens& motion)
{
    QJsonObject object;
    for (const auto& pair : kMotionTokens) {
        if (!motion.contains(pair.value)) {
            continue;
        }

        const MotionStyle style = motion.style(pair.value);
        QJsonObject styleObject;
        styleObject.insert(QStringLiteral("durationMs"), style.durationMs);
        styleObject.insert(QStringLiteral("easingType"), static_cast<int>(style.easing.type()));
        object.insert(QString::fromLatin1(pair.name), styleObject);
    }
    return object;
}

void applyMotionFromJsonObject(const QJsonObject& object, Theme* theme)
{
    auto& motion = theme->motion();
    for (const auto& pair : kMotionTokens) {
        const QString key = QString::fromLatin1(pair.name);
        if (!object.contains(key) || !object.value(key).isObject()) {
            continue;
        }

        MotionStyle style = motion.style(pair.value);
        const QJsonObject styleObject = object.value(key).toObject();
        if (styleObject.contains(QStringLiteral("durationMs"))) {
            style.durationMs = styleObject.value(QStringLiteral("durationMs")).toInt(style.durationMs);
        }
        if (styleObject.contains(QStringLiteral("easingType"))) {
            style.easing = QEasingCurve(
                static_cast<QEasingCurve::Type>(styleObject.value(QStringLiteral("easingType")).toInt(
                    static_cast<int>(style.easing.type()))));
        }
        motion.setStyle(pair.value, style);
    }
}

QJsonObject stateLayerToJsonObject(const StateLayer& stateLayer)
{
    QJsonObject object;
    object.insert(QStringLiteral("color"), colorToString(stateLayer.color));
    object.insert(QStringLiteral("hoverOpacity"), stateLayer.hoverOpacity);
    object.insert(QStringLiteral("focusOpacity"), stateLayer.focusOpacity);
    object.insert(QStringLiteral("pressOpacity"), stateLayer.pressOpacity);
    object.insert(QStringLiteral("dragOpacity"), stateLayer.dragOpacity);
    return object;
}

void applyStateLayerFromJsonObject(const QJsonObject& object, Theme* theme)
{
    auto& stateLayer = theme->stateLayer();

    if (object.contains(QStringLiteral("color"))) {
        const QColor parsed = colorFromValue(object.value(QStringLiteral("color")));
        if (parsed.isValid()) {
            stateLayer.color = parsed;
        }
    }

    if (object.contains(QStringLiteral("hoverOpacity"))) {
        stateLayer.hoverOpacity = object.value(QStringLiteral("hoverOpacity")).toDouble(stateLayer.hoverOpacity);
    }
    if (object.contains(QStringLiteral("focusOpacity"))) {
        stateLayer.focusOpacity = object.value(QStringLiteral("focusOpacity")).toDouble(stateLayer.focusOpacity);
    }
    if (object.contains(QStringLiteral("pressOpacity"))) {
        stateLayer.pressOpacity = object.value(QStringLiteral("pressOpacity")).toDouble(stateLayer.pressOpacity);
    }
    if (object.contains(QStringLiteral("dragOpacity"))) {
        stateLayer.dragOpacity = object.value(QStringLiteral("dragOpacity")).toDouble(stateLayer.dragOpacity);
    }
}

} // namespace

QJsonObject ThemeSerializer::toJsonObject(const Theme& theme)
{
    QJsonObject object;
    object.insert(QStringLiteral("formatVersion"), kCurrentFormatVersion);
    object.insert(QStringLiteral("options"), optionsToJsonObject(theme.options()));
    object.insert(QStringLiteral("colorScheme"), colorSchemeToJsonObject(theme.colorScheme()));
    object.insert(QStringLiteral("typography"), typographyToJsonObject(theme.typography()));
    object.insert(QStringLiteral("shapes"), shapesToJsonObject(theme.shapes()));
    object.insert(QStringLiteral("elevations"), elevationsToJsonObject(theme.elevations()));
    object.insert(QStringLiteral("motion"), motionToJsonObject(theme.motion()));
    object.insert(QStringLiteral("stateLayer"), stateLayerToJsonObject(theme.stateLayer()));
    return object;
}

QJsonDocument ThemeSerializer::toJsonDocument(const Theme& theme)
{
    return QJsonDocument(toJsonObject(theme));
}

QByteArray ThemeSerializer::toJson(const Theme& theme, QJsonDocument::JsonFormat format)
{
    return toJsonDocument(theme).toJson(format);
}

bool ThemeSerializer::writeToFile(const Theme& theme,
                                  const QString& filePath,
                                  QString* errorString,
                                  QJsonDocument::JsonFormat format)
{
    QSaveFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        if (errorString) {
            *errorString = file.errorString();
        }
        return false;
    }

    const QByteArray data = toJson(theme, format);
    if (file.write(data) != data.size()) {
        if (errorString) {
            *errorString = file.errorString();
        }
        return false;
    }

    if (!file.commit()) {
        if (errorString) {
            *errorString = file.errorString();
        }
        return false;
    }

    return true;
}

Theme ThemeSerializer::fromJsonObject(const QJsonObject& object, bool* ok, QString* errorString)
{
    if (ok) {
        *ok = false;
    }

    if (object.isEmpty()) {
        if (errorString) {
            *errorString = QStringLiteral("Theme JSON object is empty.");
        }
        return Theme();
    }

    bool optionsOk = false;
    const ThemeOptions options =
        optionsFromJsonObject(object.value(QStringLiteral("options")).toObject(), &optionsOk, errorString);
    if (!optionsOk) {
        return Theme();
    }

    ThemeBuilder builder;
    Theme theme = builder.build(options);

    if (object.contains(QStringLiteral("colorScheme")) && object.value(QStringLiteral("colorScheme")).isObject()) {
        applyColorSchemeFromJsonObject(object.value(QStringLiteral("colorScheme")).toObject(), &theme);
    }
    if (object.contains(QStringLiteral("typography")) && object.value(QStringLiteral("typography")).isObject()) {
        applyTypographyFromJsonObject(object.value(QStringLiteral("typography")).toObject(), &theme);
    }
    if (object.contains(QStringLiteral("shapes")) && object.value(QStringLiteral("shapes")).isObject()) {
        applyShapesFromJsonObject(object.value(QStringLiteral("shapes")).toObject(), &theme);
    }
    if (object.contains(QStringLiteral("elevations")) && object.value(QStringLiteral("elevations")).isObject()) {
        applyElevationsFromJsonObject(object.value(QStringLiteral("elevations")).toObject(), &theme);
    }
    if (object.contains(QStringLiteral("motion")) && object.value(QStringLiteral("motion")).isObject()) {
        applyMotionFromJsonObject(object.value(QStringLiteral("motion")).toObject(), &theme);
    }
    if (object.contains(QStringLiteral("stateLayer")) && object.value(QStringLiteral("stateLayer")).isObject()) {
        applyStateLayerFromJsonObject(object.value(QStringLiteral("stateLayer")).toObject(), &theme);
    }

    if (ok) {
        *ok = true;
    }
    if (errorString) {
        errorString->clear();
    }
    return theme;
}

Theme ThemeSerializer::fromJsonDocument(const QJsonDocument& document, bool* ok, QString* errorString)
{
    if (!document.isObject()) {
        if (ok) {
            *ok = false;
        }
        if (errorString) {
            *errorString = QStringLiteral("Theme JSON root must be an object.");
        }
        return Theme();
    }

    return fromJsonObject(document.object(), ok, errorString);
}

Theme ThemeSerializer::fromJson(const QByteArray& json, bool* ok, QString* errorString)
{
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(json, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        if (ok) {
            *ok = false;
        }
        if (errorString) {
            *errorString = parseError.errorString();
        }
        return Theme();
    }

    return fromJsonDocument(document, ok, errorString);
}

bool ThemeSerializer::readFromFile(const QString& filePath, Theme* outTheme, QString* errorString)
{
    if (!outTheme) {
        if (errorString) {
            *errorString = QStringLiteral("Output theme pointer is null.");
        }
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        if (errorString) {
            *errorString = file.errorString();
        }
        return false;
    }

    bool ok = false;
    const Theme theme = fromJson(file.readAll(), &ok, errorString);
    if (!ok) {
        return false;
    }

    *outTheme = theme;
    return true;
}

} // namespace QtMaterial
