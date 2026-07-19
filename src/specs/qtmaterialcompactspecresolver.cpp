#include "qtmaterial/specs/qtmaterialcompactspecresolver.h"

#include <QStringList>
#include <QVariant>
#include <QtGlobal>

#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialtheme.h"
#include "qtmaterialcomponenttokenapplier_p.h"

namespace QtMaterial {
namespace {

bool readInt(
    const QVariantMap& custom,
    const char* key,
    int* value)
{
    const QVariant candidate =
        custom.value(QString::fromLatin1(key));

    if (!candidate.isValid()) {
        return false;
    }

    bool ok = false;
    const int converted = candidate.toInt(&ok);
    if (!ok) {
        return false;
    }

    *value = converted;
    return true;
}

bool readReal(
    const QVariantMap& custom,
    const char* key,
    qreal* value)
{
    const QVariant candidate =
        custom.value(QString::fromLatin1(key));

    if (!candidate.isValid()) {
        return false;
    }

    bool ok = false;
    const double converted = candidate.toDouble(&ok);
    if (!ok) {
        return false;
    }

    *value = static_cast<qreal>(converted);
    return true;
}

bool readColor(
    const QVariantMap& custom,
    const char* key,
    QColor* value)
{
    const QVariant candidate =
        custom.value(QString::fromLatin1(key));

    if (!candidate.isValid()) {
        return false;
    }

    QColor converted = candidate.value<QColor>();
    if (!converted.isValid()) {
        converted = QColor(candidate.toString());
    }

    if (!converted.isValid()) {
        return false;
    }

    *value = converted;
    return true;
}

void applyColorRole(
    QColor* destination,
    const ComponentTokenOverride& tokens,
    ColorRole role)
{
    if (tokens.colors.contains(role)) {
        *destination = tokens.colors.value(role);
    }
}

void selectShapeRole(
    const ComponentTokenOverride& tokens,
    ShapeRole* role)
{
    const ShapeRole ordered[] = {
        ShapeRole::Full,
        ShapeRole::ExtraLarge,
        ShapeRole::Large,
        ShapeRole::Medium,
        ShapeRole::Small,
        ShapeRole::ExtraSmall,
        ShapeRole::None
    };

    for (ShapeRole candidate : ordered) {
        if (tokens.shapes.contains(candidate)) {
            *role = candidate;
            return;
        }
    }
}

QString variantComponentName(ChipVariant variant)
{
    switch (variant) {
    case ChipVariant::Filter:
        return QStringLiteral("chip.filter");
    case ChipVariant::Input:
        return QStringLiteral("chip.input");
    case ChipVariant::Suggestion:
        return QStringLiteral("chip.suggestion");
    case ChipVariant::Assist:
    default:
        return QStringLiteral("chip.assist");
    }
}

QString legacyVariantComponentName(ChipVariant variant)
{
    switch (variant) {
    case ChipVariant::Filter:
        return QStringLiteral("FilterChip");
    case ChipVariant::Input:
        return QStringLiteral("InputChip");
    case ChipVariant::Suggestion:
        return QStringLiteral("SuggestionChip");
    case ChipVariant::Assist:
    default:
        return QStringLiteral("AssistChip");
    }
}

} // namespace

CompactSpecResolver::CompactSpecResolver() = default;
CompactSpecResolver::~CompactSpecResolver() = default;

ChipSpec CompactSpecResolver::resolve(
    ChipVariant variant,
    const Theme& theme,
    Density density) const
{
    ChipSpec spec = baseChipSpec(theme, density);
    applyVariant(variant, theme, &spec);
    applyComponentOverrides(
        variant,
        theme,
        &spec);
    resolveRuntimeValues(theme, &spec);
    normalize(&spec);
    return spec;
}

ChipSpec CompactSpecResolver::assistChipSpec(
    const Theme& theme,
    Density density) const
{
    return resolve(
        ChipVariant::Assist,
        theme,
        density);
}

ChipSpec CompactSpecResolver::filterChipSpec(
    const Theme& theme,
    Density density) const
{
    return resolve(
        ChipVariant::Filter,
        theme,
        density);
}

ChipSpec CompactSpecResolver::inputChipSpec(
    const Theme& theme,
    Density density) const
{
    return resolve(
        ChipVariant::Input,
        theme,
        density);
}

ChipSpec CompactSpecResolver::suggestionChipSpec(
    const Theme& theme,
    Density density) const
{
    return resolve(
        ChipVariant::Suggestion,
        theme,
        density);
}

ChipSpec CompactSpecResolver::baseChipSpec(
    const Theme& theme,
    Density density) const
{
    ChipSpec spec;

    spec.containerColor =
        theme.colorScheme().color(
            ColorRole::SurfaceContainerLow);
    spec.selectedContainerColor =
        theme.colorScheme().color(
            ColorRole::SecondaryContainer);
    spec.labelColor =
        theme.colorScheme().color(
            ColorRole::OnSurface);
    spec.selectedLabelColor =
        theme.colorScheme().color(
            ColorRole::OnSecondaryContainer);
    spec.iconColor =
        theme.colorScheme().color(
            ColorRole::Primary);
    spec.selectedIconColor =
        spec.selectedLabelColor;
    spec.outlineColor =
        theme.colorScheme().color(
            ColorRole::Outline);
    spec.disabledContainerColor =
        theme.colorScheme().color(
            ColorRole::SurfaceContainerHigh);
    spec.disabledLabelColor =
        theme.colorScheme().color(
            ColorRole::OnSurfaceVariant);
    spec.disabledOutlineColor =
        theme.colorScheme().color(
            ColorRole::OutlineVariant);
    spec.stateLayerColor =
        theme.colorScheme().color(
            ColorRole::OnSurface);
    spec.focusRingColor =
        theme.colorScheme().color(
            ColorRole::Primary);

    switch (density) {
    case Density::Compact:
        spec.containerHeight = 28;
        spec.horizontalPadding = 12;
        spec.iconSpacing = 6;
        break;
    case Density::Comfortable:
        spec.containerHeight = 36;
        spec.horizontalPadding = 18;
        spec.iconSpacing = 8;
        break;
    case Density::Default:
    default:
        break;
    }

    spec.touchTarget.setHeight(
        qMax(
            spec.touchTarget.height(),
            spec.containerHeight));

    return spec;
}

void CompactSpecResolver::applyVariant(
    ChipVariant variant,
    const Theme& theme,
    ChipSpec* spec) const
{
    if (!spec) {
        return;
    }

    spec->variant = variant;

    switch (variant) {
    case ChipVariant::Filter:
        spec->containerColor = Qt::transparent;
        spec->selectedContainerColor =
            theme.colorScheme().color(
                ColorRole::SecondaryContainer);
        spec->labelColor =
            theme.colorScheme().color(
                ColorRole::OnSurfaceVariant);
        spec->selectedLabelColor =
            theme.colorScheme().color(
                ColorRole::OnSecondaryContainer);
        spec->iconColor = spec->labelColor;
        spec->selectedIconColor =
            spec->selectedLabelColor;
        spec->stateLayerColor =
            spec->labelColor;
        break;

    case ChipVariant::Input:
        spec->containerColor = Qt::transparent;
        spec->selectedContainerColor =
            theme.colorScheme().color(
                ColorRole::SurfaceContainerHighest);
        spec->iconColor =
            theme.colorScheme().color(
                ColorRole::OnSurfaceVariant);
        spec->selectedIconColor =
            spec->selectedLabelColor;
        break;

    case ChipVariant::Suggestion:
        spec->containerColor = Qt::transparent;
        spec->selectedContainerColor =
            theme.colorScheme().color(
                ColorRole::PrimaryContainer);
        spec->selectedLabelColor =
            theme.colorScheme().color(
                ColorRole::OnPrimaryContainer);
        spec->selectedIconColor =
            spec->selectedLabelColor;
        break;

    case ChipVariant::Assist:
    default:
        break;
    }
}

void CompactSpecResolver::applyComponentOverrides(
    ChipVariant variant,
    const Theme& theme,
    ChipSpec* spec) const
{
    if (!spec) {
        return;
    }

    const ComponentTokenOverride tokens =
        mergedComponentOverride(
            theme,
            QStringList{
                QStringLiteral("compact"),
                QStringLiteral("compact.chip"),
                QStringLiteral("chip"),
                variantComponentName(variant),
                legacyVariantComponentName(variant)
            });

    if (tokens.isEmpty()) {
        return;
    }

    applyColorRole(
        &spec->containerColor,
        tokens,
        ColorRole::SurfaceContainerLow);
    applyColorRole(
        &spec->selectedContainerColor,
        tokens,
        ColorRole::SecondaryContainer);
    applyColorRole(
        &spec->labelColor,
        tokens,
        ColorRole::OnSurface);
    applyColorRole(
        &spec->selectedLabelColor,
        tokens,
        ColorRole::OnSecondaryContainer);
    applyColorRole(
        &spec->iconColor,
        tokens,
        ColorRole::Primary);
    applyColorRole(
        &spec->selectedIconColor,
        tokens,
        ColorRole::OnPrimaryContainer);
    applyColorRole(
        &spec->outlineColor,
        tokens,
        ColorRole::Outline);
    applyColorRole(
        &spec->disabledContainerColor,
        tokens,
        ColorRole::SurfaceContainerHigh);
    applyColorRole(
        &spec->disabledLabelColor,
        tokens,
        ColorRole::OnSurfaceVariant);
    applyColorRole(
        &spec->disabledOutlineColor,
        tokens,
        ColorRole::OutlineVariant);
    applyColorRole(
        &spec->stateLayerColor,
        tokens,
        ColorRole::SurfaceTint);
    applyColorRole(
        &spec->focusRingColor,
        tokens,
        ColorRole::Primary);

    readColor(
        tokens.custom,
        "containerColor",
        &spec->containerColor);
    readColor(
        tokens.custom,
        "selectedContainerColor",
        &spec->selectedContainerColor);
    readColor(
        tokens.custom,
        "labelColor",
        &spec->labelColor);
    readColor(
        tokens.custom,
        "selectedLabelColor",
        &spec->selectedLabelColor);
    readColor(
        tokens.custom,
        "iconColor",
        &spec->iconColor);
    readColor(
        tokens.custom,
        "selectedIconColor",
        &spec->selectedIconColor);
    readColor(
        tokens.custom,
        "outlineColor",
        &spec->outlineColor);
    readColor(
        tokens.custom,
        "disabledContainerColor",
        &spec->disabledContainerColor);
    readColor(
        tokens.custom,
        "disabledLabelColor",
        &spec->disabledLabelColor);
    readColor(
        tokens.custom,
        "disabledOutlineColor",
        &spec->disabledOutlineColor);
    readColor(
        tokens.custom,
        "stateLayerColor",
        &spec->stateLayerColor);
    readColor(
        tokens.custom,
        "focusRingColor",
        &spec->focusRingColor);

    selectShapeRole(
        tokens,
        &spec->shapeRole);

    if (tokens.iconSizes.contains(
            IconSizeRole::Small)) {
        spec->iconSize =
            tokens.iconSizes.value(
                IconSizeRole::Small);
    }

    readInt(
        tokens.custom,
        "containerHeight",
        &spec->containerHeight);
    readInt(
        tokens.custom,
        "minWidth",
        &spec->minWidth);
    readInt(
        tokens.custom,
        "horizontalPadding",
        &spec->horizontalPadding);
    readInt(
        tokens.custom,
        "iconSize",
        &spec->iconSize);
    readInt(
        tokens.custom,
        "iconSpacing",
        &spec->iconSpacing);
    readInt(
        tokens.custom,
        "outlineWidth",
        &spec->outlineWidth);

    int touchWidth = spec->touchTarget.width();
    int touchHeight = spec->touchTarget.height();
    const bool touchWidthChanged =
        readInt(
            tokens.custom,
            "touchTargetWidth",
            &touchWidth);
    const bool touchHeightChanged =
        readInt(
            tokens.custom,
            "touchTargetHeight",
            &touchHeight);

    if (touchWidthChanged || touchHeightChanged) {
        spec->touchTarget =
            QSize(touchWidth, touchHeight);
    }

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
        "focusRingWidth",
        &spec->focusRingWidth);

    if (tokens.hasStateLayer) {
        spec->hoverStateLayerOpacity =
            tokens.stateLayer.hoverOpacity;
        spec->focusStateLayerOpacity =
            tokens.stateLayer.focusOpacity;
        spec->pressStateLayerOpacity =
            tokens.stateLayer.pressOpacity;
    }
}

void CompactSpecResolver::resolveRuntimeValues(
    const Theme& theme,
    ChipSpec* spec) const
{
    if (!spec) {
        return;
    }

    const ComponentTokenOverride tokens =
        mergedComponentOverride(
            theme,
            QStringList{
                QStringLiteral("compact"),
                QStringLiteral("compact.chip"),
                QStringLiteral("chip"),
                variantComponentName(spec->variant),
                legacyVariantComponentName(spec->variant)
            });

    spec->hasResolvedLabelFont = false;
    if (tokens.typography.contains(
            spec->labelTypeRole)) {
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

    if (spec->shapeRole == ShapeRole::Full) {
        spec->cornerRadius =
            spec->containerHeight / 2.0;
    } else if (
        tokens.shapes.contains(
            spec->shapeRole)) {
        spec->cornerRadius =
            tokens.shapes.value(
                spec->shapeRole);
    } else if (
        theme.shapes().contains(
            spec->shapeRole)) {
        spec->cornerRadius =
            theme.shapes().radius(
                spec->shapeRole);
    }

    if (!tokens.hasStateLayer) {
        const StateLayer& stateLayer =
            theme.stateLayer();
        spec->hoverStateLayerOpacity =
            stateLayer.hoverOpacity;
        spec->focusStateLayerOpacity =
            stateLayer.focusOpacity;
        spec->pressStateLayerOpacity =
            stateLayer.pressOpacity;
    }

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
        "focusRingWidth",
        &spec->focusRingWidth);
}

void CompactSpecResolver::normalize(
    ChipSpec* spec)
{
    if (!spec) {
        return;
    }

    spec->containerHeight =
        qMax(1, spec->containerHeight);
    spec->minWidth =
        qMax(1, spec->minWidth);
    spec->horizontalPadding =
        qMax(0, spec->horizontalPadding);
    spec->iconSize =
        qMax(0, spec->iconSize);
    spec->iconSpacing =
        qMax(0, spec->iconSpacing);
    spec->outlineWidth =
        qMax(0, spec->outlineWidth);

    spec->touchTarget.setWidth(
        qMax(
            spec->minWidth,
            spec->touchTarget.width()));
    spec->touchTarget.setHeight(
        qMax(
            spec->containerHeight,
            spec->touchTarget.height()));

    spec->cornerRadius =
        qBound<qreal>(
            0.0,
            spec->cornerRadius,
            spec->containerHeight / 2.0);

    spec->hoverStateLayerOpacity =
        qBound<qreal>(
            0.0,
            spec->hoverStateLayerOpacity,
            1.0);
    spec->focusStateLayerOpacity =
        qBound<qreal>(
            0.0,
            spec->focusStateLayerOpacity,
            1.0);
    spec->pressStateLayerOpacity =
        qBound<qreal>(
            0.0,
            spec->pressStateLayerOpacity,
            1.0);
    spec->focusRingWidth =
        qMax<qreal>(
            0.0,
            spec->focusRingWidth);
}

} // namespace QtMaterial
