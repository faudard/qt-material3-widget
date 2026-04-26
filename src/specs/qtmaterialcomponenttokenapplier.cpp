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

void applyButtonComponentTokens(const Theme& theme, const QStringList& componentNames, ButtonSpec* spec)
{
    if (!spec) return;
    const ComponentTokenOverride tokens = mergedComponentOverride(theme, componentNames);
    if (tokens.isEmpty()) return;

    applyColor(&spec->containerColor, tokens, ColorRole::Primary);
    applyColor(&spec->labelColor, tokens, ColorRole::OnPrimary);
    applyColor(&spec->iconColor, tokens, ColorRole::OnPrimary);
    applyColor(&spec->disabledContainerColor, tokens, ColorRole::SurfaceContainerHigh);
    applyColor(&spec->disabledLabelColor, tokens, ColorRole::OnSurfaceVariant);
    applyColor(&spec->stateLayerColor, tokens, ColorRole::SurfaceTint);
    applyColor(&spec->focusRingColor, tokens, ColorRole::Primary);
    applyColor(&spec->outlineColor, tokens, ColorRole::Outline);
    applyColor(&spec->disabledOutlineColor, tokens, ColorRole::OutlineVariant);

    applyCustomColor(&spec->containerColor, tokens, "containerColor");
    applyCustomColor(&spec->labelColor, tokens, "labelColor");
    applyCustomColor(&spec->iconColor, tokens, "iconColor");
    applyCustomColor(&spec->disabledContainerColor, tokens, "disabledContainerColor");
    applyCustomColor(&spec->disabledLabelColor, tokens, "disabledLabelColor");
    applyCustomColor(&spec->stateLayerColor, tokens, "stateLayerColor");
    applyCustomColor(&spec->focusRingColor, tokens, "focusRingColor");
    applyCustomColor(&spec->outlineColor, tokens, "outlineColor");
    applyCustomColor(&spec->disabledOutlineColor, tokens, "disabledOutlineColor");

    applyShapeMotionElevation(tokens, &spec->shapeRole, &spec->elevationRole, &spec->motionToken);
    applyTouchTarget(&spec->touchTarget, tokens);

    spec->iconSize = iconSizeFromTokens(tokens, IconSizeRole::Small, spec->iconSize);
    readInt(tokens.custom, "containerHeight", &spec->containerHeight);
    readInt(tokens.custom, "horizontalPadding", &spec->horizontalPadding);
    readInt(tokens.custom, "iconSize", &spec->iconSize);
    readInt(tokens.custom, "iconSpacing", &spec->iconSpacing);
}

void applyFabComponentTokens(const Theme& theme, const QStringList& componentNames, FabSpec* spec)
{
    if (!spec) return;
    const ComponentTokenOverride tokens = mergedComponentOverride(theme, componentNames);
    if (tokens.isEmpty()) return;

    applyColor(&spec->containerColor, tokens, ColorRole::PrimaryContainer);
    applyColor(&spec->iconColor, tokens, ColorRole::OnPrimaryContainer);
    applyColor(&spec->disabledContainerColor, tokens, ColorRole::SurfaceContainerHigh);
    applyColor(&spec->disabledIconColor, tokens, ColorRole::OnSurfaceVariant);
    applyColor(&spec->stateLayerColor, tokens, ColorRole::SurfaceTint);

    applyCustomColor(&spec->containerColor, tokens, "containerColor");
    applyCustomColor(&spec->iconColor, tokens, "iconColor");
    applyCustomColor(&spec->disabledContainerColor, tokens, "disabledContainerColor");
    applyCustomColor(&spec->disabledIconColor, tokens, "disabledIconColor");
    applyCustomColor(&spec->stateLayerColor, tokens, "stateLayerColor");

    applyShapeMotionElevation(tokens, &spec->shapeRole, &spec->elevationRole, &spec->motionToken);
    applyTouchTarget(&spec->touchTarget, tokens);
    spec->iconSize = iconSizeFromTokens(tokens, IconSizeRole::Medium, spec->iconSize);
    readInt(tokens.custom, "containerDiameter", &spec->containerDiameter);
    readInt(tokens.custom, "iconSize", &spec->iconSize);
}

void applyIconButtonComponentTokens(const Theme& theme, const QStringList& componentNames, IconButtonSpec* spec)
{
    if (!spec) return;
    const ComponentTokenOverride tokens = mergedComponentOverride(theme, componentNames);
    if (tokens.isEmpty()) return;

    applyColor(&spec->containerColor, tokens, ColorRole::SurfaceContainerLow);
    applyColor(&spec->iconColor, tokens, ColorRole::OnSurfaceVariant);
    applyColor(&spec->selectedContainerColor, tokens, ColorRole::PrimaryContainer);
    applyColor(&spec->selectedIconColor, tokens, ColorRole::OnPrimaryContainer);
    applyColor(&spec->disabledIconColor, tokens, ColorRole::OnSurfaceVariant);
    applyColor(&spec->stateLayerColor, tokens, ColorRole::SurfaceTint);

    applyCustomColor(&spec->containerColor, tokens, "containerColor");
    applyCustomColor(&spec->iconColor, tokens, "iconColor");
    applyCustomColor(&spec->selectedContainerColor, tokens, "selectedContainerColor");
    applyCustomColor(&spec->selectedIconColor, tokens, "selectedIconColor");
    applyCustomColor(&spec->disabledIconColor, tokens, "disabledIconColor");
    applyCustomColor(&spec->stateLayerColor, tokens, "stateLayerColor");

    applyShapeMotionElevation(tokens, &spec->shapeRole, &spec->elevationRole, &spec->motionToken);
    applyTouchTarget(&spec->touchTarget, tokens);
    spec->iconSize = iconSizeFromTokens(tokens, IconSizeRole::Medium, spec->iconSize);
    readInt(tokens.custom, "containerSize", &spec->containerSize);
    readInt(tokens.custom, "iconSize", &spec->iconSize);
}

void applyCheckboxComponentTokens(const Theme& theme, const QStringList& componentNames, CheckboxSpec* spec)
{
    if (!spec) return;
    const ComponentTokenOverride tokens = mergedComponentOverride(theme, componentNames);
    if (tokens.isEmpty()) return;

    applyColor(&spec->selectedContainerColor, tokens, ColorRole::Primary);
    applyColor(&spec->selectedIconColor, tokens, ColorRole::OnPrimary);
    applyColor(&spec->unselectedOutlineColor, tokens, ColorRole::OnSurfaceVariant);
    applyColor(&spec->disabledSelectedContainerColor, tokens, ColorRole::SurfaceContainerHigh);
    applyColor(&spec->disabledUnselectedOutlineColor, tokens, ColorRole::OutlineVariant);
    applyColor(&spec->labelColor, tokens, ColorRole::OnSurface);
    applyColor(&spec->disabledLabelColor, tokens, ColorRole::OnSurfaceVariant);
    applyColor(&spec->stateLayerColor, tokens, ColorRole::Primary);
    applyColor(&spec->focusRingColor, tokens, ColorRole::Primary);

    applyCustomColor(&spec->selectedContainerColor, tokens, "selectedContainerColor");
    applyCustomColor(&spec->selectedIconColor, tokens, "selectedIconColor");
    applyCustomColor(&spec->unselectedOutlineColor, tokens, "unselectedOutlineColor");
    applyCustomColor(&spec->disabledSelectedContainerColor, tokens, "disabledSelectedContainerColor");
    applyCustomColor(&spec->disabledUnselectedOutlineColor, tokens, "disabledUnselectedOutlineColor");
    applyCustomColor(&spec->labelColor, tokens, "labelColor");
    applyCustomColor(&spec->disabledLabelColor, tokens, "disabledLabelColor");
    applyCustomColor(&spec->stateLayerColor, tokens, "stateLayerColor");
    applyCustomColor(&spec->focusRingColor, tokens, "focusRingColor");

    applyTouchTarget(&spec->touchTarget, tokens);
    spec->indicatorSize = iconSizeFromTokens(tokens, IconSizeRole::Small, spec->indicatorSize);
    if (tokens.motion.contains(MotionToken::Short3)) spec->motionToken = MotionToken::Short3;
    readInt(tokens.custom, "indicatorSize", &spec->indicatorSize);
    readInt(tokens.custom, "spacing", &spec->spacing);
    readInt(tokens.custom, "outlineWidth", &spec->outlineWidth);
    if (tokens.custom.contains(QStringLiteral("motionToken"))) {
        spec->motionToken = parseMotionToken(tokens.custom.value(QStringLiteral("motionToken")).toString(), spec->motionToken);
    }
}

void applyRadioButtonComponentTokens(const Theme& theme, const QStringList& componentNames, RadioButtonSpec* spec)
{
    if (!spec) return;
    const ComponentTokenOverride tokens = mergedComponentOverride(theme, componentNames);
    if (tokens.isEmpty()) return;

    applyColor(&spec->selectedColor, tokens, ColorRole::Primary);
    applyColor(&spec->unselectedOutlineColor, tokens, ColorRole::OnSurfaceVariant);
    applyColor(&spec->disabledColor, tokens, ColorRole::OutlineVariant);
    applyColor(&spec->labelColor, tokens, ColorRole::OnSurface);
    applyColor(&spec->disabledLabelColor, tokens, ColorRole::OnSurfaceVariant);
    applyColor(&spec->stateLayerColor, tokens, ColorRole::Primary);
    applyColor(&spec->focusRingColor, tokens, ColorRole::Primary);

    applyCustomColor(&spec->selectedColor, tokens, "selectedColor");
    applyCustomColor(&spec->unselectedOutlineColor, tokens, "unselectedOutlineColor");
    applyCustomColor(&spec->disabledColor, tokens, "disabledColor");
    applyCustomColor(&spec->labelColor, tokens, "labelColor");
    applyCustomColor(&spec->disabledLabelColor, tokens, "disabledLabelColor");
    applyCustomColor(&spec->stateLayerColor, tokens, "stateLayerColor");
    applyCustomColor(&spec->focusRingColor, tokens, "focusRingColor");

    applyTouchTarget(&spec->touchTarget, tokens);
    spec->indicatorSize = iconSizeFromTokens(tokens, IconSizeRole::Small, spec->indicatorSize);
    if (tokens.motion.contains(MotionToken::Short3)) spec->motionToken = MotionToken::Short3;
    readInt(tokens.custom, "indicatorSize", &spec->indicatorSize);
    readInt(tokens.custom, "dotSize", &spec->dotSize);
    readInt(tokens.custom, "spacing", &spec->spacing);
    readInt(tokens.custom, "outlineWidth", &spec->outlineWidth);
    if (tokens.custom.contains(QStringLiteral("motionToken"))) {
        spec->motionToken = parseMotionToken(tokens.custom.value(QStringLiteral("motionToken")).toString(), spec->motionToken);
    }
}

void applySwitchComponentTokens(const Theme& theme, const QStringList& componentNames, SwitchSpec* spec)
{
    if (!spec) return;
    const ComponentTokenOverride tokens = mergedComponentOverride(theme, componentNames);
    if (tokens.isEmpty()) return;

    applyColor(&spec->selectedTrackColor, tokens, ColorRole::Primary);
    applyColor(&spec->selectedHandleColor, tokens, ColorRole::OnPrimary);
    applyColor(&spec->unselectedTrackColor, tokens, ColorRole::SurfaceVariant);
    applyColor(&spec->unselectedHandleColor, tokens, ColorRole::Outline);
    applyColor(&spec->disabledSelectedTrackColor, tokens, ColorRole::SurfaceContainerHigh);
    applyColor(&spec->disabledSelectedHandleColor, tokens, ColorRole::OnSurfaceVariant);
    applyColor(&spec->disabledUnselectedTrackColor, tokens, ColorRole::SurfaceContainerHigh);
    applyColor(&spec->disabledUnselectedHandleColor, tokens, ColorRole::OutlineVariant);
    applyColor(&spec->iconColor, tokens, ColorRole::OnPrimary);
    applyColor(&spec->stateLayerColor, tokens, ColorRole::Primary);
    applyColor(&spec->focusRingColor, tokens, ColorRole::Primary);
    applyColor(&spec->labelColor, tokens, ColorRole::OnSurface);
    applyColor(&spec->disabledLabelColor, tokens, ColorRole::OnSurfaceVariant);

    applyCustomColor(&spec->selectedTrackColor, tokens, "selectedTrackColor");
    applyCustomColor(&spec->selectedHandleColor, tokens, "selectedHandleColor");
    applyCustomColor(&spec->unselectedTrackColor, tokens, "unselectedTrackColor");
    applyCustomColor(&spec->unselectedHandleColor, tokens, "unselectedHandleColor");
    applyCustomColor(&spec->disabledSelectedTrackColor, tokens, "disabledSelectedTrackColor");
    applyCustomColor(&spec->disabledSelectedHandleColor, tokens, "disabledSelectedHandleColor");
    applyCustomColor(&spec->disabledUnselectedTrackColor, tokens, "disabledUnselectedTrackColor");
    applyCustomColor(&spec->disabledUnselectedHandleColor, tokens, "disabledUnselectedHandleColor");
    applyCustomColor(&spec->iconColor, tokens, "iconColor");
    applyCustomColor(&spec->stateLayerColor, tokens, "stateLayerColor");
    applyCustomColor(&spec->focusRingColor, tokens, "focusRingColor");
    applyCustomColor(&spec->labelColor, tokens, "labelColor");
    applyCustomColor(&spec->disabledLabelColor, tokens, "disabledLabelColor");

    applyTouchTarget(&spec->touchTarget, tokens);
    if (tokens.motion.contains(MotionToken::Short3)) spec->motionToken = MotionToken::Short3;
    readInt(tokens.custom, "trackWidth", &spec->trackWidth);
    readInt(tokens.custom, "trackHeight", &spec->trackHeight);
    readInt(tokens.custom, "handleDiameter", &spec->handleDiameter);
    readInt(tokens.custom, "selectedHandleDiameter", &spec->selectedHandleDiameter);
    readInt(tokens.custom, "spacing", &spec->spacing);
    readInt(tokens.custom, "stateLayerSize", &spec->stateLayerSize);
    if (tokens.custom.contains(QStringLiteral("motionToken"))) {
        spec->motionToken = parseMotionToken(tokens.custom.value(QStringLiteral("motionToken")).toString(), spec->motionToken);
    }
}

void applyTextFieldComponentTokens(const Theme& theme, const QStringList& componentNames, TextFieldSpec* spec)
{
    if (!spec) return;
    const ComponentTokenOverride tokens = mergedComponentOverride(theme, componentNames);
    if (tokens.isEmpty()) return;

    applyColor(&spec->containerColor, tokens, ColorRole::SurfaceContainerHighest);
    applyColor(&spec->activeIndicatorColor, tokens, ColorRole::OnSurfaceVariant);
    applyColor(&spec->outlineColor, tokens, ColorRole::Outline);
    applyColor(&spec->focusedOutlineColor, tokens, ColorRole::Primary);
    applyColor(&spec->disabledOutlineColor, tokens, ColorRole::OutlineVariant);
    applyColor(&spec->inputTextColor, tokens, ColorRole::OnSurface);
    applyColor(&spec->disabledInputTextColor, tokens, ColorRole::OnSurfaceVariant);
    applyColor(&spec->labelColor, tokens, ColorRole::OnSurfaceVariant);
    applyColor(&spec->disabledLabelColor, tokens, ColorRole::OnSurfaceVariant);
    applyColor(&spec->supportingTextColor, tokens, ColorRole::OnSurfaceVariant);
    applyColor(&spec->disabledSupportingTextColor, tokens, ColorRole::OnSurfaceVariant);
    applyColor(&spec->errorTextColor, tokens, ColorRole::Error);
    applyColor(&spec->cursorColor, tokens, ColorRole::Primary);
    applyColor(&spec->errorColor, tokens, ColorRole::Error);
    applyColor(&spec->stateLayerColor, tokens, ColorRole::OnSurfaceVariant);
    applyColor(&spec->disabledErrorTextColor, tokens, ColorRole::OnSurfaceVariant);
    applyColor(&spec->focusRingColor, tokens, ColorRole::Primary);

    applyCustomColor(&spec->containerColor, tokens, "containerColor");
    applyCustomColor(&spec->activeIndicatorColor, tokens, "activeIndicatorColor");
    applyCustomColor(&spec->outlineColor, tokens, "outlineColor");
    applyCustomColor(&spec->focusedOutlineColor, tokens, "focusedOutlineColor");
    applyCustomColor(&spec->disabledOutlineColor, tokens, "disabledOutlineColor");
    applyCustomColor(&spec->inputTextColor, tokens, "inputTextColor");
    applyCustomColor(&spec->disabledInputTextColor, tokens, "disabledInputTextColor");
    applyCustomColor(&spec->labelColor, tokens, "labelColor");
    applyCustomColor(&spec->disabledLabelColor, tokens, "disabledLabelColor");
    applyCustomColor(&spec->supportingTextColor, tokens, "supportingTextColor");
    applyCustomColor(&spec->disabledSupportingTextColor, tokens, "disabledSupportingTextColor");
    applyCustomColor(&spec->errorTextColor, tokens, "errorTextColor");
    applyCustomColor(&spec->cursorColor, tokens, "cursorColor");
    applyCustomColor(&spec->errorColor, tokens, "errorColor");
    applyCustomColor(&spec->stateLayerColor, tokens, "stateLayerColor");
    applyCustomColor(&spec->disabledErrorTextColor, tokens, "disabledErrorTextColor");
    applyCustomColor(&spec->focusRingColor, tokens, "focusRingColor");

    ElevationRole unusedElevation = ElevationRole::Level0;
    applyShapeMotionElevation(tokens, &spec->shapeRole, &unusedElevation, &spec->motionToken);
    readInt(tokens.custom, "minHeight", &spec->minHeight);
    readInt(tokens.custom, "horizontalPadding", &spec->horizontalPadding);
    readInt(tokens.custom, "verticalPadding", &spec->verticalPadding);
    readInt(tokens.custom, "supportingTopSpacing", &spec->supportingTopSpacing);
    readInt(tokens.custom, "topLabelHeight", &spec->topLabelHeight);
    readInt(tokens.custom, "supportingHeight", &spec->supportingHeight);
    readInt(tokens.custom, "outlineWidth", &spec->outlineWidth);
    readInt(tokens.custom, "focusedOutlineWidth", &spec->focusedOutlineWidth);
    readReal(tokens.custom, "focusRingWidth", &spec->focusRingWidth);
    readInt(tokens.custom, "supportingLineHeight", &spec->supportingLineHeight);
    readBool(tokens.custom, "reserveSupportingLine", &spec->reserveSupportingLine);
}

void applyCardComponentTokens(const Theme& theme, const QStringList& componentNames, CardSpec* spec)
{
    if (!spec) return;
    const ComponentTokenOverride tokens = mergedComponentOverride(theme, componentNames);
    if (tokens.isEmpty()) return;

    applyColor(&spec->containerColor, tokens, ColorRole::SurfaceContainerLow);
    applyColor(&spec->outlineColor, tokens, ColorRole::OutlineVariant);
    applyColor(&spec->contentColor, tokens, ColorRole::OnSurface);

    applyCustomColor(&spec->containerColor, tokens, "containerColor");
    applyCustomColor(&spec->outlineColor, tokens, "outlineColor");
    applyCustomColor(&spec->contentColor, tokens, "contentColor");

    MotionToken unusedMotion = MotionToken::Short4;
    applyShapeMotionElevation(tokens, &spec->shapeRole, &spec->elevationRole, &unusedMotion);
    readMargins(tokens.custom, &spec->contentPadding);
}

void applyDialogComponentTokens(const Theme& theme, const QStringList& componentNames, DialogSpec* spec)
{
    if (!spec) return;
    const ComponentTokenOverride tokens = mergedComponentOverride(theme, componentNames);
    if (tokens.isEmpty()) return;

    applyColor(&spec->containerColor, tokens, ColorRole::SurfaceContainerHigh);
    applyColor(&spec->headlineColor, tokens, ColorRole::OnSurface);
    applyColor(&spec->bodyColor, tokens, ColorRole::OnSurfaceVariant);
    applyColor(&spec->scrimColor, tokens, ColorRole::Scrim);

    applyCustomColor(&spec->containerColor, tokens, "containerColor");
    applyCustomColor(&spec->headlineColor, tokens, "headlineColor");
    applyCustomColor(&spec->bodyColor, tokens, "bodyColor");
    applyCustomColor(&spec->scrimColor, tokens, "scrimColor");

    applyShapeMotionElevation(tokens, &spec->shapeRole, &spec->elevationRole, &spec->enterMotion);
    if (tokens.custom.contains(QStringLiteral("enterMotion"))) {
        spec->enterMotion = parseMotionToken(tokens.custom.value(QStringLiteral("enterMotion")).toString(), spec->enterMotion);
    }
    if (tokens.custom.contains(QStringLiteral("exitMotion"))) {
        spec->exitMotion = parseMotionToken(tokens.custom.value(QStringLiteral("exitMotion")).toString(), spec->exitMotion);
    }
    readInt(tokens.custom, "maxWidth", &spec->maxWidth);
    readInt(tokens.custom, "padding", &spec->padding);
    readInt(tokens.custom, "headlineBottomSpacing", &spec->headlineBottomSpacing);
    readInt(tokens.custom, "actionsTopSpacing", &spec->actionsTopSpacing);
}

} // namespace QtMaterial
