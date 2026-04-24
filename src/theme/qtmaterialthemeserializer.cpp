#include "qtmaterial/theme/qtmaterialthemeserializer.h"

#include <QFile>
#include <QFont>
#include <QJsonArray>
#include <QJsonParseError>

#include <array>
#include <utility>

namespace QtMaterial {
namespace {

template <typename Enum>
using EnumNamePair = std::pair<Enum, const char*>;

template <typename Enum, std::size_t N>
QString enumToString(Enum value, const std::array<EnumNamePair<Enum>, N>& map)
{
    for (const auto& [candidate, name] : map) {
        if (candidate == value) {
            return QString::fromLatin1(name);
        }
    }
    return QString();
}

template <typename Enum, std::size_t N>
bool stringToEnum(const QString& text,
                  const std::array<EnumNamePair<Enum>, N>& map,
                  Enum* outValue)
{
    for (const auto& [candidate, name] : map) {
        if (text == QLatin1String(name)) {
            if (outValue) {
                *outValue = candidate;
            }
            return true;
        }
    }
    return false;
}

bool fail(bool* ok, QString* errorString, const QString& message)
{
    if (ok) {
        *ok = false;
    }
    if (errorString) {
        *errorString = message;
    }
    return false;
}

void succeed(bool* ok, QString* errorString)
{
    if (ok) {
        *ok = true;
    }
    if (errorString) {
        errorString->clear();
    }
}

QString colorToString(const QColor& color)
{
    return color.name(QColor::HexArgb);
}

bool jsonToColor(const QJsonValue& value, QColor* outColor)
{
    if (!value.isString()) {
        return false;
    }

    const QColor color(value.toString());
    if (!color.isValid()) {
        return false;
    }

    if (outColor) {
        *outColor = color;
    }
    return true;
}

const std::array kThemeModes = {
    EnumNamePair<ThemeMode>{ ThemeMode::Light, "Light" },
    EnumNamePair<ThemeMode>{ ThemeMode::Dark, "Dark" },
};

const std::array kContrastModes = {
    EnumNamePair<ContrastMode>{ ContrastMode::Standard, "Standard" },
    EnumNamePair<ContrastMode>{ ContrastMode::Medium, "Medium" },
    EnumNamePair<ContrastMode>{ ContrastMode::High, "High" },
};

const std::array kColorRoles = {
    EnumNamePair<ColorRole>{ ColorRole::Primary, "Primary" },
    EnumNamePair<ColorRole>{ ColorRole::OnPrimary, "OnPrimary" },
    EnumNamePair<ColorRole>{ ColorRole::PrimaryContainer, "PrimaryContainer" },
    EnumNamePair<ColorRole>{ ColorRole::OnPrimaryContainer, "OnPrimaryContainer" },
    EnumNamePair<ColorRole>{ ColorRole::Secondary, "Secondary" },
    EnumNamePair<ColorRole>{ ColorRole::OnSecondary, "OnSecondary" },
    EnumNamePair<ColorRole>{ ColorRole::SecondaryContainer, "SecondaryContainer" },
    EnumNamePair<ColorRole>{ ColorRole::OnSecondaryContainer, "OnSecondaryContainer" },
    EnumNamePair<ColorRole>{ ColorRole::Tertiary, "Tertiary" },
    EnumNamePair<ColorRole>{ ColorRole::OnTertiary, "OnTertiary" },
    EnumNamePair<ColorRole>{ ColorRole::TertiaryContainer, "TertiaryContainer" },
    EnumNamePair<ColorRole>{ ColorRole::OnTertiaryContainer, "OnTertiaryContainer" },
    EnumNamePair<ColorRole>{ ColorRole::Error, "Error" },
    EnumNamePair<ColorRole>{ ColorRole::OnError, "OnError" },
    EnumNamePair<ColorRole>{ ColorRole::ErrorContainer, "ErrorContainer" },
    EnumNamePair<ColorRole>{ ColorRole::OnErrorContainer, "OnErrorContainer" },
    EnumNamePair<ColorRole>{ ColorRole::Background, "Background" },
    EnumNamePair<ColorRole>{ ColorRole::OnBackground, "OnBackground" },
    EnumNamePair<ColorRole>{ ColorRole::Surface, "Surface" },
    EnumNamePair<ColorRole>{ ColorRole::OnSurface, "OnSurface" },
    EnumNamePair<ColorRole>{ ColorRole::SurfaceDim, "SurfaceDim" },
    EnumNamePair<ColorRole>{ ColorRole::SurfaceBright, "SurfaceBright" },
    EnumNamePair<ColorRole>{ ColorRole::SurfaceContainerLowest, "SurfaceContainerLowest" },
    EnumNamePair<ColorRole>{ ColorRole::SurfaceContainerLow, "SurfaceContainerLow" },
    EnumNamePair<ColorRole>{ ColorRole::SurfaceContainer, "SurfaceContainer" },
    EnumNamePair<ColorRole>{ ColorRole::SurfaceContainerHigh, "SurfaceContainerHigh" },
    EnumNamePair<ColorRole>{ ColorRole::SurfaceContainerHighest, "SurfaceContainerHighest" },
    EnumNamePair<ColorRole>{ ColorRole::SurfaceVariant, "SurfaceVariant" },
    EnumNamePair<ColorRole>{ ColorRole::OnSurfaceVariant, "OnSurfaceVariant" },
    EnumNamePair<ColorRole>{ ColorRole::Outline, "Outline" },
    EnumNamePair<ColorRole>{ ColorRole::OutlineVariant, "OutlineVariant" },
    EnumNamePair<ColorRole>{ ColorRole::InverseSurface, "InverseSurface" },
    EnumNamePair<ColorRole>{ ColorRole::InverseOnSurface, "InverseOnSurface" },
    EnumNamePair<ColorRole>{ ColorRole::InversePrimary, "InversePrimary" },
    EnumNamePair<ColorRole>{ ColorRole::Shadow, "Shadow" },
    EnumNamePair<ColorRole>{ ColorRole::Scrim, "Scrim" },
};

const std::array kTypeRoles = {
    EnumNamePair<TypeRole>{ TypeRole::DisplayLarge, "DisplayLarge" },
    EnumNamePair<TypeRole>{ TypeRole::DisplayMedium, "DisplayMedium" },
    EnumNamePair<TypeRole>{ TypeRole::DisplaySmall, "DisplaySmall" },
    EnumNamePair<TypeRole>{ TypeRole::HeadlineLarge, "HeadlineLarge" },
    EnumNamePair<TypeRole>{ TypeRole::HeadlineMedium, "HeadlineMedium" },
    EnumNamePair<TypeRole>{ TypeRole::HeadlineSmall, "HeadlineSmall" },
    EnumNamePair<TypeRole>{ TypeRole::TitleLarge, "TitleLarge" },
    EnumNamePair<TypeRole>{ TypeRole::TitleMedium, "TitleMedium" },
    EnumNamePair<TypeRole>{ TypeRole::TitleSmall, "TitleSmall" },
    EnumNamePair<TypeRole>{ TypeRole::BodyLarge, "BodyLarge" },
    EnumNamePair<TypeRole>{ TypeRole::BodyMedium, "BodyMedium" },
    EnumNamePair<TypeRole>{ TypeRole::BodySmall, "BodySmall" },
    EnumNamePair<TypeRole>{ TypeRole::LabelLarge, "LabelLarge" },
    EnumNamePair<TypeRole>{ TypeRole::LabelMedium, "LabelMedium" },
    EnumNamePair<TypeRole>{ TypeRole::LabelSmall, "LabelSmall" },
};

const std::array kShapeRoles = {
    EnumNamePair<ShapeRole>{ ShapeRole::None, "None" },
    EnumNamePair<ShapeRole>{ ShapeRole::ExtraSmall, "ExtraSmall" },
    EnumNamePair<ShapeRole>{ ShapeRole::Small, "Small" },
    EnumNamePair<ShapeRole>{ ShapeRole::Medium, "Medium" },
    EnumNamePair<ShapeRole>{ ShapeRole::Large, "Large" },
    EnumNamePair<ShapeRole>{ ShapeRole::ExtraLarge, "ExtraLarge" },
    EnumNamePair<ShapeRole>{ ShapeRole::Full, "Full" },
};

const std::array kElevationRoles = {
    EnumNamePair<ElevationRole>{ ElevationRole::Level0, "Level0" },
    EnumNamePair<ElevationRole>{ ElevationRole::Level1, "Level1" },
    EnumNamePair<ElevationRole>{ ElevationRole::Level2, "Level2" },
    EnumNamePair<ElevationRole>{ ElevationRole::Level3, "Level3" },
    EnumNamePair<ElevationRole>{ ElevationRole::Level4, "Level4" },
    EnumNamePair<ElevationRole>{ ElevationRole::Level5, "Level5" },
};

const std::array kMotionTokens = {
    EnumNamePair<MotionToken>{ MotionToken::Short1, "Short1" },
    EnumNamePair<MotionToken>{ MotionToken::Short2, "Short2" },
    EnumNamePair<MotionToken>{ MotionToken::Short3, "Short3" },
    EnumNamePair<MotionToken>{ MotionToken::Short4, "Short4" },
    EnumNamePair<MotionToken>{ MotionToken::Medium1, "Medium1" },
    EnumNamePair<MotionToken>{ MotionToken::Medium2, "Medium2" },
    EnumNamePair<MotionToken>{ MotionToken::Medium3, "Medium3" },
    EnumNamePair<MotionToken>{ MotionToken::Medium4, "Medium4" },
    EnumNamePair<MotionToken>{ MotionToken::Long1, "Long1" },
    EnumNamePair<MotionToken>{ MotionToken::Long2, "Long2" },
    EnumNamePair<MotionToken>{ MotionToken::Long3, "Long3" },
    EnumNamePair<MotionToken>{ MotionToken::Long4, "Long4" },
};

QJsonObject fontToJson(const QFont& font)
{
    QJsonObject object;
    object.insert(QStringLiteral("family"), font.family());
    object.insert(QStringLiteral("pointSize"), font.pointSize());
    object.insert(QStringLiteral("pixelSize"), font.pixelSize());
    object.insert(QStringLiteral("weight"), font.weight());
    object.insert(QStringLiteral("italic"), font.italic());
    object.insert(QStringLiteral("underline"), font.underline());
    object.insert(QStringLiteral("strikeOut"), font.strikeOut());
    object.insert(QStringLiteral("kerning"), font.kerning());
    return object;
}

QFont fontFromJson(const QJsonObject& object)
{
    QFont font;

    if (object.contains(QStringLiteral("family"))) {
        font.setFamily(object.value(QStringLiteral("family")).toString());
    }

    const int pointSize = object.value(QStringLiteral("pointSize")).toInt(-1);
    if (pointSize > 0) {
        font.setPointSize(pointSize);
    }

    const int pixelSize = object.value(QStringLiteral("pixelSize")).toInt(-1);
    if (pixelSize > 0) {
        font.setPixelSize(pixelSize);
    }

    font.setWeight(object.value(QStringLiteral("weight")).toInt(QFont::Normal));
    font.setItalic(object.value(QStringLiteral("italic")).toBool(false));
    font.setUnderline(object.value(QStringLiteral("underline")).toBool(false));
    font.setStrikeOut(object.value(QStringLiteral("strikeOut")).toBool(false));
    font.setKerning(object.value(QStringLiteral("kerning")).toBool(true));

    return font;
}

QJsonObject optionsToJson(const ThemeOptions& options)
{
    QJsonObject object;
    object.insert(QStringLiteral("sourceColor"), colorToString(options.sourceColor));
    object.insert(QStringLiteral("mode"), enumToString(options.mode, kThemeModes));
    object.insert(QStringLiteral("contrast"), enumToString(options.contrast, kContrastModes));
    // Persisted even if not currently consumed by ThemeBuilder.
    // This keeps the snapshot format forward-compatible with future theme-generation backends.    
    object.insert(QStringLiteral("expressive"), options.expressive);
    object.insert(QStringLiteral("useMaterialColorUtilities"), options.useMaterialColorUtilities);
    return object;
}

bool optionsFromJson(const QJsonObject& object,
                     ThemeOptions* outOptions,
                     QString* errorString)
{
    if (!outOptions) {
        if (errorString) {
            *errorString = QStringLiteral("Output ThemeOptions pointer is null.");
        }
        return false;
    }

    ThemeOptions options;

    if (object.contains(QStringLiteral("sourceColor"))) {
        QColor sourceColor;
        if (!jsonToColor(object.value(QStringLiteral("sourceColor")), &sourceColor)) {
            if (errorString) {
                *errorString = QStringLiteral("Invalid options.sourceColor value.");
            }
            return false;
        }
        options.sourceColor = sourceColor;
    }

    if (object.contains(QStringLiteral("mode"))) {
        ThemeMode mode = ThemeMode::Light;
        if (!stringToEnum(object.value(QStringLiteral("mode")).toString(), kThemeModes, &mode)) {
            if (errorString) {
                *errorString = QStringLiteral("Invalid options.mode value.");
            }
            return false;
        }
        options.mode = mode;
    }

    if (object.contains(QStringLiteral("contrast"))) {
        ContrastMode contrast = ContrastMode::Standard;
        if (!stringToEnum(object.value(QStringLiteral("contrast")).toString(), kContrastModes, &contrast)) {
            if (errorString) {
                *errorString = QStringLiteral("Invalid options.contrast value.");
            }
            return false;
        }
        options.contrast = contrast;
    }

    if (object.contains(QStringLiteral("expressive"))) {
        options.expressive = object.value(QStringLiteral("expressive")).toBool(options.expressive);
    }

    if (object.contains(QStringLiteral("useMaterialColorUtilities"))) {
        options.useMaterialColorUtilities = object.value(QStringLiteral("useMaterialColorUtilities")).toBool(options.useMaterialColorUtilities);
    }

    *outOptions = options;
    return true;
}

QJsonObject colorSchemeToJson(const ColorScheme& scheme)
{
    QJsonObject object;
    for (const auto& [role, name] : kColorRoles) {
        if (scheme.contains(role)) {
            object.insert(QString::fromLatin1(name), colorToString(scheme.color(role)));
        }
    }
    return object;
}

bool colorSchemeFromJson(const QJsonObject& object,
                         ColorScheme* outScheme,
                         QString* errorString)
{
    if (!outScheme) {
        if (errorString) {
            *errorString = QStringLiteral("Output ColorScheme pointer is null.");
        }
        return false;
    }

    ColorScheme scheme;
    for (const auto& [role, name] : kColorRoles) {
        const QString key = QString::fromLatin1(name);
        if (!object.contains(key)) {
            continue;
        }

        QColor color;
        if (!jsonToColor(object.value(key), &color)) {
            if (errorString) {
                *errorString = QStringLiteral("Invalid color for role '%1'.").arg(key);
            }
            return false;
        }
        scheme.setColor(role, color);
    }

    *outScheme = scheme;
    return true;
}

QJsonObject typographyToJson(const TypographyScale& typography)
{
    QJsonObject object;
    for (const auto& [role, name] : kTypeRoles) {
        if (!typography.contains(role)) {
            continue;
        }

        const TypographyStyle style = typography.style(role);
        QJsonObject styleObject;
        styleObject.insert(QStringLiteral("font"), fontToJson(style.font));
        styleObject.insert(QStringLiteral("lineHeight"), style.lineHeight);
        styleObject.insert(QStringLiteral("letterSpacing"), style.letterSpacing);
        object.insert(QString::fromLatin1(name), styleObject);
    }
    return object;
}

bool typographyFromJson(const QJsonObject& object,
                        TypographyScale* outTypography,
                        QString* errorString)
{
    if (!outTypography) {
        if (errorString) {
            *errorString = QStringLiteral("Output TypographyScale pointer is null.");
        }
        return false;
    }

    TypographyScale typography;
    for (const auto& [role, name] : kTypeRoles) {
        const QString key = QString::fromLatin1(name);
        if (!object.contains(key)) {
            continue;
        }

        const QJsonValue value = object.value(key);
        if (!value.isObject()) {
            if (errorString) {
                *errorString = QStringLiteral("Typography entry '%1' must be an object.").arg(key);
            }
            return false;
        }

        const QJsonObject styleObject = value.toObject();
        TypographyStyle style;
        style.font = fontFromJson(styleObject.value(QStringLiteral("font")).toObject());
        style.lineHeight = styleObject.value(QStringLiteral("lineHeight")).toDouble(0.0);
        style.letterSpacing = styleObject.value(QStringLiteral("letterSpacing")).toDouble(0.0);
        typography.setStyle(role, style);
    }

    *outTypography = typography;
    return true;
}

QJsonObject shapesToJson(const ShapeScale& shapes)
{
    QJsonObject object;
    for (const auto& [role, name] : kShapeRoles) {
        if (shapes.contains(role)) {
            object.insert(QString::fromLatin1(name), shapes.radius(role));
        }
    }
    return object;
}

bool shapesFromJson(const QJsonObject& object,
                    ShapeScale* outShapes,
                    QString* errorString)
{
    if (!outShapes) {
        if (errorString) {
            *errorString = QStringLiteral("Output ShapeScale pointer is null.");
        }
        return false;
    }

    ShapeScale shapes;
    for (const auto& [role, name] : kShapeRoles) {
        const QString key = QString::fromLatin1(name);
        if (!object.contains(key)) {
            continue;
        }
        shapes.setRadius(role, object.value(key).toInt());
    }

    *outShapes = shapes;
    return true;
}

QJsonObject elevationsToJson(const ElevationScale& elevations)
{
    QJsonObject object;
    for (const auto& [role, name] : kElevationRoles) {
        if (!elevations.contains(role)) {
            continue;
        }

        const ElevationStyle style = elevations.style(role);
        QJsonObject styleObject;
        styleObject.insert(QStringLiteral("shadowBlur"), style.shadowBlur);
        styleObject.insert(QStringLiteral("shadowYOffset"), style.shadowYOffset);
        styleObject.insert(QStringLiteral("tonalOverlayOpacity"), style.tonalOverlayOpacity);
        object.insert(QString::fromLatin1(name), styleObject);
    }
    return object;
}

bool elevationsFromJson(const QJsonObject& object,
                        ElevationScale* outElevations,
                        QString* errorString)
{
    if (!outElevations) {
        if (errorString) {
            *errorString = QStringLiteral("Output ElevationScale pointer is null.");
        }
        return false;
    }

    ElevationScale elevations;
    for (const auto& [role, name] : kElevationRoles) {
        const QString key = QString::fromLatin1(name);
        if (!object.contains(key)) {
            continue;
        }

        const QJsonValue value = object.value(key);
        if (!value.isObject()) {
            if (errorString) {
                *errorString = QStringLiteral("Elevation entry '%1' must be an object.").arg(key);
            }
            return false;
        }

        const QJsonObject styleObject = value.toObject();
        ElevationStyle style;
        style.shadowBlur = styleObject.value(QStringLiteral("shadowBlur")).toInt();
        style.shadowYOffset = styleObject.value(QStringLiteral("shadowYOffset")).toInt();
        style.tonalOverlayOpacity = styleObject.value(QStringLiteral("tonalOverlayOpacity")).toDouble(0.0);
        elevations.setStyle(role, style);
    }

    *outElevations = elevations;
    return true;
}

QJsonObject motionToJson(const MotionTokens& motion)
{
    QJsonObject object;
    for (const auto& [token, name] : kMotionTokens) {
        if (!motion.contains(token)) {
            continue;
        }

        const MotionStyle style = motion.style(token);
        QJsonObject styleObject;
        styleObject.insert(QStringLiteral("durationMs"), style.durationMs);
        styleObject.insert(QStringLiteral("easingType"), static_cast<int>(style.easing.type()));
        object.insert(QString::fromLatin1(name), styleObject);
    }
    return object;
}

bool motionFromJson(const QJsonObject& object,
                    MotionTokens* outMotion,
                    QString* errorString)
{
    if (!outMotion) {
        if (errorString) {
            *errorString = QStringLiteral("Output MotionTokens pointer is null.");
        }
        return false;
    }

    MotionTokens motion;
    for (const auto& [token, name] : kMotionTokens) {
        const QString key = QString::fromLatin1(name);
        if (!object.contains(key)) {
            continue;
        }

        const QJsonValue value = object.value(key);
        if (!value.isObject()) {
            if (errorString) {
                *errorString = QStringLiteral("Motion entry '%1' must be an object.").arg(key);
            }
            return false;
        }

        const QJsonObject styleObject = value.toObject();
        MotionStyle style;
        style.durationMs = styleObject.value(QStringLiteral("durationMs")).toInt();
        style.easing = QEasingCurve(static_cast<QEasingCurve::Type>(
            styleObject.value(QStringLiteral("easingType")).toInt(static_cast<int>(QEasingCurve::Linear))));
        motion.setStyle(token, style);
    }

    *outMotion = motion;
    return true;
}

QJsonObject stateLayerToJson(const StateLayer& stateLayer)
{
    QJsonObject object;
    object.insert(QStringLiteral("color"), colorToString(stateLayer.color));
    object.insert(QStringLiteral("hoverOpacity"), stateLayer.hoverOpacity);
    object.insert(QStringLiteral("focusOpacity"), stateLayer.focusOpacity);
    object.insert(QStringLiteral("pressOpacity"), stateLayer.pressOpacity);
    object.insert(QStringLiteral("dragOpacity"), stateLayer.dragOpacity);
    return object;
}

bool stateLayerFromJson(const QJsonObject& object,
                        StateLayer* outStateLayer,
                        QString* errorString)
{
    if (!outStateLayer) {
        if (errorString) {
            *errorString = QStringLiteral("Output StateLayer pointer is null.");
        }
        return false;
    }

    StateLayer stateLayer;

    if (object.contains(QStringLiteral("color"))) {
        QColor color;
        if (!jsonToColor(object.value(QStringLiteral("color")), &color)) {
            if (errorString) {
                *errorString = QStringLiteral("Invalid stateLayer.color value.");
            }
            return false;
        }
        stateLayer.color = color;
    }

    stateLayer.hoverOpacity = object.value(QStringLiteral("hoverOpacity")).toDouble(stateLayer.hoverOpacity);
    stateLayer.focusOpacity = object.value(QStringLiteral("focusOpacity")).toDouble(stateLayer.focusOpacity);
    stateLayer.pressOpacity = object.value(QStringLiteral("pressOpacity")).toDouble(stateLayer.pressOpacity);
    stateLayer.dragOpacity = object.value(QStringLiteral("dragOpacity")).toDouble(stateLayer.dragOpacity);

    *outStateLayer = stateLayer;
    return true;
}

} // namespace

QJsonObject ThemeSerializer::toJsonObject(const Theme& theme)
{
    QJsonObject object;
    object.insert(QStringLiteral("formatVersion"), kCurrentFormatVersion);
    object.insert(QStringLiteral("mode"), enumToString(theme.mode(), kThemeModes));
    object.insert(QStringLiteral("contrast"), enumToString(theme.contrastMode(), kContrastModes));
    object.insert(QStringLiteral("options"), optionsToJson(theme.options()));
    object.insert(QStringLiteral("colorScheme"), colorSchemeToJson(theme.colorScheme()));
    object.insert(QStringLiteral("typography"), typographyToJson(theme.typography()));
    object.insert(QStringLiteral("shapes"), shapesToJson(theme.shapes()));
    object.insert(QStringLiteral("elevations"), elevationsToJson(theme.elevations()));
    object.insert(QStringLiteral("motion"), motionToJson(theme.motion()));
    object.insert(QStringLiteral("stateLayer"), stateLayerToJson(theme.stateLayer()));
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
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (errorString) {
            *errorString = file.errorString();
        }
        return false;
    }

    const QByteArray json = toJson(theme, format);
    if (file.write(json) != json.size()) {
        if (errorString) {
            *errorString = file.errorString();
        }
        return false;
    }

    if (errorString) {
        errorString->clear();
    }
    return true;
}

Theme ThemeSerializer::fromJsonObject(const QJsonObject& object,
                                      bool* ok,
                                      QString* errorString)
{
    if (object.isEmpty()) {
        fail(ok, errorString, QStringLiteral("Theme JSON object is empty."));
        return Theme();
    }

    if (object.contains(QStringLiteral("formatVersion"))
        && object.value(QStringLiteral("formatVersion")).toInt(-1) != kCurrentFormatVersion) {
        fail(ok,
             errorString,
             QStringLiteral("Unsupported theme formatVersion. Expected %1.")
                 .arg(kCurrentFormatVersion));
        return Theme();
    }

    ThemeOptions options;
    if (object.contains(QStringLiteral("options"))) {
        const QJsonValue optionsValue = object.value(QStringLiteral("options"));
        if (!optionsValue.isObject()) {
            fail(ok, errorString, QStringLiteral("Theme options entry must be an object."));
            return Theme();
        }

        QString localError;
        if (!optionsFromJson(optionsValue.toObject(), &options, &localError)) {
            fail(ok, errorString, localError);
            return Theme();
        }
    }

    if (object.contains(QStringLiteral("mode"))) {
        ThemeMode mode = ThemeMode::Light;
        if (!stringToEnum(object.value(QStringLiteral("mode")).toString(), kThemeModes, &mode)) {
            fail(ok, errorString, QStringLiteral("Invalid top-level mode value."));
            return Theme();
        }
        options.mode = mode;
    }

    if (object.contains(QStringLiteral("contrast"))) {
        ContrastMode contrast = ContrastMode::Standard;
        if (!stringToEnum(object.value(QStringLiteral("contrast")).toString(), kContrastModes, &contrast)) {
            fail(ok, errorString, QStringLiteral("Invalid top-level contrast value."));
            return Theme();
        }
        options.contrast = contrast;
    }

    Theme theme(options);
    theme.setMode(options.mode);
    theme.setContrastMode(options.contrast);
    theme.setOptions(options);

    if (object.contains(QStringLiteral("colorScheme"))) {
        const QJsonValue value = object.value(QStringLiteral("colorScheme"));
        if (!value.isObject()) {
            fail(ok, errorString, QStringLiteral("colorScheme entry must be an object."));
            return Theme();
        }

        ColorScheme scheme;
        QString localError;
        if (!colorSchemeFromJson(value.toObject(), &scheme, &localError)) {
            fail(ok, errorString, localError);
            return Theme();
        }
        theme.colorScheme() = scheme;
    }

    if (object.contains(QStringLiteral("typography"))) {
        const QJsonValue value = object.value(QStringLiteral("typography"));
        if (!value.isObject()) {
            fail(ok, errorString, QStringLiteral("typography entry must be an object."));
            return Theme();
        }

        TypographyScale typography;
        QString localError;
        if (!typographyFromJson(value.toObject(), &typography, &localError)) {
            fail(ok, errorString, localError);
            return Theme();
        }
        theme.typography() = typography;
    }

    if (object.contains(QStringLiteral("shapes"))) {
        const QJsonValue value = object.value(QStringLiteral("shapes"));
        if (!value.isObject()) {
            fail(ok, errorString, QStringLiteral("shapes entry must be an object."));
            return Theme();
        }

        ShapeScale shapes;
        QString localError;
        if (!shapesFromJson(value.toObject(), &shapes, &localError)) {
            fail(ok, errorString, localError);
            return Theme();
        }
        theme.shapes() = shapes;
    }

    if (object.contains(QStringLiteral("elevations"))) {
        const QJsonValue value = object.value(QStringLiteral("elevations"));
        if (!value.isObject()) {
            fail(ok, errorString, QStringLiteral("elevations entry must be an object."));
            return Theme();
        }

        ElevationScale elevations;
        QString localError;
        if (!elevationsFromJson(value.toObject(), &elevations, &localError)) {
            fail(ok, errorString, localError);
            return Theme();
        }
        theme.elevations() = elevations;
    }

    if (object.contains(QStringLiteral("motion"))) {
        const QJsonValue value = object.value(QStringLiteral("motion"));
        if (!value.isObject()) {
            fail(ok, errorString, QStringLiteral("motion entry must be an object."));
            return Theme();
        }

        MotionTokens motion;
        QString localError;
        if (!motionFromJson(value.toObject(), &motion, &localError)) {
            fail(ok, errorString, localError);
            return Theme();
        }
        theme.motion() = motion;
    }

    if (object.contains(QStringLiteral("stateLayer"))) {
        const QJsonValue value = object.value(QStringLiteral("stateLayer"));
        if (!value.isObject()) {
            fail(ok, errorString, QStringLiteral("stateLayer entry must be an object."));
            return Theme();
        }

        StateLayer stateLayer;
        QString localError;
        if (!stateLayerFromJson(value.toObject(), &stateLayer, &localError)) {
            fail(ok, errorString, localError);
            return Theme();
        }
        theme.stateLayer() = stateLayer;
    }

    succeed(ok, errorString);
    return theme;
}

Theme ThemeSerializer::fromJsonDocument(const QJsonDocument& document,
                                        bool* ok,
                                        QString* errorString)
{
    if (!document.isObject()) {
        fail(ok, errorString, QStringLiteral("Theme document root must be a JSON object."));
        return Theme();
    }

    return fromJsonObject(document.object(), ok, errorString);
}

Theme ThemeSerializer::fromJson(const QByteArray& json,
                                bool* ok,
                                QString* errorString)
{
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(json, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        fail(ok, errorString, parseError.errorString());
        return Theme();
    }

    return fromJsonDocument(document, ok, errorString);
}

bool ThemeSerializer::readFromFile(const QString& filePath,
                                   Theme* outTheme,
                                   QString* errorString)
{
    if (!outTheme) {
        if (errorString) {
            *errorString = QStringLiteral("Output Theme pointer is null.");
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
    QString localError;
    const Theme theme = fromJson(file.readAll(), &ok, &localError);
    if (!ok) {
        if (errorString) {
            *errorString = localError;
        }
        return false;
    }

    *outTheme = theme;
    if (errorString) {
        errorString->clear();
    }
    return true;
}

} // namespace QtMaterial
