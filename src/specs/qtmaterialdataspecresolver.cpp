#include "qtmaterial/specs/qtmaterialdataspecresolver.h"

#include <QCoreApplication>
#include <QGuiApplication>
#include <QVariant>

#include "qtmaterialcomponenttokenapplier_p.h"

namespace QtMaterial {
namespace {

QColor roleOr(
    const Theme& theme,
    ColorRole role,
    const QColor& fallback)
{
    const ColorScheme& scheme = theme.colorScheme();
    if (!scheme.contains(role)) {
        return fallback;
    }

    const QColor color = scheme.color(role);
    return color.isValid() ? color : fallback;
}

QColor withOpacity(
    QColor color,
    qreal opacity)
{
    color.setAlphaF(qBound(0.0, opacity, 1.0));
    return color;
}

QFont applicationFont()
{
    const auto* application =
        qobject_cast<QGuiApplication*>(
            QCoreApplication::instance());

    return application
        ? QGuiApplication::font()
        : QFont();
}

QFont fontFor(
    const Theme& theme,
    TypeRole role,
    const QFont& fallback)
{
    if (!theme.typography().contains(role)) {
        return fallback;
    }

    const QFont font =
        theme.typography().style(role).font;

    return font.family().isEmpty()
        ? fallback
        : font;
}

ShapeRole overriddenShapeRole(
    const ComponentTokenOverride& tokens,
    ShapeRole fallback)
{
    const ShapeRole roles[] = {
        ShapeRole::Full,
        ShapeRole::ExtraLarge,
        ShapeRole::Large,
        ShapeRole::Medium,
        ShapeRole::Small,
        ShapeRole::ExtraSmall,
        ShapeRole::None
    };

    for (ShapeRole role : roles) {
        if (tokens.shapes.contains(role)) {
            return role;
        }
    }

    const QString custom =
        tokens.custom
            .value(QStringLiteral("shapeRole"))
            .toString()
            .trimmed()
            .toLower();

    if (custom == QLatin1String("none")) {
        return ShapeRole::None;
    }
    if (
        custom == QLatin1String("extra-small")
        || custom == QLatin1String("extrasmall")) {
        return ShapeRole::ExtraSmall;
    }
    if (custom == QLatin1String("small")) {
        return ShapeRole::Small;
    }
    if (custom == QLatin1String("medium")) {
        return ShapeRole::Medium;
    }
    if (custom == QLatin1String("large")) {
        return ShapeRole::Large;
    }
    if (
        custom == QLatin1String("extra-large")
        || custom == QLatin1String("extralarge")) {
        return ShapeRole::ExtraLarge;
    }
    if (custom == QLatin1String("full")) {
        return ShapeRole::Full;
    }

    return fallback;
}

bool readInt(
    const QVariantMap& custom,
    const char* key,
    int* output)
{
    if (!output) {
        return false;
    }

    const QVariant value =
        custom.value(QString::fromLatin1(key));

    if (!value.isValid()) {
        return false;
    }

    bool ok = false;
    const int converted = value.toInt(&ok);

    if (!ok) {
        return false;
    }

    *output = converted;
    return true;
}

bool readReal(
    const QVariantMap& custom,
    const char* key,
    qreal* output)
{
    if (!output) {
        return false;
    }

    const QVariant value =
        custom.value(QString::fromLatin1(key));

    if (!value.isValid()) {
        return false;
    }

    bool ok = false;
    const double converted = value.toDouble(&ok);

    if (!ok) {
        return false;
    }

    *output = static_cast<qreal>(converted);
    return true;
}

bool readBool(
    const QVariantMap& custom,
    const char* key,
    bool* output)
{
    if (!output) {
        return false;
    }

    const QVariant value =
        custom.value(QString::fromLatin1(key));

    if (!value.isValid()) {
        return false;
    }

    *output = value.toBool();
    return true;
}

bool readColor(
    const QVariantMap& custom,
    const char* key,
    QColor* output)
{
    if (!output) {
        return false;
    }

    const QVariant value =
        custom.value(QString::fromLatin1(key));

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

    *output = color;
    return true;
}

void applyTokenColor(
    QColor* output,
    const ComponentTokenOverride& tokens,
    ColorRole role)
{
    if (
        output
        && tokens.colors.contains(role)) {
        *output = tokens.colors.value(role);
    }
}

qreal radiusFor(
    const Theme& theme,
    ShapeRole role,
    qreal fallback,
    int fullHeight)
{
    if (role == ShapeRole::Full) {
        return qMax<qreal>(
            0.0,
            static_cast<qreal>(fullHeight) / 2.0);
    }

    if (theme.shapes().contains(role)) {
        return qMax<qreal>(
            0.0,
            theme.shapes().radius(role));
    }

    return qMax<qreal>(0.0, fallback);
}

void applyListItemOverrides(
    const Theme& theme,
    ListItemSpec* spec)
{
    if (!spec) {
        return;
    }

    const ComponentTokenOverride tokens =
        mergedComponentOverride(
            theme,
            QStringList{
                QStringLiteral("data"),
                QStringLiteral("list"),
                QStringLiteral("listItem"),
                QStringLiteral("ListItem")
            });

    if (tokens.isEmpty()) {
        return;
    }

    applyTokenColor(
        &spec->containerColor,
        tokens,
        ColorRole::Surface);
    applyTokenColor(
        &spec->hoveredContainerColor,
        tokens,
        ColorRole::SurfaceContainerLow);
    applyTokenColor(
        &spec->selectedContainerColor,
        tokens,
        ColorRole::SecondaryContainer);
    applyTokenColor(
        &spec->pressedContainerColor,
        tokens,
        ColorRole::SurfaceContainerHigh);
    applyTokenColor(
        &spec->headlineColor,
        tokens,
        ColorRole::OnSurface);
    applyTokenColor(
        &spec->supportingTextColor,
        tokens,
        ColorRole::OnSurfaceVariant);
    applyTokenColor(
        &spec->leadingIconColor,
        tokens,
        ColorRole::OnSurfaceVariant);
    applyTokenColor(
        &spec->trailingIconColor,
        tokens,
        ColorRole::OnSurfaceVariant);
    applyTokenColor(
        &spec->dividerColor,
        tokens,
        ColorRole::OutlineVariant);
    applyTokenColor(
        &spec->stateLayerColor,
        tokens,
        ColorRole::SurfaceTint);
    applyTokenColor(
        &spec->focusRingColor,
        tokens,
        ColorRole::Primary);

    const QVariantMap& custom = tokens.custom;

    readColor(
        custom,
        "containerColor",
        &spec->containerColor);
    readColor(
        custom,
        "hoveredContainerColor",
        &spec->hoveredContainerColor);
    readColor(
        custom,
        "selectedContainerColor",
        &spec->selectedContainerColor);
    readColor(
        custom,
        "pressedContainerColor",
        &spec->pressedContainerColor);
    readColor(
        custom,
        "headlineColor",
        &spec->headlineColor);
    readColor(
        custom,
        "supportingTextColor",
        &spec->supportingTextColor);
    readColor(
        custom,
        "leadingIconColor",
        &spec->leadingIconColor);
    readColor(
        custom,
        "trailingIconColor",
        &spec->trailingIconColor);
    readColor(
        custom,
        "dividerColor",
        &spec->dividerColor);
    readColor(
        custom,
        "stateLayerColor",
        &spec->stateLayerColor);
    readColor(
        custom,
        "focusRingColor",
        &spec->focusRingColor);

    spec->shapeRole =
        overriddenShapeRole(
            tokens,
            spec->shapeRole);

    if (
        tokens.typography.contains(
            spec->headlineTypeRole)) {
        spec->headlineFont =
            tokens.typography
                .value(spec->headlineTypeRole)
                .font;
    }
    if (
        tokens.typography.contains(
            spec->supportingTypeRole)) {
        spec->supportingFont =
            tokens.typography
                .value(spec->supportingTypeRole)
                .font;
    }

    if (tokens.hasStateLayer) {
        spec->hoverOpacity =
            tokens.stateLayer.hoverOpacity;
        spec->focusOpacity =
            tokens.stateLayer.focusOpacity;
        spec->pressedOpacity =
            tokens.stateLayer.pressOpacity;
    }

    readInt(custom, "minHeight", &spec->minHeight);
    readInt(
        custom,
        "compactMinHeight",
        &spec->compactMinHeight);
    readInt(
        custom,
        "largeMinHeight",
        &spec->largeMinHeight);
    readInt(
        custom,
        "horizontalPadding",
        &spec->horizontalPadding);
    readInt(
        custom,
        "verticalPadding",
        &spec->verticalPadding);
    readInt(
        custom,
        "leadingSlotWidth",
        &spec->leadingSlotWidth);
    readInt(
        custom,
        "trailingSlotWidth",
        &spec->trailingSlotWidth);
    readInt(custom, "iconSize", &spec->iconSize);
    readInt(
        custom,
        "contentSpacing",
        &spec->contentSpacing);
    readInt(
        custom,
        "supportingTopSpacing",
        &spec->supportingTopSpacing);
    readInt(
        custom,
        "dividerInsetStart",
        &spec->dividerInsetStart);
    readInt(
        custom,
        "dividerInsetEnd",
        &spec->dividerInsetEnd);
    readInt(
        custom,
        "focusRingWidth",
        &spec->focusRingWidth);

    readReal(
        custom,
        "cornerRadius",
        &spec->cornerRadius);
    readReal(
        custom,
        "hoverOpacity",
        &spec->hoverOpacity);
    readReal(
        custom,
        "focusOpacity",
        &spec->focusOpacity);
    readReal(
        custom,
        "pressedOpacity",
        &spec->pressedOpacity);
    readReal(
        custom,
        "disabledOpacity",
        &spec->disabledOpacity);

    readBool(
        custom,
        "showDivider",
        &spec->showDivider);
    readBool(
        custom,
        "reserveLeadingSlot",
        &spec->reserveLeadingSlot);
    readBool(
        custom,
        "reserveTrailingSlot",
        &spec->reserveTrailingSlot);
}

void applyDividerOverrides(
    const Theme& theme,
    DividerSpec* spec)
{
    if (!spec) {
        return;
    }

    const ComponentTokenOverride tokens =
        mergedComponentOverride(
            theme,
            QStringList{
                QStringLiteral("data"),
                QStringLiteral("divider"),
                QStringLiteral("Divider")
            });

    if (tokens.isEmpty()) {
        return;
    }

    applyTokenColor(
        &spec->color,
        tokens,
        ColorRole::OutlineVariant);
    applyTokenColor(
        &spec->insetColor,
        tokens,
        ColorRole::OutlineVariant);

    const QVariantMap& custom = tokens.custom;

    readColor(custom, "color", &spec->color);
    readColor(
        custom,
        "insetColor",
        &spec->insetColor);

    spec->shapeRole =
        overriddenShapeRole(
            tokens,
            spec->shapeRole);

    readInt(
        custom,
        "thickness",
        &spec->thickness);
    readInt(
        custom,
        "leadingInset",
        &spec->leadingInset);
    readInt(
        custom,
        "trailingInset",
        &spec->trailingInset);
    readReal(
        custom,
        "cornerRadius",
        &spec->cornerRadius);

    int left = spec->margins.left();
    int top = spec->margins.top();
    int right = spec->margins.right();
    int bottom = spec->margins.bottom();

    bool marginsChanged = false;
    marginsChanged =
        readInt(custom, "marginLeft", &left)
        || marginsChanged;
    marginsChanged =
        readInt(custom, "marginTop", &top)
        || marginsChanged;
    marginsChanged =
        readInt(custom, "marginRight", &right)
        || marginsChanged;
    marginsChanged =
        readInt(custom, "marginBottom", &bottom)
        || marginsChanged;

    if (marginsChanged) {
        spec->margins =
            QMargins(left, top, right, bottom);
    }
}

} // namespace

ListItemSpec DataSpecResolver::listItemSpec(
    const Theme& theme,
    Density density) const
{
    ListItemSpec spec;

    spec.containerColor = Qt::transparent;
    spec.hoveredContainerColor =
        roleOr(
            theme,
            ColorRole::SurfaceContainerLow,
            QColor(QStringLiteral("#F7F2FA")));
    spec.selectedContainerColor =
        roleOr(
            theme,
            ColorRole::SecondaryContainer,
            QColor(QStringLiteral("#E8DEF8")));
    spec.pressedContainerColor =
        roleOr(
            theme,
            ColorRole::SurfaceContainerHigh,
            QColor(QStringLiteral("#ECE6F0")));

    spec.headlineColor =
        roleOr(
            theme,
            ColorRole::OnSurface,
            QColor(QStringLiteral("#1D1B20")));
    spec.supportingTextColor =
        roleOr(
            theme,
            ColorRole::OnSurfaceVariant,
            QColor(QStringLiteral("#49454F")));
    spec.leadingIconColor =
        spec.supportingTextColor;
    spec.trailingIconColor =
        spec.supportingTextColor;
    spec.dividerColor =
        roleOr(
            theme,
            ColorRole::OutlineVariant,
            QColor(QStringLiteral("#CAC4D0")));
    spec.stateLayerColor =
        roleOr(
            theme,
            ColorRole::OnSurface,
            spec.headlineColor);
    spec.focusRingColor =
        roleOr(
            theme,
            ColorRole::Primary,
            QColor(QStringLiteral("#6750A4")));

    const QFont fallback = applicationFont();
    spec.headlineFont =
        fontFor(
            theme,
            spec.headlineTypeRole,
            fallback);
    spec.supportingFont =
        fontFor(
            theme,
            spec.supportingTypeRole,
            fallback);

    const StateLayer& stateLayer =
        theme.stateLayer();

    spec.hoverOpacity =
        stateLayer.hoverOpacity;
    spec.focusOpacity =
        stateLayer.focusOpacity;
    spec.pressedOpacity =
        stateLayer.pressOpacity;

    switch (density) {
    case Density::Compact:
        spec.horizontalPadding = 12;
        spec.verticalPadding = 4;
        spec.contentSpacing = 8;
        break;
    case Density::Comfortable:
        spec.horizontalPadding = 20;
        spec.verticalPadding = 10;
        spec.contentSpacing = 16;
        break;
    case Density::Default:
    default:
        break;
    }

    applyListItemOverrides(theme, &spec);

    spec.disabledHeadlineColor =
        withOpacity(
            spec.headlineColor,
            spec.disabledOpacity);
    spec.disabledSupportingTextColor =
        withOpacity(
            spec.supportingTextColor,
            spec.disabledOpacity);
    spec.disabledIconColor =
        spec.disabledSupportingTextColor;

    spec.minHeight = qMax(1, spec.minHeight);
    spec.compactMinHeight =
        qMax(1, spec.compactMinHeight);
    spec.largeMinHeight =
        qMax(1, spec.largeMinHeight);
    spec.horizontalPadding =
        qMax(0, spec.horizontalPadding);
    spec.verticalPadding =
        qMax(0, spec.verticalPadding);
    spec.leadingSlotWidth =
        qMax(0, spec.leadingSlotWidth);
    spec.trailingSlotWidth =
        qMax(0, spec.trailingSlotWidth);
    spec.iconSize =
        qMax(1, spec.iconSize);
    spec.contentSpacing =
        qMax(0, spec.contentSpacing);
    spec.supportingTopSpacing =
        qMax(0, spec.supportingTopSpacing);
    spec.dividerInsetStart =
        qMax(0, spec.dividerInsetStart);
    spec.dividerInsetEnd =
        qMax(0, spec.dividerInsetEnd);
    spec.focusRingWidth =
        qMax(0, spec.focusRingWidth);

    spec.hoverOpacity =
        qBound<qreal>(
            0.0,
            spec.hoverOpacity,
            1.0);
    spec.focusOpacity =
        qBound<qreal>(
            0.0,
            spec.focusOpacity,
            1.0);
    spec.pressedOpacity =
        qBound<qreal>(
            0.0,
            spec.pressedOpacity,
            1.0);
    spec.disabledOpacity =
        qBound<qreal>(
            0.0,
            spec.disabledOpacity,
            1.0);

    if (spec.cornerRadius < 0.0) {
        spec.cornerRadius =
            radiusFor(
                theme,
                spec.shapeRole,
                0.0,
                spec.minHeight);
    }

    if (spec.headlineFont.family().isEmpty()) {
        spec.headlineFont =
            fontFor(
                theme,
                spec.headlineTypeRole,
                fallback);
    }

    if (spec.supportingFont.family().isEmpty()) {
        spec.supportingFont =
            fontFor(
                theme,
                spec.supportingTypeRole,
                fallback);
    }

    return spec;
}

DividerSpec DataSpecResolver::dividerSpec(
    const Theme& theme) const
{
    DividerSpec spec;

    spec.color =
        roleOr(
            theme,
            ColorRole::OutlineVariant,
            QColor(QStringLiteral("#CAC4D0")));
    spec.insetColor = spec.color;
    spec.shapeRole = ShapeRole::None;
    spec.orientation = Qt::Horizontal;
    spec.margins = QMargins();
    spec.thickness = 1;
    spec.leadingInset = 0;
    spec.trailingInset = 0;

    applyDividerOverrides(theme, &spec);

    spec.thickness =
        qMax(1, spec.thickness);
    spec.leadingInset =
        qMax(0, spec.leadingInset);
    spec.trailingInset =
        qMax(0, spec.trailingInset);

    if (spec.cornerRadius < 0.0) {
        spec.cornerRadius =
            radiusFor(
                theme,
                spec.shapeRole,
                0.0,
                spec.thickness);
    }

    if (!spec.insetColor.isValid()) {
        spec.insetColor = spec.color;
    }

    return spec;
}

} // namespace QtMaterial
