#include "qtmaterialcomponenttokenapplier_p.h"

#include <QColor>
#include <QVariant>
#include <QVariantMap>

namespace QtMaterial {
namespace {

void mergeOverride(ComponentTokenOverride* target, const ComponentTokenOverride& source)
{
    for (auto it = source.colors.constBegin(); it != source.colors.constEnd(); ++it) {
        target->colors.insert(it.key(), it.value());
    }
    for (auto it = source.typography.constBegin(); it != source.typography.constEnd(); ++it) {
        target->typography.insert(it.key(), it.value());
    }
    for (auto it = source.shapes.constBegin(); it != source.shapes.constEnd(); ++it) {
        target->shapes.insert(it.key(), it.value());
    }
    for (auto it = source.elevations.constBegin(); it != source.elevations.constEnd(); ++it) {
        target->elevations.insert(it.key(), it.value());
    }
    for (auto it = source.motion.constBegin(); it != source.motion.constEnd(); ++it) {
        target->motion.insert(it.key(), it.value());
    }
    for (auto it = source.density.constBegin(); it != source.density.constEnd(); ++it) {
        target->density.insert(it.key(), it.value());
    }
    for (auto it = source.iconSizes.constBegin(); it != source.iconSizes.constEnd(); ++it) {
        target->iconSizes.insert(it.key(), it.value());
    }
    if (source.hasStateLayer) {
        target->stateLayer = source.stateLayer;
        target->hasStateLayer = true;
    }
    for (auto it = source.custom.constBegin(); it != source.custom.constEnd(); ++it) {
        target->custom.insert(it.key(), it.value());
    }
}

bool readInt(const QVariantMap& custom, const char* key, int* out)
{
    const QVariant value = custom.value(QString::fromLatin1(key));
    if (!value.isValid()) {
        return false;
    }
    bool ok = false;
    const int intValue = value.toInt(&ok);
    if (!ok) {
        return false;
    }
    *out = intValue;
    return true;
}

bool readReal(const QVariantMap& custom, const char* key, qreal* out)
{
    const QVariant value = custom.value(QString::fromLatin1(key));
    if (!value.isValid()) {
        return false;
    }
    bool ok = false;
    const double realValue = value.toDouble(&ok);
    if (!ok) {
        return false;
    }
    *out = static_cast<qreal>(realValue);
    return true;
}

bool readBool(const QVariantMap& custom, const char* key, bool* out)
{
    const QVariant value = custom.value(QString::fromLatin1(key));
    if (!value.isValid()) {
        return false;
    }
    *out = value.toBool();
    return true;
}

bool readColor(const QVariantMap& custom, const char* key, QColor* out)
{
    const QVariant value = custom.value(QString::fromLatin1(key));
    if (!value.isValid()) {
        return false;
    }

    QColor color = value.value<QColor>();
    if (!color.isValid()) {
        color = QColor(value.toString());
    }
    if (!color.isValid()) {
        return false;
    }
    *out = color;
    return true;
}

bool readMargins(const QVariantMap& custom, QMargins* out)
{
    int left = out->left();
    int top = out->top();
    int right = out->right();
    int bottom = out->bottom();
    bool changed = false;
    changed = readInt(custom, "contentPaddingLeft", &left) || changed;
    changed = readInt(custom, "contentPaddingTop", &top) || changed;
    changed = readInt(custom, "contentPaddingRight", &right) || changed;
    changed = readInt(custom, "contentPaddingBottom", &bottom) || changed;
    if (changed) {
        *out = QMargins(left, top, right, bottom);
    }
    return changed;
}

ShapeRole parseShapeRole(const QString& text, ShapeRole fallback)
{
    const QString value = text.trimmed().toLower();
    if (value == QLatin1String("none")) return ShapeRole::None;
    if (value == QLatin1String("extrasmall") || value == QLatin1String("extra-small")) return ShapeRole::ExtraSmall;
    if (value == QLatin1String("small")) return ShapeRole::Small;
    if (value == QLatin1String("medium")) return ShapeRole::Medium;
    if (value == QLatin1String("large")) return ShapeRole::Large;
    if (value == QLatin1String("extralarge") || value == QLatin1String("extra-large")) return ShapeRole::ExtraLarge;
    if (value == QLatin1String("full")) return ShapeRole::Full;
    return fallback;
}

ElevationRole parseElevationRole(const QString& text, ElevationRole fallback)
{
    const QString value = text.trimmed().toLower();
    if (value == QLatin1String("level0") || value == QLatin1String("0")) return ElevationRole::Level0;
    if (value == QLatin1String("level1") || value == QLatin1String("1")) return ElevationRole::Level1;
    if (value == QLatin1String("level2") || value == QLatin1String("2")) return ElevationRole::Level2;
    if (value == QLatin1String("level3") || value == QLatin1String("3")) return ElevationRole::Level3;
    if (value == QLatin1String("level4") || value == QLatin1String("4")) return ElevationRole::Level4;
    if (value == QLatin1String("level5") || value == QLatin1String("5")) return ElevationRole::Level5;
    return fallback;
}

MotionToken parseMotionToken(const QString& text, MotionToken fallback)
{
    const QString value = text.trimmed().toLower();
    if (value == QLatin1String("short1")) return MotionToken::Short1;
    if (value == QLatin1String("short2")) return MotionToken::Short2;
    if (value == QLatin1String("short3")) return MotionToken::Short3;
    if (value == QLatin1String("short4")) return MotionToken::Short4;
    if (value == QLatin1String("medium1")) return MotionToken::Medium1;
    if (value == QLatin1String("medium2")) return MotionToken::Medium2;
    if (value == QLatin1String("medium3")) return MotionToken::Medium3;
    if (value == QLatin1String("medium4")) return MotionToken::Medium4;
    if (value == QLatin1String("long1")) return MotionToken::Long1;
    if (value == QLatin1String("long2")) return MotionToken::Long2;
    if (value == QLatin1String("long3")) return MotionToken::Long3;
    if (value == QLatin1String("long4")) return MotionToken::Long4;
    return fallback;
}

void applyShapeMotionElevation(const ComponentTokenOverride& tokens, ShapeRole* shape, ElevationRole* elevation, MotionToken* motion)
{
    if (tokens.shapes.contains(ShapeRole::Full)) *shape = ShapeRole::Full;
    else if (tokens.shapes.contains(ShapeRole::ExtraLarge)) *shape = ShapeRole::ExtraLarge;
    else if (tokens.shapes.contains(ShapeRole::Large)) *shape = ShapeRole::Large;
    else if (tokens.shapes.contains(ShapeRole::Medium)) *shape = ShapeRole::Medium;
    else if (tokens.shapes.contains(ShapeRole::Small)) *shape = ShapeRole::Small;
    else if (tokens.shapes.contains(ShapeRole::ExtraSmall)) *shape = ShapeRole::ExtraSmall;
    else if (tokens.shapes.contains(ShapeRole::None)) *shape = ShapeRole::None;

    if (tokens.elevations.contains(ElevationRole::Level5)) *elevation = ElevationRole::Level5;
    else if (tokens.elevations.contains(ElevationRole::Level4)) *elevation = ElevationRole::Level4;
    else if (tokens.elevations.contains(ElevationRole::Level3)) *elevation = ElevationRole::Level3;
    else if (tokens.elevations.contains(ElevationRole::Level2)) *elevation = ElevationRole::Level2;
    else if (tokens.elevations.contains(ElevationRole::Level1)) *elevation = ElevationRole::Level1;
    else if (tokens.elevations.contains(ElevationRole::Level0)) *elevation = ElevationRole::Level0;

    if (tokens.motion.contains(MotionToken::Short1)) *motion = MotionToken::Short1;
    else if (tokens.motion.contains(MotionToken::Short2)) *motion = MotionToken::Short2;
    else if (tokens.motion.contains(MotionToken::Short3)) *motion = MotionToken::Short3;
    else if (tokens.motion.contains(MotionToken::Short4)) *motion = MotionToken::Short4;
    else if (tokens.motion.contains(MotionToken::Medium1)) *motion = MotionToken::Medium1;
    else if (tokens.motion.contains(MotionToken::Medium2)) *motion = MotionToken::Medium2;
    else if (tokens.motion.contains(MotionToken::Medium3)) *motion = MotionToken::Medium3;
    else if (tokens.motion.contains(MotionToken::Medium4)) *motion = MotionToken::Medium4;
    else if (tokens.motion.contains(MotionToken::Long1)) *motion = MotionToken::Long1;
    else if (tokens.motion.contains(MotionToken::Long2)) *motion = MotionToken::Long2;
    else if (tokens.motion.contains(MotionToken::Long3)) *motion = MotionToken::Long3;
    else if (tokens.motion.contains(MotionToken::Long4)) *motion = MotionToken::Long4;

    const QVariantMap& custom = tokens.custom;
    if (custom.contains(QStringLiteral("shapeRole"))) {
        *shape = parseShapeRole(custom.value(QStringLiteral("shapeRole")).toString(), *shape);
    }
    if (custom.contains(QStringLiteral("elevationRole"))) {
        *elevation = parseElevationRole(custom.value(QStringLiteral("elevationRole")).toString(), *elevation);
    }
    if (custom.contains(QStringLiteral("motionToken"))) {
        *motion = parseMotionToken(custom.value(QStringLiteral("motionToken")).toString(), *motion);
    }
}

void applyColor(QColor* destination, const ComponentTokenOverride& tokens, ColorRole role)
{
    if (tokens.colors.contains(role)) {
        *destination = tokens.colors.value(role);
    }
}

void applyCustomColor(QColor* destination, const ComponentTokenOverride& tokens, const char* key)
{
    QColor color;
    if (readColor(tokens.custom, key, &color)) {
        *destination = color;
    }
}

void applyTouchTarget(QSize* touchTarget, const ComponentTokenOverride& tokens)
{
    int width = touchTarget->width();
    int height = touchTarget->height();
    bool changed = false;
    changed = readInt(tokens.custom, "touchTargetWidth", &width) || changed;
    changed = readInt(tokens.custom, "touchTargetHeight", &height) || changed;
    if (changed) {
        *touchTarget = QSize(width, height);
    }
}

int iconSizeFromTokens(const ComponentTokenOverride& tokens, IconSizeRole role, int fallback)
{
    return tokens.iconSizes.value(role, fallback);
}

} // namespace

ComponentTokenOverride mergedComponentOverride(const Theme& theme, const QStringList& componentNames)
{
    ComponentTokenOverride merged;
    for (const QString& componentName : componentNames) {
        if (theme.componentOverrides().contains(componentName)) {
            mergeOverride(&merged, theme.componentOverrides().overrideFor(componentName));
        }
    }
    return merged;
}

void applyButtonComponentTokens(
    const Theme& theme,
    const QStringList& componentNames,
    ButtonSpec* spec)
{
    if (!spec) {
        return;
    }

    const ComponentTokenOverride tokens =
        mergedComponentOverride(theme, componentNames);

    // Runtime defaults are resolved even when no component override exists.
    spec->shadowColor = theme.colorScheme().color(ColorRole::Shadow);

    if (!tokens.isEmpty()) {
        applyColor(&spec->containerColor, tokens, ColorRole::Primary);
        applyColor(&spec->labelColor, tokens, ColorRole::OnPrimary);
        applyColor(&spec->iconColor, tokens, ColorRole::OnPrimary);
        applyColor(
            &spec->disabledContainerColor,
            tokens,
            ColorRole::SurfaceContainerHigh);
        applyColor(
            &spec->disabledLabelColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(&spec->stateLayerColor, tokens, ColorRole::SurfaceTint);
        applyColor(&spec->focusRingColor, tokens, ColorRole::Primary);
        applyColor(&spec->outlineColor, tokens, ColorRole::Outline);
        applyColor(
            &spec->disabledOutlineColor,
            tokens,
            ColorRole::OutlineVariant);
        applyColor(&spec->shadowColor, tokens, ColorRole::Shadow);

        applyCustomColor(&spec->containerColor, tokens, "containerColor");
        applyCustomColor(&spec->labelColor, tokens, "labelColor");
        applyCustomColor(&spec->iconColor, tokens, "iconColor");
        applyCustomColor(
            &spec->disabledContainerColor,
            tokens,
            "disabledContainerColor");
        applyCustomColor(
            &spec->disabledLabelColor,
            tokens,
            "disabledLabelColor");
        applyCustomColor(
            &spec->stateLayerColor,
            tokens,
            "stateLayerColor");
        applyCustomColor(&spec->focusRingColor, tokens, "focusRingColor");
        applyCustomColor(&spec->outlineColor, tokens, "outlineColor");
        applyCustomColor(
            &spec->disabledOutlineColor,
            tokens,
            "disabledOutlineColor");
        applyCustomColor(&spec->shadowColor, tokens, "shadowColor");

        applyShapeMotionElevation(
            tokens,
            &spec->shapeRole,
            &spec->elevationRole,
            &spec->motionToken);
        applyTouchTarget(&spec->touchTarget, tokens);

        spec->iconSize =
            iconSizeFromTokens(tokens, IconSizeRole::Small, spec->iconSize);
        readInt(tokens.custom, "containerHeight", &spec->containerHeight);
        readInt(tokens.custom, "horizontalPadding", &spec->horizontalPadding);
        readInt(tokens.custom, "iconSize", &spec->iconSize);
        readInt(tokens.custom, "iconSpacing", &spec->iconSpacing);
    }

    // Resolve typography after role overrides.
    spec->hasResolvedLabelFont = false;
    if (tokens.typography.contains(spec->labelTypeRole)) {
        spec->labelFont = tokens.typography.value(spec->labelTypeRole).font;
        spec->hasResolvedLabelFont = true;
    } else if (theme.typography().contains(spec->labelTypeRole)) {
        spec->labelFont =
            theme.typography().style(spec->labelTypeRole).font;
        spec->hasResolvedLabelFont = true;
    }

    // A negative radius represents the dynamic Material "full" pill radius.
    if (spec->shapeRole == ShapeRole::Full) {
        spec->cornerRadius = -1.0;
    } else if (tokens.shapes.contains(spec->shapeRole)) {
        spec->cornerRadius =
            qMax<qreal>(0.0, tokens.shapes.value(spec->shapeRole));
    } else if (theme.shapes().contains(spec->shapeRole)) {
        spec->cornerRadius =
            qMax<qreal>(0.0, theme.shapes().radius(spec->shapeRole));
    } else {
        spec->cornerRadius = -1.0;
    }

    // Resolve motion after token overrides.
    spec->hasResolvedMotionStyle = false;
    if (tokens.motion.contains(spec->motionToken)) {
        spec->motionStyle = tokens.motion.value(spec->motionToken);
        spec->hasResolvedMotionStyle = true;
    } else if (theme.motion().contains(spec->motionToken)) {
        spec->motionStyle = theme.motion().style(spec->motionToken);
        spec->hasResolvedMotionStyle = true;
    }

    // Component-local state-layer tokens take precedence over global tokens.
    const StateLayer& stateLayer =
        tokens.hasStateLayer ? tokens.stateLayer : theme.stateLayer();
    spec->hoverStateLayerOpacity = stateLayer.hoverOpacity;
    spec->focusStateLayerOpacity = stateLayer.focusOpacity;
    spec->pressStateLayerOpacity = stateLayer.pressOpacity;
    spec->dragStateLayerOpacity = stateLayer.dragOpacity;

    // Explicit concrete values are the final override layer.
    if (!tokens.isEmpty()) {
        readReal(tokens.custom, "cornerRadius", &spec->cornerRadius);
        readReal(
            tokens.custom,
            "hoverStateLayerOpacity",
            &spec->hoverStateLayerOpacity);
        readReal(
            tokens.custom,
            "focusStateLayerOpacity",
            &spec->focusStateLayerOpacity);
        readReal(
            tokens.custom,
            "pressStateLayerOpacity",
            &spec->pressStateLayerOpacity);
        readReal(
            tokens.custom,
            "dragStateLayerOpacity",
            &spec->dragStateLayerOpacity);
    }
}

void applyFabComponentTokens(
    const Theme& theme,
    const QStringList& componentNames,
    FabSpec* spec)
{
    if (!spec) {
        return;
    }

    const ComponentTokenOverride tokens =
        mergedComponentOverride(theme, componentNames);

    if (!tokens.isEmpty()) {
        applyColor(
            &spec->containerColor,
            tokens,
            ColorRole::PrimaryContainer);
        applyColor(
            &spec->iconColor,
            tokens,
            ColorRole::OnPrimaryContainer);
        applyColor(
            &spec->disabledContainerColor,
            tokens,
            ColorRole::SurfaceContainerHigh);
        applyColor(
            &spec->disabledIconColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->stateLayerColor,
            tokens,
            ColorRole::SurfaceTint);
        applyColor(
            &spec->focusRingColor,
            tokens,
            ColorRole::Primary);

        applyCustomColor(
            &spec->containerColor,
            tokens,
            "containerColor");
        applyCustomColor(
            &spec->iconColor,
            tokens,
            "iconColor");
        applyCustomColor(
            &spec->disabledContainerColor,
            tokens,
            "disabledContainerColor");
        applyCustomColor(
            &spec->disabledIconColor,
            tokens,
            "disabledIconColor");
        applyCustomColor(
            &spec->stateLayerColor,
            tokens,
            "stateLayerColor");
        applyCustomColor(
            &spec->focusRingColor,
            tokens,
            "focusRingColor");
        applyCustomColor(
            &spec->shadowColor,
            tokens,
            "shadowColor");

        applyShapeMotionElevation(
            tokens,
            &spec->shapeRole,
            &spec->elevationRole,
            &spec->motionToken);
        applyTouchTarget(&spec->touchTarget, tokens);
        spec->iconSize = iconSizeFromTokens(
            tokens,
            IconSizeRole::Medium,
            spec->iconSize);

        readInt(
            tokens.custom,
            "containerDiameter",
            &spec->containerDiameter);
        readInt(
            tokens.custom,
            "iconSize",
            &spec->iconSize);
    }

    if (!spec->focusRingColor.isValid()) {
        spec->focusRingColor =
            theme.colorScheme().color(ColorRole::Primary);
    }
    if (!spec->shadowColor.isValid()) {
        spec->shadowColor =
            theme.colorScheme().color(ColorRole::Shadow);
    }

    spec->hasResolvedLabelFont = false;
    if (tokens.typography.contains(spec->labelTypeRole)) {
        spec->labelFont =
            tokens.typography.value(spec->labelTypeRole).font;
        spec->hasResolvedLabelFont = true;
    } else if (
        theme.typography().contains(spec->labelTypeRole)) {
        spec->labelFont =
            theme.typography().style(spec->labelTypeRole).font;
        spec->hasResolvedLabelFont = true;
    }

    if (spec->shapeRole == ShapeRole::Full) {
        spec->cornerRadius = -1.0;
    } else if (tokens.shapes.contains(spec->shapeRole)) {
        spec->cornerRadius = qMax<qreal>(
            0.0,
            static_cast<qreal>(
                tokens.shapes.value(spec->shapeRole)));
    } else if (theme.shapes().contains(spec->shapeRole)) {
        spec->cornerRadius = qMax<qreal>(
            0.0,
            static_cast<qreal>(
                theme.shapes().radius(spec->shapeRole)));
    }

    spec->hasResolvedMotionStyle = false;
    if (tokens.motion.contains(spec->motionToken)) {
        spec->motionStyle =
            tokens.motion.value(spec->motionToken);
        spec->hasResolvedMotionStyle = true;
    } else if (theme.motion().contains(spec->motionToken)) {
        spec->motionStyle =
            theme.motion().style(spec->motionToken);
        spec->hasResolvedMotionStyle = true;
    }

    spec->hasResolvedElevationStyle = false;
    if (tokens.elevations.contains(spec->elevationRole)) {
        spec->elevationStyle =
            tokens.elevations.value(spec->elevationRole);
        spec->hasResolvedElevationStyle = true;
    } else if (
        theme.elevations().contains(spec->elevationRole)) {
        spec->elevationStyle =
            theme.elevations().style(spec->elevationRole);
        spec->hasResolvedElevationStyle = true;
    }

    const StateLayer& stateLayer =
        tokens.hasStateLayer
        ? tokens.stateLayer
        : theme.stateLayer();
    spec->hoverStateLayerOpacity =
        stateLayer.hoverOpacity;
    spec->focusStateLayerOpacity =
        stateLayer.focusOpacity;
    spec->pressStateLayerOpacity =
        stateLayer.pressOpacity;
    spec->dragStateLayerOpacity =
        stateLayer.dragOpacity;

    if (!tokens.isEmpty()) {
        readReal(
            tokens.custom,
            "cornerRadius",
            &spec->cornerRadius);
        readReal(
            tokens.custom,
            "hoverStateLayerOpacity",
            &spec->hoverStateLayerOpacity);
        readReal(
            tokens.custom,
            "focusStateLayerOpacity",
            &spec->focusStateLayerOpacity);
        readReal(
            tokens.custom,
            "pressStateLayerOpacity",
            &spec->pressStateLayerOpacity);
        readReal(
            tokens.custom,
            "dragStateLayerOpacity",
            &spec->dragStateLayerOpacity);
        readReal(
            tokens.custom,
            "restingElevationProgress",
            &spec->restingElevationProgress);
        readReal(
            tokens.custom,
            "hoverElevationProgress",
            &spec->hoverElevationProgress);
        readReal(
            tokens.custom,
            "focusElevationProgress",
            &spec->focusElevationProgress);
        readReal(
            tokens.custom,
            "pressElevationProgress",
            &spec->pressElevationProgress);
        readReal(
            tokens.custom,
            "disabledElevationProgress",
            &spec->disabledElevationProgress);
    }
}

void applyIconButtonComponentTokens(
    const Theme& theme,
    const QStringList& componentNames,
    IconButtonSpec* spec)
{
    if (!spec) {
        return;
    }

    const ComponentTokenOverride tokens =
        mergedComponentOverride(theme, componentNames);

    if (!tokens.isEmpty()) {
        applyColor(
            &spec->containerColor,
            tokens,
            ColorRole::SurfaceContainerLow);
        applyColor(
            &spec->iconColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->selectedContainerColor,
            tokens,
            ColorRole::PrimaryContainer);
        applyColor(
            &spec->selectedIconColor,
            tokens,
            ColorRole::OnPrimaryContainer);
        applyColor(
            &spec->disabledIconColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->stateLayerColor,
            tokens,
            ColorRole::SurfaceTint);
        applyColor(
            &spec->focusRingColor,
            tokens,
            ColorRole::Primary);

        applyCustomColor(
            &spec->containerColor,
            tokens,
            "containerColor");
        applyCustomColor(
            &spec->iconColor,
            tokens,
            "iconColor");
        applyCustomColor(
            &spec->selectedContainerColor,
            tokens,
            "selectedContainerColor");
        applyCustomColor(
            &spec->selectedIconColor,
            tokens,
            "selectedIconColor");
        applyCustomColor(
            &spec->disabledIconColor,
            tokens,
            "disabledIconColor");
        applyCustomColor(
            &spec->stateLayerColor,
            tokens,
            "stateLayerColor");
        applyCustomColor(
            &spec->focusRingColor,
            tokens,
            "focusRingColor");

        applyShapeMotionElevation(
            tokens,
            &spec->shapeRole,
            &spec->elevationRole,
            &spec->motionToken);
        applyTouchTarget(&spec->touchTarget, tokens);
        spec->iconSize = iconSizeFromTokens(
            tokens,
            IconSizeRole::Medium,
            spec->iconSize);

        readInt(
            tokens.custom,
            "containerSize",
            &spec->containerSize);
        readInt(
            tokens.custom,
            "iconSize",
            &spec->iconSize);
        readReal(
            tokens.custom,
            "focusRingWidth",
            &spec->focusRingWidth);
    }

    if (!spec->focusRingColor.isValid()) {
        spec->focusRingColor =
            theme.colorScheme().color(ColorRole::Primary);
    }

    if (spec->shapeRole == ShapeRole::Full) {
        spec->cornerRadius = -1.0;
    } else if (tokens.shapes.contains(spec->shapeRole)) {
        spec->cornerRadius = qMax<qreal>(
            0.0,
            static_cast<qreal>(
                tokens.shapes.value(spec->shapeRole)));
    } else if (theme.shapes().contains(spec->shapeRole)) {
        spec->cornerRadius = qMax<qreal>(
            0.0,
            static_cast<qreal>(
                theme.shapes().radius(spec->shapeRole)));
    }

    spec->hasResolvedMotionStyle = false;
    if (tokens.motion.contains(spec->motionToken)) {
        spec->motionStyle =
            tokens.motion.value(spec->motionToken);
        spec->hasResolvedMotionStyle = true;
    } else if (theme.motion().contains(spec->motionToken)) {
        spec->motionStyle =
            theme.motion().style(spec->motionToken);
        spec->hasResolvedMotionStyle = true;
    }

    const StateLayer& stateLayer =
        tokens.hasStateLayer
        ? tokens.stateLayer
        : theme.stateLayer();
    spec->hoverStateLayerOpacity =
        stateLayer.hoverOpacity;
    spec->focusStateLayerOpacity =
        stateLayer.focusOpacity;
    spec->pressStateLayerOpacity =
        stateLayer.pressOpacity;

    if (!tokens.isEmpty()) {
        readReal(
            tokens.custom,
            "cornerRadius",
            &spec->cornerRadius);
        readReal(
            tokens.custom,
            "hoverStateLayerOpacity",
            &spec->hoverStateLayerOpacity);
        readReal(
            tokens.custom,
            "focusStateLayerOpacity",
            &spec->focusStateLayerOpacity);
        readReal(
            tokens.custom,
            "pressStateLayerOpacity",
            &spec->pressStateLayerOpacity);
    }
}

template <typename SpecT>
void resolveSelectionRuntimeTokens(
    const Theme& theme,
    const ComponentTokenOverride& tokens,
    SpecT* spec)
{
    if (!spec) {
        return;
    }

    spec->hasResolvedLabelFont = false;
    if (tokens.typography.contains(spec->labelTypeRole)) {
        spec->labelFont =
            tokens.typography.value(spec->labelTypeRole).font;
        spec->hasResolvedLabelFont = true;
    } else if (theme.typography().contains(spec->labelTypeRole)) {
        spec->labelFont =
            theme.typography().style(spec->labelTypeRole).font;
        spec->hasResolvedLabelFont = true;
    }

    spec->hasResolvedMotionStyle = false;
    if (tokens.motion.contains(spec->motionToken)) {
        spec->motionStyle =
            tokens.motion.value(spec->motionToken);
        spec->hasResolvedMotionStyle = true;
    } else if (theme.motion().contains(spec->motionToken)) {
        spec->motionStyle =
            theme.motion().style(spec->motionToken);
        spec->hasResolvedMotionStyle = true;
    }

    const StateLayer& stateLayer =
        tokens.hasStateLayer ? tokens.stateLayer : theme.stateLayer();
    spec->hoverStateLayerOpacity = stateLayer.hoverOpacity;
    spec->focusStateLayerOpacity = stateLayer.focusOpacity;
    spec->pressStateLayerOpacity = stateLayer.pressOpacity;
    spec->dragStateLayerOpacity = stateLayer.dragOpacity;

    if (!tokens.isEmpty()) {
        readReal(
            tokens.custom,
            "hoverStateLayerOpacity",
            &spec->hoverStateLayerOpacity);
        readReal(
            tokens.custom,
            "focusStateLayerOpacity",
            &spec->focusStateLayerOpacity);
        readReal(
            tokens.custom,
            "pressStateLayerOpacity",
            &spec->pressStateLayerOpacity);
        readReal(
            tokens.custom,
            "dragStateLayerOpacity",
            &spec->dragStateLayerOpacity);
    }
}

void applyCheckboxComponentTokens(
    const Theme& theme,
    const QStringList& componentNames,
    CheckboxSpec* spec)
{
    if (!spec) {
        return;
    }

    const ComponentTokenOverride tokens =
        mergedComponentOverride(theme, componentNames);

    if (!tokens.isEmpty()) {
        applyColor(
            &spec->selectedContainerColor,
            tokens,
            ColorRole::Primary);
        applyColor(
            &spec->selectedIconColor,
            tokens,
            ColorRole::OnPrimary);
        applyColor(
            &spec->unselectedOutlineColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->disabledSelectedContainerColor,
            tokens,
            ColorRole::SurfaceContainerHigh);
        applyColor(
            &spec->disabledUnselectedOutlineColor,
            tokens,
            ColorRole::OutlineVariant);
        applyColor(&spec->labelColor, tokens, ColorRole::OnSurface);
        applyColor(
            &spec->disabledLabelColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->stateLayerColor,
            tokens,
            ColorRole::Primary);
        applyColor(
            &spec->focusRingColor,
            tokens,
            ColorRole::Primary);

        applyCustomColor(
            &spec->selectedContainerColor,
            tokens,
            "selectedContainerColor");
        applyCustomColor(
            &spec->selectedIconColor,
            tokens,
            "selectedIconColor");
        applyCustomColor(
            &spec->unselectedOutlineColor,
            tokens,
            "unselectedOutlineColor");
        applyCustomColor(
            &spec->disabledSelectedContainerColor,
            tokens,
            "disabledSelectedContainerColor");
        applyCustomColor(
            &spec->disabledUnselectedOutlineColor,
            tokens,
            "disabledUnselectedOutlineColor");
        applyCustomColor(&spec->labelColor, tokens, "labelColor");
        applyCustomColor(
            &spec->disabledLabelColor,
            tokens,
            "disabledLabelColor");
        applyCustomColor(
            &spec->stateLayerColor,
            tokens,
            "stateLayerColor");
        applyCustomColor(
            &spec->focusRingColor,
            tokens,
            "focusRingColor");

        applyTouchTarget(&spec->touchTarget, tokens);
        spec->indicatorSize = iconSizeFromTokens(
            tokens,
            IconSizeRole::Small,
            spec->indicatorSize);

        readInt(tokens.custom, "indicatorSize", &spec->indicatorSize);
        readInt(tokens.custom, "spacing", &spec->spacing);
        readInt(tokens.custom, "outlineWidth", &spec->outlineWidth);
        readInt(tokens.custom, "cornerRadius", &spec->cornerRadius);
        readInt(
            tokens.custom,
            "checkmarkStrokeWidth",
            &spec->checkmarkStrokeWidth);
        readInt(tokens.custom, "stateLayerSize", &spec->stateLayerSize);

        if (tokens.custom.contains(QStringLiteral("motionToken"))) {
            spec->motionToken = parseMotionToken(
                tokens.custom
                    .value(QStringLiteral("motionToken"))
                    .toString(),
                spec->motionToken);
        }
    }

    resolveSelectionRuntimeTokens(theme, tokens, spec);
}

void applyRadioButtonComponentTokens(
    const Theme& theme,
    const QStringList& componentNames,
    RadioButtonSpec* spec)
{
    if (!spec) {
        return;
    }

    const ComponentTokenOverride tokens =
        mergedComponentOverride(theme, componentNames);

    if (!tokens.isEmpty()) {
        applyColor(
            &spec->selectedColor,
            tokens,
            ColorRole::Primary);
        applyColor(
            &spec->unselectedOutlineColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->disabledColor,
            tokens,
            ColorRole::OutlineVariant);
        applyColor(&spec->labelColor, tokens, ColorRole::OnSurface);
        applyColor(
            &spec->disabledLabelColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->stateLayerColor,
            tokens,
            ColorRole::Primary);
        applyColor(
            &spec->focusRingColor,
            tokens,
            ColorRole::Primary);

        applyCustomColor(
            &spec->selectedColor,
            tokens,
            "selectedColor");
        applyCustomColor(
            &spec->unselectedOutlineColor,
            tokens,
            "unselectedOutlineColor");
        applyCustomColor(
            &spec->disabledColor,
            tokens,
            "disabledColor");
        applyCustomColor(&spec->labelColor, tokens, "labelColor");
        applyCustomColor(
            &spec->disabledLabelColor,
            tokens,
            "disabledLabelColor");
        applyCustomColor(
            &spec->stateLayerColor,
            tokens,
            "stateLayerColor");
        applyCustomColor(
            &spec->focusRingColor,
            tokens,
            "focusRingColor");

        applyTouchTarget(&spec->touchTarget, tokens);
        spec->indicatorSize = iconSizeFromTokens(
            tokens,
            IconSizeRole::Small,
            spec->indicatorSize);

        readInt(tokens.custom, "indicatorSize", &spec->indicatorSize);
        readInt(tokens.custom, "dotSize", &spec->dotSize);
        readInt(tokens.custom, "spacing", &spec->spacing);
        readInt(tokens.custom, "outlineWidth", &spec->outlineWidth);
        readInt(tokens.custom, "stateLayerSize", &spec->stateLayerSize);

        if (tokens.custom.contains(QStringLiteral("motionToken"))) {
            spec->motionToken = parseMotionToken(
                tokens.custom
                    .value(QStringLiteral("motionToken"))
                    .toString(),
                spec->motionToken);
        }
    }

    resolveSelectionRuntimeTokens(theme, tokens, spec);
}

void applySwitchComponentTokens(
    const Theme& theme,
    const QStringList& componentNames,
    SwitchSpec* spec)
{
    if (!spec) {
        return;
    }

    const ComponentTokenOverride tokens =
        mergedComponentOverride(theme, componentNames);

    if (!tokens.isEmpty()) {
        applyColor(
            &spec->selectedTrackColor,
            tokens,
            ColorRole::Primary);
        applyColor(
            &spec->selectedHandleColor,
            tokens,
            ColorRole::OnPrimary);
        applyColor(
            &spec->unselectedTrackColor,
            tokens,
            ColorRole::SurfaceVariant);
        applyColor(
            &spec->unselectedHandleColor,
            tokens,
            ColorRole::Outline);
        applyColor(
            &spec->disabledSelectedTrackColor,
            tokens,
            ColorRole::SurfaceContainerHigh);
        applyColor(
            &spec->disabledSelectedHandleColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->disabledUnselectedTrackColor,
            tokens,
            ColorRole::SurfaceContainerHigh);
        applyColor(
            &spec->disabledUnselectedHandleColor,
            tokens,
            ColorRole::OutlineVariant);
        applyColor(&spec->iconColor, tokens, ColorRole::OnPrimary);
        applyColor(
            &spec->stateLayerColor,
            tokens,
            ColorRole::Primary);
        applyColor(
            &spec->focusRingColor,
            tokens,
            ColorRole::Primary);
        applyColor(&spec->labelColor, tokens, ColorRole::OnSurface);
        applyColor(
            &spec->disabledLabelColor,
            tokens,
            ColorRole::OnSurfaceVariant);

        applyCustomColor(
            &spec->selectedTrackColor,
            tokens,
            "selectedTrackColor");
        applyCustomColor(
            &spec->selectedHandleColor,
            tokens,
            "selectedHandleColor");
        applyCustomColor(
            &spec->unselectedTrackColor,
            tokens,
            "unselectedTrackColor");
        applyCustomColor(
            &spec->unselectedHandleColor,
            tokens,
            "unselectedHandleColor");
        applyCustomColor(
            &spec->disabledSelectedTrackColor,
            tokens,
            "disabledSelectedTrackColor");
        applyCustomColor(
            &spec->disabledSelectedHandleColor,
            tokens,
            "disabledSelectedHandleColor");
        applyCustomColor(
            &spec->disabledUnselectedTrackColor,
            tokens,
            "disabledUnselectedTrackColor");
        applyCustomColor(
            &spec->disabledUnselectedHandleColor,
            tokens,
            "disabledUnselectedHandleColor");
        applyCustomColor(&spec->iconColor, tokens, "iconColor");
        applyCustomColor(
            &spec->stateLayerColor,
            tokens,
            "stateLayerColor");
        applyCustomColor(
            &spec->focusRingColor,
            tokens,
            "focusRingColor");
        applyCustomColor(&spec->labelColor, tokens, "labelColor");
        applyCustomColor(
            &spec->disabledLabelColor,
            tokens,
            "disabledLabelColor");

        applyTouchTarget(&spec->touchTarget, tokens);

        readInt(tokens.custom, "trackWidth", &spec->trackWidth);
        readInt(tokens.custom, "trackHeight", &spec->trackHeight);
        readInt(
            tokens.custom,
            "handleDiameter",
            &spec->handleDiameter);
        readInt(
            tokens.custom,
            "selectedHandleDiameter",
            &spec->selectedHandleDiameter);
        readInt(tokens.custom, "spacing", &spec->spacing);
        readInt(tokens.custom, "stateLayerSize", &spec->stateLayerSize);

        if (tokens.custom.contains(QStringLiteral("motionToken"))) {
            spec->motionToken = parseMotionToken(
                tokens.custom
                    .value(QStringLiteral("motionToken"))
                    .toString(),
                spec->motionToken);
        }
    }

    resolveSelectionRuntimeTokens(theme, tokens, spec);
}

void applyAutocompletePopupComponentTokens(
    const Theme& theme,
    const QStringList& componentNames,
    AutocompletePopupSpec* spec)
{
    if (!spec) {
        return;
    }

    const ComponentTokenOverride tokens =
        mergedComponentOverride(theme, componentNames);

    if (!tokens.isEmpty()) {
        applyColor(
            &spec->containerColor,
            tokens,
            ColorRole::SurfaceContainerHigh);
        applyColor(
            &spec->textColor,
            tokens,
            ColorRole::OnSurface);
        applyColor(
            &spec->selectedTextColor,
            tokens,
            ColorRole::OnSecondaryContainer);
        applyColor(
            &spec->supportingTextColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->selectedContainerColor,
            tokens,
            ColorRole::SecondaryContainer);
        applyColor(
            &spec->stateLayerColor,
            tokens,
            ColorRole::OnSurface);
        applyColor(
            &spec->dividerColor,
            tokens,
            ColorRole::OutlineVariant);
        applyColor(
            &spec->focusRingColor,
            tokens,
            ColorRole::Primary);

        applyCustomColor(
            &spec->containerColor,
            tokens,
            "containerColor");
        applyCustomColor(
            &spec->textColor,
            tokens,
            "textColor");
        applyCustomColor(
            &spec->selectedTextColor,
            tokens,
            "selectedTextColor");
        applyCustomColor(
            &spec->supportingTextColor,
            tokens,
            "supportingTextColor");
        applyCustomColor(
            &spec->selectedContainerColor,
            tokens,
            "selectedContainerColor");
        applyCustomColor(
            &spec->stateLayerColor,
            tokens,
            "stateLayerColor");
        applyCustomColor(
            &spec->dividerColor,
            tokens,
            "dividerColor");
        applyCustomColor(
            &spec->focusRingColor,
            tokens,
            "focusRingColor");
        applyCustomColor(
            &spec->shadowColor,
            tokens,
            "shadowColor");

        applyShapeMotionElevation(
            tokens,
            &spec->shapeRole,
            &spec->elevationRole,
            &spec->motionToken);

        int minWidth = spec->minPopupSize.width();
        int minHeight = spec->minPopupSize.height();
        int maxWidth = spec->maxPopupSize.width();
        int maxHeight = spec->maxPopupSize.height();
        int iconWidth = spec->itemIconSize.width();
        int iconHeight = spec->itemIconSize.height();
        int itemMinWidth = spec->itemMinSize.width();
        int itemMinHeight = spec->itemMinSize.height();

        readInt(tokens.custom, "minPopupWidth", &minWidth);
        readInt(tokens.custom, "minPopupHeight", &minHeight);
        readInt(tokens.custom, "maxPopupWidth", &maxWidth);
        readInt(tokens.custom, "maxPopupHeight", &maxHeight);
        readInt(tokens.custom, "itemIconWidth", &iconWidth);
        readInt(tokens.custom, "itemIconHeight", &iconHeight);
        readInt(tokens.custom, "itemMinWidth", &itemMinWidth);
        readInt(tokens.custom, "itemMinHeight", &itemMinHeight);

        spec->minPopupSize = QSize(minWidth, minHeight);
        spec->maxPopupSize = QSize(maxWidth, maxHeight);
        spec->itemIconSize = QSize(iconWidth, iconHeight);
        spec->itemMinSize = QSize(
            itemMinWidth,
            itemMinHeight);

        readMargins(tokens.custom, &spec->contentMargins);
        readInt(
            tokens.custom,
            "itemSpacing",
            &spec->itemSpacing);
        readInt(
            tokens.custom,
            "supportingTopSpacing",
            &spec->supportingTopSpacing);
        readInt(
            tokens.custom,
            "visibleItemCount",
            &spec->visibleItemCount);
        readInt(
            tokens.custom,
            "cornerRadius",
            &spec->cornerRadius);
        readInt(
            tokens.custom,
            "focusRingWidth",
            &spec->focusRingWidth);
        readBool(
            tokens.custom,
            "showDividers",
            &spec->showDividers);
    }

    if (!spec->shadowColor.isValid()) {
        spec->shadowColor =
            theme.colorScheme().color(ColorRole::Shadow);
    }

    spec->hasResolvedItemFont = false;
    if (tokens.typography.contains(spec->itemTypeRole)) {
        spec->itemFont =
            tokens.typography.value(
                spec->itemTypeRole).font;
        spec->hasResolvedItemFont = true;
    } else if (
        theme.typography().contains(spec->itemTypeRole)) {
        spec->itemFont =
            theme.typography().style(
                spec->itemTypeRole).font;
        spec->hasResolvedItemFont = true;
    }

    spec->hasResolvedSupportingFont = false;
    if (tokens.typography.contains(
            spec->supportingTypeRole)) {
        spec->supportingFont =
            tokens.typography.value(
                spec->supportingTypeRole).font;
        spec->hasResolvedSupportingFont = true;
    } else if (
        theme.typography().contains(
            spec->supportingTypeRole)) {
        spec->supportingFont =
            theme.typography().style(
                spec->supportingTypeRole).font;
        spec->hasResolvedSupportingFont = true;
    }

    if (spec->shapeRole == ShapeRole::Full) {
        spec->cornerRadius = -1;
    } else if (tokens.shapes.contains(
                   spec->shapeRole)) {
        spec->cornerRadius = qMax(
            0,
            tokens.shapes.value(spec->shapeRole));
    } else if (
        theme.shapes().contains(spec->shapeRole)) {
        spec->cornerRadius = qMax(
            0,
            theme.shapes().radius(spec->shapeRole));
    }

    spec->hasResolvedElevationStyle = false;
    if (tokens.elevations.contains(
            spec->elevationRole)) {
        spec->elevationStyle =
            tokens.elevations.value(
                spec->elevationRole);
        spec->hasResolvedElevationStyle = true;
    } else if (
        theme.elevations().contains(
            spec->elevationRole)) {
        spec->elevationStyle =
            theme.elevations().style(
                spec->elevationRole);
        spec->hasResolvedElevationStyle = true;
    }

    spec->hasResolvedMotionStyle = false;
    if (tokens.motion.contains(spec->motionToken)) {
        spec->motionStyle =
            tokens.motion.value(spec->motionToken);
        spec->hasResolvedMotionStyle = true;
    } else if (
        theme.motion().contains(spec->motionToken)) {
        spec->motionStyle =
            theme.motion().style(spec->motionToken);
        spec->hasResolvedMotionStyle = true;
    }

    const StateLayer& stateLayer =
        tokens.hasStateLayer
        ? tokens.stateLayer
        : theme.stateLayer();
    spec->hoverStateLayerOpacity =
        stateLayer.hoverOpacity;
    spec->focusStateLayerOpacity =
        stateLayer.focusOpacity;
    spec->pressStateLayerOpacity =
        stateLayer.pressOpacity;

    if (!tokens.isEmpty()) {
        readReal(
            tokens.custom,
            "hoverStateLayerOpacity",
            &spec->hoverStateLayerOpacity);
        readReal(
            tokens.custom,
            "focusStateLayerOpacity",
            &spec->focusStateLayerOpacity);
        readReal(
            tokens.custom,
            "pressStateLayerOpacity",
            &spec->pressStateLayerOpacity);
    }
}

void applyDateFieldComponentTokens(
    const Theme& theme,
    const QStringList& componentNames,
    DateFieldSpec* spec)
{
    if (!spec) {
        return;
    }

    const ComponentTokenOverride tokens =
        mergedComponentOverride(theme, componentNames);

    if (!tokens.isEmpty()) {
        applyColor(
            &spec->leadingIconColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->trailingIconColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->clearIconColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->placeholderColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->disabledIconColor,
            tokens,
            ColorRole::OnSurfaceVariant);

        applyCustomColor(
            &spec->leadingIconColor,
            tokens,
            "leadingIconColor");
        applyCustomColor(
            &spec->trailingIconColor,
            tokens,
            "trailingIconColor");
        applyCustomColor(
            &spec->clearIconColor,
            tokens,
            "clearIconColor");
        applyCustomColor(
            &spec->placeholderColor,
            tokens,
            "placeholderColor");
        applyCustomColor(
            &spec->disabledIconColor,
            tokens,
            "disabledIconColor");

        if (tokens.custom.contains(
                QStringLiteral("motionToken"))) {
            spec->motionToken = parseMotionToken(
                tokens.custom
                    .value(QStringLiteral("motionToken"))
                    .toString(),
                spec->motionToken);
        }

        readInt(
            tokens.custom,
            "actionIconSize",
            &spec->actionIconSize);
        readInt(
            tokens.custom,
            "actionButtonExtent",
            &spec->actionButtonExtent);
        readBool(
            tokens.custom,
            "calendarVisibleWhenEmpty",
            &spec->calendarVisibleWhenEmpty);
        readBool(
            tokens.custom,
            "preferClearAction",
            &spec->preferClearAction);
    }

    spec->actionIconSize =
        qMax(12, spec->actionIconSize);
    spec->actionButtonExtent =
        qMax(
            spec->actionIconSize,
            spec->actionButtonExtent);

    spec->hasResolvedMotionStyle = false;
    if (tokens.motion.contains(spec->motionToken)) {
        spec->motionStyle =
            tokens.motion.value(spec->motionToken);
        spec->hasResolvedMotionStyle = true;
    } else if (
        theme.motion().contains(spec->motionToken)) {
        spec->motionStyle =
            theme.motion().style(spec->motionToken);
        spec->hasResolvedMotionStyle = true;
    }
}

void applyNavigationRailComponentTokens(
    const Theme& theme,
    const QStringList& componentNames,
    NavigationRailSpec* spec)
{
    if (!spec) {
        return;
    }

    const ComponentTokenOverride tokens =
        mergedComponentOverride(theme, componentNames);

    if (!tokens.isEmpty()) {
        applyColor(
            &spec->containerColor,
            tokens,
            ColorRole::Surface);
        applyColor(
            &spec->indicatorColor,
            tokens,
            ColorRole::SecondaryContainer);
        applyColor(
            &spec->selectedIconColor,
            tokens,
            ColorRole::OnSecondaryContainer);
        applyColor(
            &spec->selectedLabelColor,
            tokens,
            ColorRole::OnSurface);
        applyColor(
            &spec->unselectedIconColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->unselectedLabelColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->disabledIconColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->disabledLabelColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->stateLayerColor,
            tokens,
            ColorRole::OnSurface);
        applyColor(
            &spec->focusRingColor,
            tokens,
            ColorRole::Primary);
        applyColor(
            &spec->dividerColor,
            tokens,
            ColorRole::OutlineVariant);

        applyCustomColor(
            &spec->containerColor,
            tokens,
            "containerColor");
        applyCustomColor(
            &spec->indicatorColor,
            tokens,
            "indicatorColor");
        applyCustomColor(
            &spec->selectedIconColor,
            tokens,
            "selectedIconColor");
        applyCustomColor(
            &spec->selectedLabelColor,
            tokens,
            "selectedLabelColor");
        applyCustomColor(
            &spec->unselectedIconColor,
            tokens,
            "unselectedIconColor");
        applyCustomColor(
            &spec->unselectedLabelColor,
            tokens,
            "unselectedLabelColor");
        applyCustomColor(
            &spec->stateLayerColor,
            tokens,
            "stateLayerColor");
        applyCustomColor(
            &spec->focusRingColor,
            tokens,
            "focusRingColor");
        applyCustomColor(
            &spec->dividerColor,
            tokens,
            "dividerColor");

        applyShapeMotionElevation(
            tokens,
            &spec->indicatorShapeRole,
            &spec->elevationRole,
            &spec->motionToken);

        spec->iconSize = iconSizeFromTokens(
            tokens,
            IconSizeRole::Small,
            spec->iconSize);

        readInt(tokens.custom, "railWidth", &spec->railWidth);
        readInt(tokens.custom, "itemHeight", &spec->itemHeight);
        readInt(tokens.custom, "itemSpacing", &spec->itemSpacing);
        readInt(tokens.custom, "topPadding", &spec->topPadding);
        readInt(tokens.custom, "bottomPadding", &spec->bottomPadding);
        readInt(tokens.custom, "iconSize", &spec->iconSize);
        readInt(
            tokens.custom,
            "indicatorTopOffset",
            &spec->indicatorTopOffset);
        readInt(
            tokens.custom,
            "iconTopOffset",
            &spec->iconTopOffset);
        readInt(
            tokens.custom,
            "labelTopOffset",
            &spec->labelTopOffset);
        readInt(
            tokens.custom,
            "labelHeight",
            &spec->labelHeight);
        readInt(
            tokens.custom,
            "dividerWidth",
            &spec->dividerWidth);
        readBool(
            tokens.custom,
            "showDivider",
            &spec->showDivider);

        int indicatorWidth = spec->indicatorSize.width();
        int indicatorHeight = spec->indicatorSize.height();
        readInt(
            tokens.custom,
            "indicatorWidth",
            &indicatorWidth);
        readInt(
            tokens.custom,
            "indicatorHeight",
            &indicatorHeight);
        spec->indicatorSize = QSize(
            qMax(1, indicatorWidth),
            qMax(1, indicatorHeight));

        readReal(
            tokens.custom,
            "indicatorRadius",
            &spec->indicatorRadius);
        readReal(
            tokens.custom,
            "hoverStateLayerOpacity",
            &spec->hoverStateLayerOpacity);
        readReal(
            tokens.custom,
            "focusStateLayerOpacity",
            &spec->focusStateLayerOpacity);
        readReal(
            tokens.custom,
            "pressStateLayerOpacity",
            &spec->pressStateLayerOpacity);
        readReal(
            tokens.custom,
            "disabledOpacity",
            &spec->disabledOpacity);
        readReal(
            tokens.custom,
            "focusRingWidth",
            &spec->focusRingWidth);
    }

    spec->hasResolvedLabelFont = false;
    if (tokens.typography.contains(spec->labelTypeRole)) {
        spec->labelFont =
            tokens.typography.value(spec->labelTypeRole).font;
        spec->hasResolvedLabelFont = true;
    } else if (theme.typography().contains(spec->labelTypeRole)) {
        spec->labelFont =
            theme.typography().style(spec->labelTypeRole).font;
        spec->hasResolvedLabelFont = true;
    }

    if (spec->indicatorShapeRole == ShapeRole::Full) {
        spec->indicatorRadius = -1.0;
    } else if (tokens.shapes.contains(
                   spec->indicatorShapeRole)) {
        spec->indicatorRadius = qMax<qreal>(
            0.0,
            tokens.shapes.value(spec->indicatorShapeRole));
    } else if (theme.shapes().contains(
                   spec->indicatorShapeRole)) {
        spec->indicatorRadius = qMax<qreal>(
            0.0,
            theme.shapes().radius(spec->indicatorShapeRole));
    }

    spec->hasResolvedMotionStyle = false;
    if (tokens.motion.contains(spec->motionToken)) {
        spec->motionStyle =
            tokens.motion.value(spec->motionToken);
        spec->hasResolvedMotionStyle = true;
    } else if (theme.motion().contains(spec->motionToken)) {
        spec->motionStyle =
            theme.motion().style(spec->motionToken);
        spec->hasResolvedMotionStyle = true;
    }

    const StateLayer& stateLayer =
        tokens.hasStateLayer
            ? tokens.stateLayer
            : theme.stateLayer();
    spec->hoverStateLayerOpacity =
        stateLayer.hoverOpacity;
    spec->focusStateLayerOpacity =
        stateLayer.focusOpacity;
    spec->pressStateLayerOpacity =
        stateLayer.pressOpacity;

    if (!tokens.isEmpty()) {
        readReal(
            tokens.custom,
            "hoverStateLayerOpacity",
            &spec->hoverStateLayerOpacity);
        readReal(
            tokens.custom,
            "focusStateLayerOpacity",
            &spec->focusStateLayerOpacity);
        readReal(
            tokens.custom,
            "pressStateLayerOpacity",
            &spec->pressStateLayerOpacity);
        readReal(
            tokens.custom,
            "disabledOpacity",
            &spec->disabledOpacity);
        readReal(
            tokens.custom,
            "focusRingWidth",
            &spec->focusRingWidth);
    }

    spec->disabledOpacity = qBound<qreal>(
        0.0,
        spec->disabledOpacity,
        1.0);

    spec->disabledIconColor =
        spec->unselectedIconColor;
    spec->disabledIconColor.setAlphaF(
        spec->disabledIconColor.alphaF()
        * spec->disabledOpacity);
    spec->disabledLabelColor =
        spec->unselectedLabelColor;
    spec->disabledLabelColor.setAlphaF(
        spec->disabledLabelColor.alphaF()
        * spec->disabledOpacity);

    if (!tokens.isEmpty()) {
        applyCustomColor(
            &spec->disabledIconColor,
            tokens,
            "disabledIconColor");
        applyCustomColor(
            &spec->disabledLabelColor,
            tokens,
            "disabledLabelColor");
    }

    spec->railWidth = qMax(48, spec->railWidth);
    spec->itemHeight = qMax(48, spec->itemHeight);
    spec->itemSpacing = qMax(0, spec->itemSpacing);
    spec->topPadding = qMax(0, spec->topPadding);
    spec->bottomPadding = qMax(0, spec->bottomPadding);
    spec->iconSize = qMax(0, spec->iconSize);
    spec->indicatorTopOffset =
        qMax(0, spec->indicatorTopOffset);
    spec->iconTopOffset = qMax(0, spec->iconTopOffset);
    spec->labelTopOffset =
        qMax(0, spec->labelTopOffset);
    spec->labelHeight = qMax(1, spec->labelHeight);
    spec->dividerWidth = qMax(1, spec->dividerWidth);
    spec->focusRingWidth =
        qMax<qreal>(1.0, spec->focusRingWidth);
}

void applyTextFieldComponentTokens(
    const Theme& theme,
    const QStringList& componentNames,
    TextFieldSpec* spec)
{
    if (!spec) {
        return;
    }

    const ComponentTokenOverride tokens =
        mergedComponentOverride(theme, componentNames);

    if (!tokens.isEmpty()) {
        applyColor(
            &spec->containerColor,
            tokens,
            ColorRole::SurfaceContainerHighest);
        applyColor(
            &spec->activeIndicatorColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->outlineColor,
            tokens,
            ColorRole::Outline);
        applyColor(
            &spec->focusedOutlineColor,
            tokens,
            ColorRole::Primary);
        applyColor(
            &spec->disabledOutlineColor,
            tokens,
            ColorRole::OutlineVariant);
        applyColor(
            &spec->inputTextColor,
            tokens,
            ColorRole::OnSurface);
        applyColor(
            &spec->disabledInputTextColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->labelColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->disabledLabelColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->supportingTextColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->disabledSupportingTextColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->errorTextColor,
            tokens,
            ColorRole::Error);
        applyColor(
            &spec->cursorColor,
            tokens,
            ColorRole::Primary);
        applyColor(
            &spec->errorColor,
            tokens,
            ColorRole::Error);
        applyColor(
            &spec->stateLayerColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->disabledErrorTextColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->focusRingColor,
            tokens,
            ColorRole::Primary);

        applyCustomColor(
            &spec->containerColor,
            tokens,
            "containerColor");
        applyCustomColor(
            &spec->activeIndicatorColor,
            tokens,
            "activeIndicatorColor");
        applyCustomColor(
            &spec->outlineColor,
            tokens,
            "outlineColor");
        applyCustomColor(
            &spec->focusedOutlineColor,
            tokens,
            "focusedOutlineColor");
        applyCustomColor(
            &spec->disabledOutlineColor,
            tokens,
            "disabledOutlineColor");
        applyCustomColor(
            &spec->inputTextColor,
            tokens,
            "inputTextColor");
        applyCustomColor(
            &spec->disabledInputTextColor,
            tokens,
            "disabledInputTextColor");
        applyCustomColor(
            &spec->labelColor,
            tokens,
            "labelColor");
        applyCustomColor(
            &spec->disabledLabelColor,
            tokens,
            "disabledLabelColor");
        applyCustomColor(
            &spec->supportingTextColor,
            tokens,
            "supportingTextColor");
        applyCustomColor(
            &spec->disabledSupportingTextColor,
            tokens,
            "disabledSupportingTextColor");
        applyCustomColor(
            &spec->errorTextColor,
            tokens,
            "errorTextColor");
        applyCustomColor(
            &spec->cursorColor,
            tokens,
            "cursorColor");
        applyCustomColor(
            &spec->errorColor,
            tokens,
            "errorColor");
        applyCustomColor(
            &spec->stateLayerColor,
            tokens,
            "stateLayerColor");
        applyCustomColor(
            &spec->disabledErrorTextColor,
            tokens,
            "disabledErrorTextColor");
        applyCustomColor(
            &spec->focusRingColor,
            tokens,
            "focusRingColor");

        ElevationRole unusedElevation =
            ElevationRole::Level0;
        applyShapeMotionElevation(
            tokens,
            &spec->shapeRole,
            &unusedElevation,
            &spec->motionToken);

        readInt(
            tokens.custom,
            "minHeight",
            &spec->minHeight);
        readInt(
            tokens.custom,
            "horizontalPadding",
            &spec->horizontalPadding);
        readInt(
            tokens.custom,
            "verticalPadding",
            &spec->verticalPadding);
        readInt(
            tokens.custom,
            "supportingTopSpacing",
            &spec->supportingTopSpacing);
        readInt(
            tokens.custom,
            "topLabelHeight",
            &spec->topLabelHeight);
        readInt(
            tokens.custom,
            "supportingHeight",
            &spec->supportingHeight);
        readInt(
            tokens.custom,
            "outlineWidth",
            &spec->outlineWidth);
        readInt(
            tokens.custom,
            "focusedOutlineWidth",
            &spec->focusedOutlineWidth);
        readReal(
            tokens.custom,
            "focusRingWidth",
            &spec->focusRingWidth);
        readInt(
            tokens.custom,
            "supportingLineHeight",
            &spec->supportingLineHeight);
        readBool(
            tokens.custom,
            "reserveSupportingLine",
            &spec->reserveSupportingLine);
    }

    spec->hasResolvedInputFont = false;
    if (tokens.typography.contains(spec->inputTypeRole)) {
        spec->inputFont =
            tokens.typography.value(
                spec->inputTypeRole).font;
        spec->hasResolvedInputFont = true;
    } else if (
        theme.typography().contains(
            spec->inputTypeRole)) {
        spec->inputFont =
            theme.typography().style(
                spec->inputTypeRole).font;
        spec->hasResolvedInputFont = true;
    }

    spec->hasResolvedLabelFont = false;
    if (tokens.typography.contains(spec->labelTypeRole)) {
        spec->labelFont =
            tokens.typography.value(
                spec->labelTypeRole).font;
        spec->hasResolvedLabelFont = true;
    } else if (
        theme.typography().contains(
            spec->labelTypeRole)) {
        spec->labelFont =
            theme.typography().style(
                spec->labelTypeRole).font;
        spec->hasResolvedLabelFont = true;
    }

    spec->hasResolvedSupportingFont = false;
    if (tokens.typography.contains(
            spec->supportingTextRole)) {
        spec->supportingFont =
            tokens.typography.value(
                spec->supportingTextRole).font;
        spec->hasResolvedSupportingFont = true;
    } else if (
        theme.typography().contains(
            spec->supportingTextRole)) {
        spec->supportingFont =
            theme.typography().style(
                spec->supportingTextRole).font;
        spec->hasResolvedSupportingFont = true;
    }

    if (spec->shapeRole == ShapeRole::Full) {
        spec->cornerRadius = -1.0;
    } else if (tokens.shapes.contains(
                   spec->shapeRole)) {
        spec->cornerRadius = qMax<qreal>(
            0.0,
            static_cast<qreal>(
                tokens.shapes.value(spec->shapeRole)));
    } else if (
        theme.shapes().contains(spec->shapeRole)) {
        spec->cornerRadius = qMax<qreal>(
            0.0,
            static_cast<qreal>(
                theme.shapes().radius(
                    spec->shapeRole)));
    }

    spec->hasResolvedMotionStyle = false;
    if (tokens.motion.contains(spec->motionToken)) {
        spec->motionStyle =
            tokens.motion.value(spec->motionToken);
        spec->hasResolvedMotionStyle = true;
    } else if (
        theme.motion().contains(spec->motionToken)) {
        spec->motionStyle =
            theme.motion().style(spec->motionToken);
        spec->hasResolvedMotionStyle = true;
    }

    const StateLayer& stateLayer =
        tokens.hasStateLayer
        ? tokens.stateLayer
        : theme.stateLayer();

    spec->hoverStateLayerOpacity =
        stateLayer.hoverOpacity;
    spec->focusStateLayerOpacity =
        stateLayer.focusOpacity;
    spec->pressStateLayerOpacity =
        stateLayer.pressOpacity;

    if (!tokens.isEmpty()) {
        readReal(
            tokens.custom,
            "cornerRadius",
            &spec->cornerRadius);
        readReal(
            tokens.custom,
            "hoverStateLayerOpacity",
            &spec->hoverStateLayerOpacity);
        readReal(
            tokens.custom,
            "focusStateLayerOpacity",
            &spec->focusStateLayerOpacity);
        readReal(
            tokens.custom,
            "pressStateLayerOpacity",
            &spec->pressStateLayerOpacity);
    }
}

void applyCardComponentTokens(
    const Theme& theme,
    const QStringList& componentNames,
    CardSpec* spec)
{
    if (!spec) {
        return;
    }

    const ComponentTokenOverride tokens =
        mergedComponentOverride(theme, componentNames);

    if (!tokens.isEmpty()) {
        applyColor(
            &spec->containerColor,
            tokens,
            ColorRole::SurfaceContainerLow);
        applyColor(
            &spec->outlineColor,
            tokens,
            ColorRole::OutlineVariant);
        applyColor(
            &spec->contentColor,
            tokens,
            ColorRole::OnSurface);

        applyCustomColor(
            &spec->containerColor,
            tokens,
            "containerColor");
        applyCustomColor(
            &spec->outlineColor,
            tokens,
            "outlineColor");
        applyCustomColor(
            &spec->contentColor,
            tokens,
            "contentColor");
        applyCustomColor(
            &spec->shadowColor,
            tokens,
            "shadowColor");

        MotionToken unusedMotion = MotionToken::Short4;
        applyShapeMotionElevation(
            tokens,
            &spec->shapeRole,
            &spec->elevationRole,
            &unusedMotion);

        readMargins(tokens.custom, &spec->contentPadding);
    }

    spec->shadowColor =
        spec->shadowColor.isValid()
        ? spec->shadowColor
        : theme.colorScheme().color(ColorRole::Shadow);

    spec->hasResolvedTitleFont = false;
    if (tokens.typography.contains(spec->titleTypeRole)) {
        spec->titleFont =
            tokens.typography.value(spec->titleTypeRole).font;
        spec->hasResolvedTitleFont = true;
    } else if (theme.typography().contains(spec->titleTypeRole)) {
        spec->titleFont =
            theme.typography().style(spec->titleTypeRole).font;
        spec->hasResolvedTitleFont = true;
    }

    spec->hasResolvedBodyFont = false;
    if (tokens.typography.contains(spec->bodyTypeRole)) {
        spec->bodyFont =
            tokens.typography.value(spec->bodyTypeRole).font;
        spec->hasResolvedBodyFont = true;
    } else if (theme.typography().contains(spec->bodyTypeRole)) {
        spec->bodyFont =
            theme.typography().style(spec->bodyTypeRole).font;
        spec->hasResolvedBodyFont = true;
    }

    if (spec->shapeRole == ShapeRole::Full) {
        spec->cornerRadius = -1.0;
    } else if (tokens.shapes.contains(spec->shapeRole)) {
        spec->cornerRadius =
            qMax<qreal>(0.0, tokens.shapes.value(spec->shapeRole));
    } else if (theme.shapes().contains(spec->shapeRole)) {
        spec->cornerRadius =
            qMax<qreal>(0.0, theme.shapes().radius(spec->shapeRole));
    }

    spec->hasResolvedElevationStyle = false;
    if (tokens.elevations.contains(spec->elevationRole)) {
        spec->elevationStyle =
            tokens.elevations.value(spec->elevationRole);
        spec->hasResolvedElevationStyle = true;
    } else if (theme.elevations().contains(spec->elevationRole)) {
        spec->elevationStyle =
            theme.elevations().style(spec->elevationRole);
        spec->hasResolvedElevationStyle = true;
    }

    const StateLayer& layer =
        tokens.hasStateLayer ? tokens.stateLayer : theme.stateLayer();
    spec->hoverStateLayerOpacity = layer.hoverOpacity;
    spec->focusStateLayerOpacity = layer.focusOpacity;
    spec->pressStateLayerOpacity = layer.pressOpacity;

    if (!tokens.isEmpty()) {
        readReal(tokens.custom, "cornerRadius", &spec->cornerRadius);
        readReal(
            tokens.custom,
            "hoverStateLayerOpacity",
            &spec->hoverStateLayerOpacity);
        readReal(
            tokens.custom,
            "focusStateLayerOpacity",
            &spec->focusStateLayerOpacity);
        readReal(
            tokens.custom,
            "pressStateLayerOpacity",
            &spec->pressStateLayerOpacity);
    }
}

void applyDialogComponentTokens(
    const Theme& theme,
    const QStringList& componentNames,
    DialogSpec* spec)
{
    if (!spec) {
        return;
    }

    const ComponentTokenOverride tokens =
        mergedComponentOverride(theme, componentNames);

    if (!tokens.isEmpty()) {
        applyColor(
            &spec->containerColor,
            tokens,
            ColorRole::SurfaceContainerHigh);
        applyColor(
            &spec->headlineColor,
            tokens,
            ColorRole::OnSurface);
        applyColor(
            &spec->bodyColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->scrimColor,
            tokens,
            ColorRole::Scrim);

        applyCustomColor(
            &spec->containerColor,
            tokens,
            "containerColor");
        applyCustomColor(
            &spec->headlineColor,
            tokens,
            "headlineColor");
        applyCustomColor(
            &spec->bodyColor,
            tokens,
            "bodyColor");
        applyCustomColor(
            &spec->scrimColor,
            tokens,
            "scrimColor");
        applyCustomColor(
            &spec->shadowColor,
            tokens,
            "shadowColor");

        applyShapeMotionElevation(
            tokens,
            &spec->shapeRole,
            &spec->elevationRole,
            &spec->enterMotion);

        if (tokens.custom.contains(
                QStringLiteral("enterMotion"))) {
            spec->enterMotion = parseMotionToken(
                tokens.custom
                    .value(QStringLiteral("enterMotion"))
                    .toString(),
                spec->enterMotion);
        }
        if (tokens.custom.contains(
                QStringLiteral("exitMotion"))) {
            spec->exitMotion = parseMotionToken(
                tokens.custom
                    .value(QStringLiteral("exitMotion"))
                    .toString(),
                spec->exitMotion);
        }

        readInt(tokens.custom, "maxWidth", &spec->maxWidth);
        readInt(tokens.custom, "padding", &spec->padding);
        readInt(
            tokens.custom,
            "headlineBottomSpacing",
            &spec->headlineBottomSpacing);
        readInt(
            tokens.custom,
            "actionsTopSpacing",
            &spec->actionsTopSpacing);
        readReal(
            tokens.custom,
            "scrimOpacity",
            &spec->scrimOpacity);
    }

    if (!spec->shadowColor.isValid()) {
        spec->shadowColor =
            theme.colorScheme().color(ColorRole::Shadow);
    }

    spec->hasResolvedHeadlineFont = false;
    if (tokens.typography.contains(spec->headlineTypeRole)) {
        spec->headlineFont =
            tokens.typography.value(spec->headlineTypeRole).font;
        spec->hasResolvedHeadlineFont = true;
    } else if (theme.typography().contains(spec->headlineTypeRole)) {
        spec->headlineFont =
            theme.typography().style(spec->headlineTypeRole).font;
        spec->hasResolvedHeadlineFont = true;
    }

    spec->hasResolvedBodyFont = false;
    if (tokens.typography.contains(spec->bodyTypeRole)) {
        spec->bodyFont =
            tokens.typography.value(spec->bodyTypeRole).font;
        spec->hasResolvedBodyFont = true;
    } else if (theme.typography().contains(spec->bodyTypeRole)) {
        spec->bodyFont =
            theme.typography().style(spec->bodyTypeRole).font;
        spec->hasResolvedBodyFont = true;
    }

    if (spec->shapeRole == ShapeRole::Full) {
        spec->cornerRadius = -1.0;
    } else if (tokens.shapes.contains(spec->shapeRole)) {
        spec->cornerRadius = qMax<qreal>(
            0.0,
            static_cast<qreal>(
                tokens.shapes.value(spec->shapeRole)));
    } else if (theme.shapes().contains(spec->shapeRole)) {
        spec->cornerRadius = qMax<qreal>(
            0.0,
            static_cast<qreal>(
                theme.shapes().radius(spec->shapeRole)));
    }

    spec->hasResolvedElevationStyle = false;
    if (tokens.elevations.contains(spec->elevationRole)) {
        spec->elevationStyle =
            tokens.elevations.value(spec->elevationRole);
        spec->hasResolvedElevationStyle = true;
    } else if (theme.elevations().contains(spec->elevationRole)) {
        spec->elevationStyle =
            theme.elevations().style(spec->elevationRole);
        spec->hasResolvedElevationStyle = true;
    }

    spec->hasResolvedEnterMotionStyle = false;
    if (tokens.motion.contains(spec->enterMotion)) {
        spec->enterMotionStyle =
            tokens.motion.value(spec->enterMotion);
        spec->hasResolvedEnterMotionStyle = true;
    } else if (theme.motion().contains(spec->enterMotion)) {
        spec->enterMotionStyle =
            theme.motion().style(spec->enterMotion);
        spec->hasResolvedEnterMotionStyle = true;
    }

    spec->hasResolvedExitMotionStyle = false;
    if (tokens.motion.contains(spec->exitMotion)) {
        spec->exitMotionStyle =
            tokens.motion.value(spec->exitMotion);
        spec->hasResolvedExitMotionStyle = true;
    } else if (theme.motion().contains(spec->exitMotion)) {
        spec->exitMotionStyle =
            theme.motion().style(spec->exitMotion);
        spec->hasResolvedExitMotionStyle = true;
    }

    if (!tokens.isEmpty()) {
        readReal(
            tokens.custom,
            "cornerRadius",
            &spec->cornerRadius);
    }

    spec->scrimOpacity = qBound<qreal>(
        0.0,
        spec->scrimOpacity,
        1.0);
    spec->scrimColor.setAlphaF(
        qBound<qreal>(
            0.0,
            spec->scrimColor.alphaF() * spec->scrimOpacity,
            1.0));
}

void applyChipComponentTokens(
    const Theme& theme,
    const QStringList& componentNames,
    ChipSpec* spec)
{
    if (!spec) {
        return;
    }

    const ComponentTokenOverride tokens =
        mergedComponentOverride(theme, componentNames);

    if (!tokens.isEmpty()) {
        applyColor(&spec->containerColor, tokens, ColorRole::SurfaceContainerLow);
        applyColor(
            &spec->selectedContainerColor,
            tokens,
            ColorRole::SecondaryContainer);
        applyColor(&spec->labelColor, tokens, ColorRole::OnSurface);
        applyColor(
            &spec->selectedLabelColor,
            tokens,
            ColorRole::OnSecondaryContainer);
        applyColor(&spec->iconColor, tokens, ColorRole::Primary);
        applyColor(
            &spec->selectedIconColor,
            tokens,
            ColorRole::OnSecondaryContainer);
        applyColor(&spec->outlineColor, tokens, ColorRole::Outline);
        applyColor(
            &spec->disabledContainerColor,
            tokens,
            ColorRole::SurfaceContainerHigh);
        applyColor(
            &spec->disabledLabelColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->disabledOutlineColor,
            tokens,
            ColorRole::OutlineVariant);
        applyColor(&spec->stateLayerColor, tokens, ColorRole::OnSurface);
        applyColor(&spec->focusRingColor, tokens, ColorRole::Primary);

        applyCustomColor(&spec->containerColor, tokens, "containerColor");
        applyCustomColor(
            &spec->selectedContainerColor,
            tokens,
            "selectedContainerColor");
        applyCustomColor(&spec->labelColor, tokens, "labelColor");
        applyCustomColor(
            &spec->selectedLabelColor,
            tokens,
            "selectedLabelColor");
        applyCustomColor(&spec->iconColor, tokens, "iconColor");
        applyCustomColor(
            &spec->selectedIconColor,
            tokens,
            "selectedIconColor");
        applyCustomColor(&spec->outlineColor, tokens, "outlineColor");
        applyCustomColor(
            &spec->disabledContainerColor,
            tokens,
            "disabledContainerColor");
        applyCustomColor(
            &spec->disabledLabelColor,
            tokens,
            "disabledLabelColor");
        applyCustomColor(
            &spec->disabledOutlineColor,
            tokens,
            "disabledOutlineColor");
        applyCustomColor(
            &spec->stateLayerColor,
            tokens,
            "stateLayerColor");
        applyCustomColor(
            &spec->focusRingColor,
            tokens,
            "focusRingColor");

        applyShapeMotionElevation(
            tokens,
            &spec->shapeRole,
            &spec->elevationRole,
            &spec->motionToken);
        applyTouchTarget(&spec->touchTarget, tokens);
        spec->iconSize =
            iconSizeFromTokens(tokens, IconSizeRole::Small, spec->iconSize);
        readInt(tokens.custom, "containerHeight", &spec->containerHeight);
        readInt(tokens.custom, "minWidth", &spec->minWidth);
        readInt(tokens.custom, "horizontalPadding", &spec->horizontalPadding);
        readInt(tokens.custom, "iconSize", &spec->iconSize);
        readInt(tokens.custom, "iconSpacing", &spec->iconSpacing);
        readInt(tokens.custom, "outlineWidth", &spec->outlineWidth);
    }

    spec->hasResolvedLabelFont = false;
    if (tokens.typography.contains(spec->labelTypeRole)) {
        spec->labelFont =
            tokens.typography.value(spec->labelTypeRole).font;
        spec->hasResolvedLabelFont = true;
    } else if (theme.typography().contains(spec->labelTypeRole)) {
        spec->labelFont =
            theme.typography().style(spec->labelTypeRole).font;
        spec->hasResolvedLabelFont = true;
    }

    if (spec->shapeRole == ShapeRole::Full) {
        spec->cornerRadius = -1.0;
    } else if (tokens.shapes.contains(spec->shapeRole)) {
        spec->cornerRadius = qMax<qreal>(
            0.0,
            static_cast<qreal>(tokens.shapes.value(spec->shapeRole)));
    } else if (theme.shapes().contains(spec->shapeRole)) {
        spec->cornerRadius = qMax<qreal>(
            0.0,
            static_cast<qreal>(theme.shapes().radius(spec->shapeRole)));
    } else {
        spec->cornerRadius = -1.0;
    }

    spec->hasResolvedMotionStyle = false;
    if (tokens.motion.contains(spec->motionToken)) {
        spec->motionStyle = tokens.motion.value(spec->motionToken);
        spec->hasResolvedMotionStyle = true;
    } else if (theme.motion().contains(spec->motionToken)) {
        spec->motionStyle = theme.motion().style(spec->motionToken);
        spec->hasResolvedMotionStyle = true;
    }

    const StateLayer& stateLayer =
        tokens.hasStateLayer ? tokens.stateLayer : theme.stateLayer();
    spec->hoverStateLayerOpacity = stateLayer.hoverOpacity;
    spec->focusStateLayerOpacity = stateLayer.focusOpacity;
    spec->pressStateLayerOpacity = stateLayer.pressOpacity;
    spec->dragStateLayerOpacity = stateLayer.dragOpacity;

    if (!tokens.isEmpty()) {
        readReal(tokens.custom, "cornerRadius", &spec->cornerRadius);
        readReal(
            tokens.custom,
            "hoverStateLayerOpacity",
            &spec->hoverStateLayerOpacity);
        readReal(
            tokens.custom,
            "focusStateLayerOpacity",
            &spec->focusStateLayerOpacity);
        readReal(
            tokens.custom,
            "pressStateLayerOpacity",
            &spec->pressStateLayerOpacity);
        readReal(
            tokens.custom,
            "dragStateLayerOpacity",
            &spec->dragStateLayerOpacity);
    }
}

void applySegmentedButtonComponentTokens(
    const Theme& theme,
    const QStringList& componentNames,
    SegmentedButtonSpec* spec)
{
    if (!spec) {
        return;
    }

    const ComponentTokenOverride tokens =
        mergedComponentOverride(theme, componentNames);

    if (!tokens.isEmpty()) {
        applyColor(&spec->containerColor, tokens, ColorRole::Surface);
        applyColor(
            &spec->selectedContainerColor,
            tokens,
            ColorRole::SecondaryContainer);
        applyColor(&spec->labelColor, tokens, ColorRole::OnSurface);
        applyColor(
            &spec->selectedLabelColor,
            tokens,
            ColorRole::OnSecondaryContainer);
        applyColor(&spec->outlineColor, tokens, ColorRole::Outline);
        applyColor(&spec->stateLayerColor, tokens, ColorRole::OnSurface);
        applyColor(&spec->focusRingColor, tokens, ColorRole::Primary);
        applyColor(
            &spec->disabledLabelColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->disabledOutlineColor,
            tokens,
            ColorRole::OutlineVariant);

        applyCustomColor(&spec->containerColor, tokens, "containerColor");
        applyCustomColor(
            &spec->selectedContainerColor,
            tokens,
            "selectedContainerColor");
        applyCustomColor(&spec->labelColor, tokens, "labelColor");
        applyCustomColor(
            &spec->selectedLabelColor,
            tokens,
            "selectedLabelColor");
        applyCustomColor(&spec->outlineColor, tokens, "outlineColor");
        applyCustomColor(
            &spec->stateLayerColor,
            tokens,
            "stateLayerColor");
        applyCustomColor(
            &spec->focusRingColor,
            tokens,
            "focusRingColor");
        applyCustomColor(
            &spec->disabledLabelColor,
            tokens,
            "disabledLabelColor");
        applyCustomColor(
            &spec->disabledOutlineColor,
            tokens,
            "disabledOutlineColor");

        ElevationRole unusedElevation = ElevationRole::Level0;
        applyShapeMotionElevation(
            tokens,
            &spec->shapeRole,
            &unusedElevation,
            &spec->motionToken);
        applyTouchTarget(&spec->touchTarget, tokens);
        spec->iconSize =
            iconSizeFromTokens(tokens, IconSizeRole::Small, spec->iconSize);
        readInt(tokens.custom, "segmentHeight", &spec->segmentHeight);
        readInt(tokens.custom, "minSegmentWidth", &spec->minSegmentWidth);
        readInt(tokens.custom, "horizontalPadding", &spec->horizontalPadding);
        readInt(tokens.custom, "iconSize", &spec->iconSize);
        readInt(tokens.custom, "iconSpacing", &spec->iconSpacing);
        readInt(tokens.custom, "outlineWidth", &spec->outlineWidth);
    }

    spec->hasResolvedLabelFont = false;
    if (tokens.typography.contains(spec->labelTypeRole)) {
        spec->labelFont =
            tokens.typography.value(spec->labelTypeRole).font;
        spec->hasResolvedLabelFont = true;
    } else if (theme.typography().contains(spec->labelTypeRole)) {
        spec->labelFont =
            theme.typography().style(spec->labelTypeRole).font;
        spec->hasResolvedLabelFont = true;
    }

    if (spec->shapeRole == ShapeRole::Full) {
        spec->cornerRadius = -1.0;
    } else if (tokens.shapes.contains(spec->shapeRole)) {
        spec->cornerRadius = qMax<qreal>(
            0.0,
            static_cast<qreal>(tokens.shapes.value(spec->shapeRole)));
    } else if (theme.shapes().contains(spec->shapeRole)) {
        spec->cornerRadius = qMax<qreal>(
            0.0,
            static_cast<qreal>(theme.shapes().radius(spec->shapeRole)));
    } else {
        spec->cornerRadius = -1.0;
    }

    spec->hasResolvedMotionStyle = false;
    if (tokens.motion.contains(spec->motionToken)) {
        spec->motionStyle = tokens.motion.value(spec->motionToken);
        spec->hasResolvedMotionStyle = true;
    } else if (theme.motion().contains(spec->motionToken)) {
        spec->motionStyle = theme.motion().style(spec->motionToken);
        spec->hasResolvedMotionStyle = true;
    }

    const StateLayer& stateLayer =
        tokens.hasStateLayer ? tokens.stateLayer : theme.stateLayer();
    spec->hoverStateLayerOpacity = stateLayer.hoverOpacity;
    spec->focusStateLayerOpacity = stateLayer.focusOpacity;
    spec->pressStateLayerOpacity = stateLayer.pressOpacity;
    spec->dragStateLayerOpacity = stateLayer.dragOpacity;

    if (!tokens.isEmpty()) {
        readReal(tokens.custom, "cornerRadius", &spec->cornerRadius);
        readReal(
            tokens.custom,
            "hoverStateLayerOpacity",
            &spec->hoverStateLayerOpacity);
        readReal(
            tokens.custom,
            "focusStateLayerOpacity",
            &spec->focusStateLayerOpacity);
        readReal(
            tokens.custom,
            "pressStateLayerOpacity",
            &spec->pressStateLayerOpacity);
        readReal(
            tokens.custom,
            "dragStateLayerOpacity",
            &spec->dragStateLayerOpacity);
    }
}

void applyAutocompleteComponentTokens(
    const Theme& theme,
    const QStringList& componentNames,
    AutocompleteSpec* spec)
{
    if (!spec) {
        return;
    }

    const ComponentTokenOverride tokens =
        mergedComponentOverride(theme, componentNames);

    if (!tokens.isEmpty()) {
        applyColor(
            &spec->inputContainerColor,
            tokens,
            ColorRole::SurfaceContainerHighest);
        applyColor(
            &spec->popupContainerColor,
            tokens,
            ColorRole::SurfaceContainerHigh);
        applyColor(&spec->inputTextColor, tokens, ColorRole::OnSurface);
        applyColor(
            &spec->suggestionTextColor,
            tokens,
            ColorRole::OnSurface);
        applyColor(
            &spec->placeholderColor,
            tokens,
            ColorRole::OnSurfaceVariant);
        applyColor(
            &spec->selectedSuggestionContainerColor,
            tokens,
            ColorRole::SecondaryContainer);
        applyColor(
            &spec->selectedSuggestionTextColor,
            tokens,
            ColorRole::OnSecondaryContainer);
        applyColor(
            &spec->stateLayerColor,
            tokens,
            ColorRole::OnSurface);
        applyColor(&spec->outlineColor, tokens, ColorRole::Outline);
        applyColor(
            &spec->focusedOutlineColor,
            tokens,
            ColorRole::Primary);
        applyColor(&spec->focusRingColor, tokens, ColorRole::Primary);
        applyColor(
            &spec->disabledTextColor,
            tokens,
            ColorRole::OnSurfaceVariant);

        applyCustomColor(
            &spec->inputContainerColor,
            tokens,
            "inputContainerColor");
        applyCustomColor(
            &spec->popupContainerColor,
            tokens,
            "popupContainerColor");
        applyCustomColor(
            &spec->inputTextColor,
            tokens,
            "inputTextColor");
        applyCustomColor(
            &spec->suggestionTextColor,
            tokens,
            "suggestionTextColor");
        applyCustomColor(
            &spec->placeholderColor,
            tokens,
            "placeholderColor");
        applyCustomColor(
            &spec->selectedSuggestionContainerColor,
            tokens,
            "selectedSuggestionContainerColor");
        applyCustomColor(
            &spec->selectedSuggestionTextColor,
            tokens,
            "selectedSuggestionTextColor");
        applyCustomColor(
            &spec->stateLayerColor,
            tokens,
            "stateLayerColor");
        applyCustomColor(&spec->outlineColor, tokens, "outlineColor");
        applyCustomColor(
            &spec->focusedOutlineColor,
            tokens,
            "focusedOutlineColor");
        applyCustomColor(
            &spec->focusRingColor,
            tokens,
            "focusRingColor");
        applyCustomColor(
            &spec->disabledTextColor,
            tokens,
            "disabledTextColor");

        if (tokens.custom.contains(QStringLiteral("inputShapeRole"))) {
            spec->inputShapeRole = parseShapeRole(
                tokens.custom.value(
                    QStringLiteral("inputShapeRole")).toString(),
                spec->inputShapeRole);
        }
        if (tokens.custom.contains(QStringLiteral("popupShapeRole"))) {
            spec->popupShapeRole = parseShapeRole(
                tokens.custom.value(
                    QStringLiteral("popupShapeRole")).toString(),
                spec->popupShapeRole);
        }
        if (tokens.custom.contains(QStringLiteral("popupElevationRole"))) {
            spec->popupElevationRole = parseElevationRole(
                tokens.custom.value(
                    QStringLiteral("popupElevationRole")).toString(),
                spec->popupElevationRole);
        }
        if (tokens.custom.contains(QStringLiteral("popupMotion"))) {
            spec->popupMotion = parseMotionToken(
                tokens.custom.value(
                    QStringLiteral("popupMotion")).toString(),
                spec->popupMotion);
        }

        readInt(tokens.custom, "inputMinHeight", &spec->inputMinHeight);
        readInt(
            tokens.custom,
            "popupVisibleItemCount",
            &spec->popupVisibleItemCount);
        readInt(
            tokens.custom,
            "horizontalPadding",
            &spec->horizontalPadding);
        readInt(tokens.custom, "verticalInset", &spec->verticalInset);
        readReal(tokens.custom, "outlineWidth", &spec->outlineWidth);
        readReal(
            tokens.custom,
            "focusedOutlineWidth",
            &spec->focusedOutlineWidth);
        readReal(
            tokens.custom,
            "focusRingWidth",
            &spec->focusRingWidth);
    }

    spec->hasResolvedInputFont = false;
    if (tokens.typography.contains(spec->inputTypeRole)) {
        spec->inputFont =
            tokens.typography.value(spec->inputTypeRole).font;
        spec->hasResolvedInputFont = true;
    } else if (theme.typography().contains(spec->inputTypeRole)) {
        spec->inputFont =
            theme.typography().style(spec->inputTypeRole).font;
        spec->hasResolvedInputFont = true;
    }

    spec->hasResolvedSuggestionFont = false;
    if (tokens.typography.contains(spec->suggestionTypeRole)) {
        spec->suggestionFont =
            tokens.typography.value(spec->suggestionTypeRole).font;
        spec->hasResolvedSuggestionFont = true;
    } else if (theme.typography().contains(spec->suggestionTypeRole)) {
        spec->suggestionFont =
            theme.typography().style(spec->suggestionTypeRole).font;
        spec->hasResolvedSuggestionFont = true;
    }

    if (tokens.shapes.contains(spec->inputShapeRole)) {
        spec->inputCornerRadius = qMax<qreal>(
            0.0,
            static_cast<qreal>(
                tokens.shapes.value(spec->inputShapeRole)));
    } else if (theme.shapes().contains(spec->inputShapeRole)) {
        spec->inputCornerRadius = qMax<qreal>(
            0.0,
            static_cast<qreal>(
                theme.shapes().radius(spec->inputShapeRole)));
    }

    if (tokens.shapes.contains(spec->popupShapeRole)) {
        spec->popupCornerRadius = qMax<qreal>(
            0.0,
            static_cast<qreal>(
                tokens.shapes.value(spec->popupShapeRole)));
    } else if (theme.shapes().contains(spec->popupShapeRole)) {
        spec->popupCornerRadius = qMax<qreal>(
            0.0,
            static_cast<qreal>(
                theme.shapes().radius(spec->popupShapeRole)));
    }

    spec->hasResolvedPopupMotionStyle = false;
    if (tokens.motion.contains(spec->popupMotion)) {
        spec->popupMotionStyle =
            tokens.motion.value(spec->popupMotion);
        spec->hasResolvedPopupMotionStyle = true;
    } else if (theme.motion().contains(spec->popupMotion)) {
        spec->popupMotionStyle =
            theme.motion().style(spec->popupMotion);
        spec->hasResolvedPopupMotionStyle = true;
    }

    spec->hasResolvedPopupElevationStyle = false;
    if (tokens.elevations.contains(spec->popupElevationRole)) {
        spec->popupElevationStyle =
            tokens.elevations.value(spec->popupElevationRole);
        spec->hasResolvedPopupElevationStyle = true;
    } else if (theme.elevations().contains(spec->popupElevationRole)) {
        spec->popupElevationStyle =
            theme.elevations().style(spec->popupElevationRole);
        spec->hasResolvedPopupElevationStyle = true;
    }

    const StateLayer& stateLayer =
        tokens.hasStateLayer ? tokens.stateLayer : theme.stateLayer();
    spec->hoverStateLayerOpacity = stateLayer.hoverOpacity;
    spec->focusStateLayerOpacity = stateLayer.focusOpacity;
    spec->pressStateLayerOpacity = stateLayer.pressOpacity;

    if (!tokens.isEmpty()) {
        readReal(
            tokens.custom,
            "inputCornerRadius",
            &spec->inputCornerRadius);
        readReal(
            tokens.custom,
            "popupCornerRadius",
            &spec->popupCornerRadius);
        readReal(
            tokens.custom,
            "hoverStateLayerOpacity",
            &spec->hoverStateLayerOpacity);
        readReal(
            tokens.custom,
            "focusStateLayerOpacity",
            &spec->focusStateLayerOpacity);
        readReal(
            tokens.custom,
            "pressStateLayerOpacity",
            &spec->pressStateLayerOpacity);
    }
}

void applyMenuComponentTokens(
    const Theme& theme,
    const QStringList& componentNames,
    MenuSpec* spec)
{
    if (!spec) {
        return;
    }

    const ComponentTokenOverride tokens =
        mergedComponentOverride(theme, componentNames);

    spec->labelFont = theme.typography().style(
        spec->labelTypeRole).font;
    spec->shortcutFont = theme.typography().style(
        spec->shortcutTypeRole).font;

    if (theme.shapes().contains(spec->shapeRole)) {
        spec->cornerRadius = qMax<qreal>(
            0.0,
            theme.shapes().radius(spec->shapeRole));
    }
    if (theme.elevations().contains(spec->elevationRole)) {
        spec->elevationStyle =
            theme.elevations().style(spec->elevationRole);
    }
    if (theme.motion().contains(spec->enterMotion)) {
        spec->enterMotionStyle =
            theme.motion().style(spec->enterMotion);
    }
    if (theme.motion().contains(spec->exitMotion)) {
        spec->exitMotionStyle =
            theme.motion().style(spec->exitMotion);
    }

    const StateLayer& defaultStateLayer = theme.stateLayer();
    spec->hoverStateLayerOpacity =
        defaultStateLayer.hoverOpacity;
    spec->focusStateLayerOpacity =
        defaultStateLayer.focusOpacity;
    spec->pressStateLayerOpacity =
        defaultStateLayer.pressOpacity;

    spec->disabledItemLabelColor.setAlpha(97);
    spec->disabledItemIconColor.setAlpha(97);
    spec->disabledShortcutColor.setAlpha(97);

    if (tokens.isEmpty()) {
        return;
    }

    applyColor(
        &spec->containerColor,
        tokens,
        ColorRole::SurfaceContainer);
    applyColor(
        &spec->itemLabelColor,
        tokens,
        ColorRole::OnSurface);
    applyColor(
        &spec->itemIconColor,
        tokens,
        ColorRole::OnSurfaceVariant);
    applyColor(
        &spec->disabledItemLabelColor,
        tokens,
        ColorRole::OnSurfaceVariant);
    applyColor(
        &spec->disabledItemIconColor,
        tokens,
        ColorRole::OnSurfaceVariant);
    applyColor(
        &spec->stateLayerColor,
        tokens,
        ColorRole::OnSurface);
    applyColor(
        &spec->focusRingColor,
        tokens,
        ColorRole::Primary);
    applyColor(
        &spec->dividerColor,
        tokens,
        ColorRole::OutlineVariant);
    applyColor(
        &spec->shortcutColor,
        tokens,
        ColorRole::OnSurfaceVariant);
    applyColor(
        &spec->disabledShortcutColor,
        tokens,
        ColorRole::OnSurfaceVariant);
    applyColor(
        &spec->checkedIndicatorColor,
        tokens,
        ColorRole::Primary);

    applyCustomColor(
        &spec->containerColor, tokens, "containerColor");
    applyCustomColor(
        &spec->itemLabelColor, tokens, "itemLabelColor");
    applyCustomColor(
        &spec->itemIconColor, tokens, "itemIconColor");
    applyCustomColor(
        &spec->disabledItemLabelColor,
        tokens,
        "disabledItemLabelColor");
    applyCustomColor(
        &spec->disabledItemIconColor,
        tokens,
        "disabledItemIconColor");
    applyCustomColor(
        &spec->stateLayerColor, tokens, "stateLayerColor");
    applyCustomColor(
        &spec->focusRingColor, tokens, "focusRingColor");
    applyCustomColor(
        &spec->dividerColor, tokens, "dividerColor");
    applyCustomColor(
        &spec->shortcutColor, tokens, "shortcutColor");
    applyCustomColor(
        &spec->disabledShortcutColor,
        tokens,
        "disabledShortcutColor");
    applyCustomColor(
        &spec->checkedIndicatorColor,
        tokens,
        "checkedIndicatorColor");
    applyCustomColor(
        &spec->shadowColor, tokens, "shadowColor");

    if (tokens.typography.contains(spec->labelTypeRole)) {
        spec->labelFont =
            tokens.typography.value(spec->labelTypeRole).font;
    }
    if (tokens.typography.contains(spec->shortcutTypeRole)) {
        spec->shortcutFont =
            tokens.typography.value(spec->shortcutTypeRole).font;
    }
    if (tokens.shapes.contains(spec->shapeRole)) {
        spec->cornerRadius = qMax<qreal>(
            0.0,
            tokens.shapes.value(spec->shapeRole));
    }
    if (tokens.elevations.contains(spec->elevationRole)) {
        spec->elevationStyle =
            tokens.elevations.value(spec->elevationRole);
    }
    if (tokens.motion.contains(spec->enterMotion)) {
        spec->enterMotionStyle =
            tokens.motion.value(spec->enterMotion);
    }
    if (tokens.motion.contains(spec->exitMotion)) {
        spec->exitMotionStyle =
            tokens.motion.value(spec->exitMotion);
    }

    const StateLayer& stateLayer =
        tokens.hasStateLayer
            ? tokens.stateLayer
            : defaultStateLayer;
    spec->hoverStateLayerOpacity = stateLayer.hoverOpacity;
    spec->focusStateLayerOpacity = stateLayer.focusOpacity;
    spec->pressStateLayerOpacity = stateLayer.pressOpacity;

    int minWidth = spec->minItemSize.width();
    int minHeight = spec->minItemSize.height();
    int maxWidth = spec->maxPopupSize.width();
    int maxHeight = spec->maxPopupSize.height();
    int paddingLeft = spec->itemPadding.left();
    int paddingTop = spec->itemPadding.top();
    int paddingRight = spec->itemPadding.right();
    int paddingBottom = spec->itemPadding.bottom();

    readInt(tokens.custom, "minItemWidth", &minWidth);
    readInt(tokens.custom, "minItemHeight", &minHeight);
    readInt(tokens.custom, "maxPopupWidth", &maxWidth);
    readInt(tokens.custom, "maxPopupHeight", &maxHeight);
    readInt(tokens.custom, "paddingLeft", &paddingLeft);
    readInt(tokens.custom, "paddingTop", &paddingTop);
    readInt(tokens.custom, "paddingRight", &paddingRight);
    readInt(tokens.custom, "paddingBottom", &paddingBottom);
    readInt(tokens.custom, "iconSize", &spec->iconSize);
    readInt(tokens.custom, "iconSpacing", &spec->iconSpacing);
    readReal(tokens.custom, "cornerRadius", &spec->cornerRadius);
    readInt(
        tokens.custom,
        "separatorHeight",
        &spec->separatorHeight);
    readInt(
        tokens.custom,
        "verticalPadding",
        &spec->verticalPadding);
    readInt(
        tokens.custom,
        "checkColumnWidth",
        &spec->checkColumnWidth);
    readInt(
        tokens.custom,
        "shortcutSpacing",
        &spec->shortcutSpacing);
    readInt(
        tokens.custom,
        "dividerInset",
        &spec->dividerInset);
    readInt(
        tokens.custom,
        "focusRingWidth",
        &spec->focusRingWidth);
    readReal(
        tokens.custom,
        "hoverStateLayerOpacity",
        &spec->hoverStateLayerOpacity);
    readReal(
        tokens.custom,
        "focusStateLayerOpacity",
        &spec->focusStateLayerOpacity);
    readReal(
        tokens.custom,
        "pressStateLayerOpacity",
        &spec->pressStateLayerOpacity);

    spec->minItemSize = QSize(
        qMax(48, minWidth),
        qMax(32, minHeight));
    spec->maxPopupSize = QSize(
        qMax(spec->minItemSize.width(), maxWidth),
        qMax(spec->minItemSize.height(), maxHeight));
    spec->itemPadding = QMargins(
        qMax(0, paddingLeft),
        qMax(0, paddingTop),
        qMax(0, paddingRight),
        qMax(0, paddingBottom));
    spec->iconSize = qMax(0, spec->iconSize);
    spec->iconSpacing = qMax(0, spec->iconSpacing);
    spec->cornerRadius = qMax<qreal>(0.0, spec->cornerRadius);
    spec->separatorHeight = qMax(1, spec->separatorHeight);
    spec->verticalPadding = qMax(0, spec->verticalPadding);
    spec->checkColumnWidth = qMax(0, spec->checkColumnWidth);
    spec->shortcutSpacing = qMax(0, spec->shortcutSpacing);
    spec->dividerInset = qMax(0, spec->dividerInset);
    spec->focusRingWidth = qMax(1, spec->focusRingWidth);
}

void applySnackbarComponentTokens(
    const Theme& theme,
    const QStringList& componentNames,
    SnackbarSpec* spec)
{
    if (!spec) {
        return;
    }

    const ComponentTokenOverride tokens =
        mergedComponentOverride(theme, componentNames);

    if (!tokens.isEmpty()) {
        applyColor(
            &spec->containerColor,
            tokens,
            ColorRole::InverseSurface);
        applyColor(
            &spec->textColor,
            tokens,
            ColorRole::InverseOnSurface);
        applyColor(
            &spec->actionColor,
            tokens,
            ColorRole::InversePrimary);
        applyColor(
            &spec->dismissIconColor,
            tokens,
            ColorRole::InverseOnSurface);
        applyColor(
            &spec->shadowColor,
            tokens,
            ColorRole::Shadow);

        applyCustomColor(
            &spec->containerColor,
            tokens,
            "containerColor");
        applyCustomColor(
            &spec->textColor,
            tokens,
            "textColor");
        applyCustomColor(
            &spec->actionColor,
            tokens,
            "actionColor");
        applyCustomColor(
            &spec->dismissIconColor,
            tokens,
            "dismissIconColor");
        applyCustomColor(
            &spec->shadowColor,
            tokens,
            "shadowColor");

        applyShapeMotionElevation(
            tokens,
            &spec->shapeRole,
            &spec->elevationRole,
            &spec->enterMotion);

        if (tokens.custom.contains(
                QStringLiteral("exitMotion"))) {
            spec->exitMotion = parseMotionToken(
                tokens.custom.value(
                    QStringLiteral("exitMotion")).toString(),
                spec->exitMotion);
        }

        int outerLeft = spec->outerMargins.left();
        int outerTop = spec->outerMargins.top();
        int outerRight = spec->outerMargins.right();
        int outerBottom = spec->outerMargins.bottom();
        int contentLeft = spec->contentPadding.left();
        int contentTop = spec->contentPadding.top();
        int contentRight = spec->contentPadding.right();
        int contentBottom = spec->contentPadding.bottom();

        readInt(tokens.custom, "outerMarginLeft", &outerLeft);
        readInt(tokens.custom, "outerMarginTop", &outerTop);
        readInt(tokens.custom, "outerMarginRight", &outerRight);
        readInt(tokens.custom, "outerMarginBottom", &outerBottom);
        readInt(tokens.custom, "contentPaddingLeft", &contentLeft);
        readInt(tokens.custom, "contentPaddingTop", &contentTop);
        readInt(tokens.custom, "contentPaddingRight", &contentRight);
        readInt(tokens.custom, "contentPaddingBottom", &contentBottom);

        readInt(tokens.custom, "minHeight", &spec->minHeight);
        readInt(tokens.custom, "minWidth", &spec->minWidth);
        readInt(tokens.custom, "maxWidth", &spec->maxWidth);
        readInt(tokens.custom, "labelMinWidth", &spec->labelMinWidth);
        readInt(
            tokens.custom,
            "controlsReserveSpacing",
            &spec->controlsReserveSpacing);
        readInt(
            tokens.custom,
            "slideDistance",
            &spec->slideDistance);
        readInt(
            tokens.custom,
            "actionSpacing",
            &spec->actionSpacing);
        readInt(
            tokens.custom,
            "actionMinWidth",
            &spec->actionMinWidth);
        readInt(
            tokens.custom,
            "actionPaddingHorizontal",
            &spec->actionPaddingHorizontal);
        readInt(
            tokens.custom,
            "actionPaddingVertical",
            &spec->actionPaddingVertical);
        readInt(
            tokens.custom,
            "actionCornerRadius",
            &spec->actionCornerRadius);
        readInt(
            tokens.custom,
            "dismissButtonSize",
            &spec->dismissButtonSize);
        readInt(
            tokens.custom,
            "dismissCornerRadius",
            &spec->dismissCornerRadius);
        readInt(
            tokens.custom,
            "shortDurationMs",
            &spec->shortDurationMs);
        readInt(
            tokens.custom,
            "longDurationMs",
            &spec->longDurationMs);
        readReal(
            tokens.custom,
            "cornerRadius",
            &spec->cornerRadius);

        spec->outerMargins = QMargins(
            qMax(0, outerLeft),
            qMax(0, outerTop),
            qMax(0, outerRight),
            qMax(0, outerBottom));
        spec->contentPadding = QMargins(
            qMax(0, contentLeft),
            qMax(0, contentTop),
            qMax(0, contentRight),
            qMax(0, contentBottom));
    }

    spec->hasResolvedTextFont = false;
    if (tokens.typography.contains(spec->textTypeRole)) {
        spec->textFont =
            tokens.typography.value(spec->textTypeRole).font;
        spec->hasResolvedTextFont = true;
    } else if (theme.typography().contains(spec->textTypeRole)) {
        spec->textFont =
            theme.typography().style(spec->textTypeRole).font;
        spec->hasResolvedTextFont = true;
    }

    spec->hasResolvedActionFont = false;
    if (tokens.typography.contains(spec->actionTypeRole)) {
        spec->actionFont =
            tokens.typography.value(spec->actionTypeRole).font;
        spec->hasResolvedActionFont = true;
    } else if (theme.typography().contains(spec->actionTypeRole)) {
        spec->actionFont =
            theme.typography().style(spec->actionTypeRole).font;
        spec->hasResolvedActionFont = true;
    }

    if (tokens.shapes.contains(spec->shapeRole)) {
        spec->cornerRadius = qMax<qreal>(
            0.0,
            tokens.shapes.value(spec->shapeRole));
    } else if (theme.shapes().contains(spec->shapeRole)) {
        spec->cornerRadius = qMax<qreal>(
            0.0,
            theme.shapes().radius(spec->shapeRole));
    }

    spec->hasResolvedElevationStyle = false;
    if (tokens.elevations.contains(spec->elevationRole)) {
        spec->elevationStyle =
            tokens.elevations.value(spec->elevationRole);
        spec->hasResolvedElevationStyle = true;
    } else if (theme.elevations().contains(spec->elevationRole)) {
        spec->elevationStyle =
            theme.elevations().style(spec->elevationRole);
        spec->hasResolvedElevationStyle = true;
    }

    spec->hasResolvedEnterMotion = false;
    if (tokens.motion.contains(spec->enterMotion)) {
        spec->enterMotionStyle =
            tokens.motion.value(spec->enterMotion);
        spec->hasResolvedEnterMotion = true;
    } else if (theme.motion().contains(spec->enterMotion)) {
        spec->enterMotionStyle =
            theme.motion().style(spec->enterMotion);
        spec->hasResolvedEnterMotion = true;
    }

    spec->hasResolvedExitMotion = false;
    if (tokens.motion.contains(spec->exitMotion)) {
        spec->exitMotionStyle =
            tokens.motion.value(spec->exitMotion);
        spec->hasResolvedExitMotion = true;
    } else if (theme.motion().contains(spec->exitMotion)) {
        spec->exitMotionStyle =
            theme.motion().style(spec->exitMotion);
        spec->hasResolvedExitMotion = true;
    }

    if (!tokens.isEmpty()) {
        readReal(
            tokens.custom,
            "cornerRadius",
            &spec->cornerRadius);
    }

    spec->minHeight = qMax(32, spec->minHeight);
    spec->minWidth = qMax(80, spec->minWidth);
    spec->maxWidth = qMax(spec->minWidth, spec->maxWidth);
    spec->labelMinWidth = qMax(32, spec->labelMinWidth);
    spec->controlsReserveSpacing =
        qMax(0, spec->controlsReserveSpacing);
    spec->slideDistance = qMax(0, spec->slideDistance);
    spec->actionSpacing = qMax(0, spec->actionSpacing);
    spec->actionMinWidth = qMax(0, spec->actionMinWidth);
    spec->actionPaddingHorizontal =
        qMax(0, spec->actionPaddingHorizontal);
    spec->actionPaddingVertical =
        qMax(0, spec->actionPaddingVertical);
    spec->actionCornerRadius =
        qMax(0, spec->actionCornerRadius);
    spec->dismissButtonSize =
        qMax(24, spec->dismissButtonSize);
    spec->dismissCornerRadius =
        qMax(0, spec->dismissCornerRadius);
    spec->shortDurationMs =
        qMax(1, spec->shortDurationMs);
    spec->longDurationMs =
        qMax(spec->shortDurationMs, spec->longDurationMs);
}

} // namespace QtMaterial
