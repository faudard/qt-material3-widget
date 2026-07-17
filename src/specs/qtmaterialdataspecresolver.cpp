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


void applyListOverrides(
    const Theme& theme,
    ListSpec* spec)
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
                QStringLiteral("List")
            });

    if (tokens.isEmpty()) {
        return;
    }

    applyTokenColor(
        &spec->containerColor,
        tokens,
        ColorRole::Surface);
    applyTokenColor(
        &spec->focusRingColor,
        tokens,
        ColorRole::Primary);
    applyTokenColor(
        &spec->dividerColor,
        tokens,
        ColorRole::OutlineVariant);
    applyTokenColor(
        &spec->selectionOverlayColor,
        tokens,
        ColorRole::SecondaryContainer);

    const QVariantMap& custom = tokens.custom;
    readColor(
        custom,
        "containerColor",
        &spec->containerColor);
    readColor(
        custom,
        "focusRingColor",
        &spec->focusRingColor);
    readColor(
        custom,
        "dividerColor",
        &spec->dividerColor);
    readColor(
        custom,
        "selectionOverlayColor",
        &spec->selectionOverlayColor);

    spec->shapeRole =
        overriddenShapeRole(
            tokens,
            spec->shapeRole);

    readInt(
        custom,
        "itemSpacing",
        &spec->itemSpacing);
    readInt(
        custom,
        "dividerThickness",
        &spec->dividerThickness);
    readInt(
        custom,
        "focusRingWidth",
        &spec->focusRingWidth);
    readBool(
        custom,
        "showDividers",
        &spec->showDividers);

    int left = spec->contentMargins.left();
    int top = spec->contentMargins.top();
    int right = spec->contentMargins.right();
    int bottom = spec->contentMargins.bottom();
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
        spec->contentMargins =
            QMargins(left, top, right, bottom);
    }

    int minimumWidth =
        spec->minimumViewportSize.width();
    int minimumHeight =
        spec->minimumViewportSize.height();
    bool minimumChanged = false;

    minimumChanged =
        readInt(
            custom,
            "minimumViewportWidth",
            &minimumWidth)
        || minimumChanged;
    minimumChanged =
        readInt(
            custom,
            "minimumViewportHeight",
            &minimumHeight)
        || minimumChanged;

    if (minimumChanged) {
        spec->minimumViewportSize =
            QSize(minimumWidth, minimumHeight);
    }

    readReal(
        custom,
        "cornerRadius",
        &spec->cornerRadius);
}

void applyGridListOverrides(
    const Theme& theme,
    GridListSpec* spec)
{
    if (!spec) {
        return;
    }

    const ComponentTokenOverride tokens =
        mergedComponentOverride(
            theme,
            QStringList{
                QStringLiteral("data"),
                QStringLiteral("gridList"),
                QStringLiteral("GridList")
            });

    if (tokens.isEmpty()) {
        return;
    }

    applyTokenColor(
        &spec->backgroundColor,
        tokens,
        ColorRole::Surface);
    applyTokenColor(
        &spec->foregroundColor,
        tokens,
        ColorRole::OnSurface);
    applyTokenColor(
        &spec->itemBackgroundColor,
        tokens,
        ColorRole::SurfaceContainerLow);
    applyTokenColor(
        &spec->itemSelectedColor,
        tokens,
        ColorRole::SecondaryContainer);
    applyTokenColor(
        &spec->itemSelectedTextColor,
        tokens,
        ColorRole::OnSecondaryContainer);
    applyTokenColor(
        &spec->supportingTextColor,
        tokens,
        ColorRole::OnSurfaceVariant);
    applyTokenColor(
        &spec->focusRingColor,
        tokens,
        ColorRole::Primary);

    const QVariantMap& custom = tokens.custom;
    readColor(
        custom,
        "backgroundColor",
        &spec->backgroundColor);
    readColor(
        custom,
        "foregroundColor",
        &spec->foregroundColor);
    readColor(
        custom,
        "itemBackgroundColor",
        &spec->itemBackgroundColor);
    readColor(
        custom,
        "itemHoverColor",
        &spec->itemHoverColor);
    readColor(
        custom,
        "itemSelectedColor",
        &spec->itemSelectedColor);
    readColor(
        custom,
        "itemSelectedTextColor",
        &spec->itemSelectedTextColor);
    readColor(
        custom,
        "supportingTextColor",
        &spec->supportingTextColor);
    readColor(
        custom,
        "disabledTextColor",
        &spec->disabledTextColor);
    readColor(
        custom,
        "focusRingColor",
        &spec->focusRingColor);

    if (
        tokens.typography.contains(
            TypeRole::TitleMedium)) {
        spec->titleFont =
            tokens.typography
                .value(TypeRole::TitleMedium)
                .font;
    }
    if (
        tokens.typography.contains(
            TypeRole::BodyMedium)) {
        spec->supportingFont =
            tokens.typography
                .value(TypeRole::BodyMedium)
                .font;
    }

    int itemWidth = spec->itemSize.width();
    int itemHeight = spec->itemSize.height();
    const bool itemWidthChanged =
        readInt(custom, "itemWidth", &itemWidth);
    const bool itemHeightChanged =
        readInt(custom, "itemHeight", &itemHeight);

    if (itemWidthChanged || itemHeightChanged) {
        spec->itemSize =
            QSize(itemWidth, itemHeight);
    }
    readInt(custom, "itemRadius", &spec->itemRadius);
    readInt(custom, "spacing", &spec->spacing);
    readInt(custom, "focusRingWidth", &spec->focusRingWidth);
    readInt(custom, "columns", &spec->columns);
    readInt(custom, "minimumCellWidth", &spec->minimumCellWidth);
}

void applyCarouselOverrides(
    const Theme& theme,
    CarouselSpec* spec)
{
    if (!spec) {
        return;
    }

    const ComponentTokenOverride tokens =
        mergedComponentOverride(
            theme,
            QStringList{
                QStringLiteral("data"),
                QStringLiteral("carousel"),
                QStringLiteral("Carousel")
            });

    if (tokens.isEmpty()) {
        return;
    }

    applyTokenColor(
        &spec->backgroundColor,
        tokens,
        ColorRole::Surface);
    applyTokenColor(
        &spec->foregroundColor,
        tokens,
        ColorRole::OnSurface);
    applyTokenColor(
        &spec->itemBackgroundColor,
        tokens,
        ColorRole::SurfaceContainerLow);
    applyTokenColor(
        &spec->itemSelectedColor,
        tokens,
        ColorRole::SecondaryContainer);
    applyTokenColor(
        &spec->itemSelectedTextColor,
        tokens,
        ColorRole::OnSecondaryContainer);
    applyTokenColor(
        &spec->supportingTextColor,
        tokens,
        ColorRole::OnSurfaceVariant);
    applyTokenColor(
        &spec->outlineColor,
        tokens,
        ColorRole::OutlineVariant);
    applyTokenColor(
        &spec->selectedOutlineColor,
        tokens,
        ColorRole::Primary);
    applyTokenColor(
        &spec->focusRingColor,
        tokens,
        ColorRole::Primary);
    applyTokenColor(
        &spec->navigationButtonColor,
        tokens,
        ColorRole::Primary);
    applyTokenColor(
        &spec->navigationButtonTextColor,
        tokens,
        ColorRole::OnPrimary);

    const QVariantMap& custom = tokens.custom;

    readColor(custom, "backgroundColor", &spec->backgroundColor);
    readColor(custom, "foregroundColor", &spec->foregroundColor);
    readColor(custom, "pageIndicatorColor", &spec->pageIndicatorColor);
    readColor(custom, "activePageIndicatorColor", &spec->activePageIndicatorColor);
    readColor(custom, "navigationButtonColor", &spec->navigationButtonColor);
    readColor(custom, "navigationButtonTextColor", &spec->navigationButtonTextColor);
    readColor(custom, "focusRingColor", &spec->focusRingColor);
    readColor(custom, "itemBackgroundColor", &spec->itemBackgroundColor);
    readColor(custom, "itemHoverColor", &spec->itemHoverColor);
    readColor(custom, "itemPressedColor", &spec->itemPressedColor);
    readColor(custom, "itemSelectedColor", &spec->itemSelectedColor);
    readColor(custom, "itemSelectedTextColor", &spec->itemSelectedTextColor);
    readColor(custom, "supportingTextColor", &spec->supportingTextColor);
    readColor(custom, "disabledTextColor", &spec->disabledTextColor);
    readColor(custom, "outlineColor", &spec->outlineColor);
    readColor(custom, "selectedOutlineColor", &spec->selectedOutlineColor);
    readColor(custom, "iconColor", &spec->iconColor);
    readColor(custom, "selectedIconColor", &spec->selectedIconColor);

    if (tokens.typography.contains(TypeRole::TitleMedium)) {
        spec->labelFont =
            tokens.typography
                .value(TypeRole::TitleMedium)
                .font;
    }
    if (tokens.typography.contains(TypeRole::BodyMedium)) {
        spec->supportingFont =
            tokens.typography
                .value(TypeRole::BodyMedium)
                .font;
    }

    spec->shapeRole =
        overriddenShapeRole(
            tokens,
            spec->shapeRole);

    int itemWidth = spec->itemSize.width();
    int itemHeight = spec->itemSize.height();
    bool itemSizeChanged = false;
    itemSizeChanged =
        readInt(custom, "itemWidth", &itemWidth)
        || itemSizeChanged;
    itemSizeChanged =
        readInt(custom, "itemHeight", &itemHeight)
        || itemSizeChanged;
    if (itemSizeChanged) {
        spec->itemSize = QSize(itemWidth, itemHeight);
    }

    int minimumWidth = spec->minimumItemSize.width();
    int minimumHeight = spec->minimumItemSize.height();
    bool minimumChanged = false;
    minimumChanged =
        readInt(custom, "minimumItemWidth", &minimumWidth)
        || minimumChanged;
    minimumChanged =
        readInt(custom, "minimumItemHeight", &minimumHeight)
        || minimumChanged;
    if (minimumChanged) {
        spec->minimumItemSize =
            QSize(minimumWidth, minimumHeight);
    }

    int outerLeft = spec->outerMargins.left();
    int outerTop = spec->outerMargins.top();
    int outerRight = spec->outerMargins.right();
    int outerBottom = spec->outerMargins.bottom();
    bool outerChanged = false;
    outerChanged = readInt(custom, "outerMarginLeft", &outerLeft) || outerChanged;
    outerChanged = readInt(custom, "outerMarginTop", &outerTop) || outerChanged;
    outerChanged = readInt(custom, "outerMarginRight", &outerRight) || outerChanged;
    outerChanged = readInt(custom, "outerMarginBottom", &outerBottom) || outerChanged;
    if (outerChanged) {
        spec->outerMargins =
            QMargins(
                outerLeft,
                outerTop,
                outerRight,
                outerBottom);
    }

    int contentLeft = spec->contentMargins.left();
    int contentTop = spec->contentMargins.top();
    int contentRight = spec->contentMargins.right();
    int contentBottom = spec->contentMargins.bottom();
    bool contentChanged = false;
    contentChanged = readInt(custom, "contentMarginLeft", &contentLeft) || contentChanged;
    contentChanged = readInt(custom, "contentMarginTop", &contentTop) || contentChanged;
    contentChanged = readInt(custom, "contentMarginRight", &contentRight) || contentChanged;
    contentChanged = readInt(custom, "contentMarginBottom", &contentBottom) || contentChanged;
    if (contentChanged) {
        spec->contentMargins =
            QMargins(
                contentLeft,
                contentTop,
                contentRight,
                contentBottom);
    }

    readReal(custom, "cornerRadius", &spec->cornerRadius);
    readInt(custom, "pageSpacing", &spec->pageSpacing);
    readInt(custom, "indicatorSize", &spec->indicatorSize);
    readInt(custom, "focusRingWidth", &spec->focusRingWidth);
    readInt(custom, "outlineWidth", &spec->outlineWidth);
    readInt(custom, "selectedOutlineWidth", &spec->selectedOutlineWidth);
    readInt(custom, "iconSize", &spec->iconSize);
    readInt(custom, "iconSpacing", &spec->iconSpacing);
    readInt(custom, "supportingTopSpacing", &spec->supportingTopSpacing);
}

} // namespace


ListSpec DataSpecResolver::listSpec(
    const Theme& theme,
    Density density) const
{
    ListSpec spec;

    spec.containerColor =
        roleOr(
            theme,
            ColorRole::Surface,
            QColor(QStringLiteral("#FFFBFE")));
    spec.focusRingColor =
        roleOr(
            theme,
            ColorRole::Primary,
            QColor(QStringLiteral("#6750A4")));
    spec.dividerColor =
        roleOr(
            theme,
            ColorRole::OutlineVariant,
            QColor(QStringLiteral("#CAC4D0")));
    spec.selectionOverlayColor =
        roleOr(
            theme,
            ColorRole::SecondaryContainer,
            QColor(QStringLiteral("#E8DEF8")));

    switch (density) {
    case Density::Compact:
        spec.contentMargins =
            QMargins(0, 4, 0, 4);
        spec.minimumViewportSize =
            QSize(220, 120);
        break;
    case Density::Comfortable:
        spec.contentMargins =
            QMargins(0, 12, 0, 12);
        spec.minimumViewportSize =
            QSize(260, 200);
        spec.itemSpacing = 2;
        break;
    case Density::Default:
    default:
        break;
    }

    applyListOverrides(theme, &spec);

    spec.itemSpacing =
        qMax(0, spec.itemSpacing);
    spec.dividerThickness =
        qMax(1, spec.dividerThickness);
    spec.focusRingWidth =
        qMax(0, spec.focusRingWidth);
    spec.minimumViewportSize =
        QSize(
            qMax(1, spec.minimumViewportSize.width()),
            qMax(1, spec.minimumViewportSize.height()));

    if (spec.cornerRadius < 0.0) {
        spec.cornerRadius =
            radiusFor(
                theme,
                spec.shapeRole,
                16.0,
                spec.minimumViewportSize.height());
    }

    return spec;
}

GridListSpec DataSpecResolver::gridListSpec(
    const Theme& theme,
    Density density) const
{
    GridListSpec spec =
        defaultGridListSpec();

    spec.backgroundColor =
        roleOr(
            theme,
            ColorRole::Surface,
            spec.backgroundColor);
    spec.foregroundColor =
        roleOr(
            theme,
            ColorRole::OnSurface,
            spec.foregroundColor);
    spec.itemBackgroundColor =
        roleOr(
            theme,
            ColorRole::SurfaceContainerLow,
            spec.itemBackgroundColor);
    spec.itemSelectedColor =
        roleOr(
            theme,
            ColorRole::SecondaryContainer,
            spec.itemSelectedColor);
    spec.itemSelectedTextColor =
        roleOr(
            theme,
            ColorRole::OnSecondaryContainer,
            spec.itemSelectedTextColor);
    spec.supportingTextColor =
        roleOr(
            theme,
            ColorRole::OnSurfaceVariant,
            spec.supportingTextColor);
    spec.focusRingColor =
        roleOr(
            theme,
            ColorRole::Primary,
            spec.focusRingColor);

    const StateLayer& stateLayer =
        theme.stateLayer();

    spec.itemHoverColor =
        withOpacity(
            spec.foregroundColor,
            stateLayer.hoverOpacity);
    spec.disabledTextColor =
        withOpacity(
            spec.foregroundColor,
            0.38);

    const QFont fallback =
        applicationFont();
    spec.titleFont =
        fontFor(
            theme,
            TypeRole::TitleMedium,
            fallback);
    spec.supportingFont =
        fontFor(
            theme,
            TypeRole::BodyMedium,
            fallback);

    switch (density) {
    case Density::Compact:
        spec.itemSize = QSize(144, 120);
        spec.spacing = 8;
        spec.minimumCellWidth = 104;
        break;
    case Density::Comfortable:
        spec.itemSize = QSize(176, 152);
        spec.spacing = 16;
        spec.minimumCellWidth = 136;
        break;
    case Density::Default:
    default:
        break;
    }

    applyGridListOverrides(
        theme,
        &spec);

    spec.itemSize =
        QSize(
            qMax(48, spec.itemSize.width()),
            qMax(48, spec.itemSize.height()));
    spec.itemRadius =
        qMax(0, spec.itemRadius);
    spec.spacing =
        qMax(0, spec.spacing);
    spec.focusRingWidth =
        qMax(0, spec.focusRingWidth);
    spec.columns =
        qMax(1, spec.columns);
    spec.minimumCellWidth =
        qMax(48, spec.minimumCellWidth);

    if (spec.titleFont.family().isEmpty()) {
        spec.titleFont =
            fontFor(
                theme,
                TypeRole::TitleMedium,
                fallback);
    }
    if (spec.supportingFont.family().isEmpty()) {
        spec.supportingFont =
            fontFor(
                theme,
                TypeRole::BodyMedium,
                fallback);
    }

    return spec;
}

CarouselSpec DataSpecResolver::carouselSpec(
    const Theme& theme,
    Density density) const
{
    CarouselSpec spec =
        defaultCarouselSpec();

    spec.backgroundColor =
        roleOr(
            theme,
            ColorRole::Surface,
            spec.backgroundColor);
    spec.foregroundColor =
        roleOr(
            theme,
            ColorRole::OnSurface,
            spec.foregroundColor);
    spec.pageIndicatorColor =
        roleOr(
            theme,
            ColorRole::OutlineVariant,
            spec.pageIndicatorColor);
    spec.activePageIndicatorColor =
        roleOr(
            theme,
            ColorRole::Primary,
            spec.activePageIndicatorColor);
    spec.navigationButtonColor =
        roleOr(
            theme,
            ColorRole::Primary,
            spec.navigationButtonColor);
    spec.navigationButtonTextColor =
        roleOr(
            theme,
            ColorRole::OnPrimary,
            spec.navigationButtonTextColor);
    spec.focusRingColor =
        roleOr(
            theme,
            ColorRole::Primary,
            spec.focusRingColor);

    spec.itemBackgroundColor =
        roleOr(
            theme,
            ColorRole::SurfaceContainerLow,
            spec.itemBackgroundColor);
    spec.itemSelectedColor =
        roleOr(
            theme,
            ColorRole::SecondaryContainer,
            spec.itemSelectedColor);
    spec.itemSelectedTextColor =
        roleOr(
            theme,
            ColorRole::OnSecondaryContainer,
            spec.itemSelectedTextColor);
    spec.supportingTextColor =
        roleOr(
            theme,
            ColorRole::OnSurfaceVariant,
            spec.supportingTextColor);
    spec.outlineColor =
        roleOr(
            theme,
            ColorRole::OutlineVariant,
            spec.outlineColor);
    spec.selectedOutlineColor =
        roleOr(
            theme,
            ColorRole::Primary,
            spec.selectedOutlineColor);
    spec.iconColor =
        spec.supportingTextColor;
    spec.selectedIconColor =
        spec.itemSelectedTextColor;

    const StateLayer& stateLayer =
        theme.stateLayer();

    spec.itemHoverColor =
        withOpacity(
            spec.foregroundColor,
            stateLayer.hoverOpacity);
    spec.itemPressedColor =
        withOpacity(
            spec.foregroundColor,
            stateLayer.pressOpacity);
    spec.disabledTextColor =
        withOpacity(
            spec.foregroundColor,
            0.38);

    const QFont fallback =
        applicationFont();
    spec.labelFont =
        fontFor(
            theme,
            TypeRole::TitleMedium,
            fallback);
    spec.supportingFont =
        fontFor(
            theme,
            TypeRole::BodyMedium,
            fallback);

    spec.shapeRole = ShapeRole::Large;
    spec.cornerRadius = -1.0;

    switch (density) {
    case Density::Compact:
        spec.itemSize = QSize(160, 112);
        spec.outerMargins = QMargins(12, 12, 12, 12);
        spec.contentMargins = QMargins(12, 10, 12, 10);
        spec.pageSpacing = 8;
        spec.iconSize = 24;
        spec.iconSpacing = 8;
        spec.supportingTopSpacing = 4;
        break;
    case Density::Comfortable:
        spec.itemSize = QSize(192, 144);
        spec.outerMargins = QMargins(20, 20, 20, 20);
        spec.contentMargins = QMargins(16, 14, 16, 14);
        spec.pageSpacing = 16;
        spec.iconSize = 32;
        spec.iconSpacing = 12;
        spec.supportingTopSpacing = 8;
        break;
    case Density::Default:
    default:
        break;
    }

    applyCarouselOverrides(
        theme,
        &spec);

    spec.itemSize =
        QSize(
            qMax(48, spec.itemSize.width()),
            qMax(48, spec.itemSize.height()));
    spec.minimumItemSize =
        QSize(
            qMax(48, spec.minimumItemSize.width()),
            qMax(48, spec.minimumItemSize.height()));
    spec.outerMargins =
        QMargins(
            qMax(0, spec.outerMargins.left()),
            qMax(0, spec.outerMargins.top()),
            qMax(0, spec.outerMargins.right()),
            qMax(0, spec.outerMargins.bottom()));
    spec.contentMargins =
        QMargins(
            qMax(0, spec.contentMargins.left()),
            qMax(0, spec.contentMargins.top()),
            qMax(0, spec.contentMargins.right()),
            qMax(0, spec.contentMargins.bottom()));
    spec.pageSpacing = qMax(0, spec.pageSpacing);
    spec.indicatorSize = qMax(1, spec.indicatorSize);
    spec.focusRingWidth = qMax(0, spec.focusRingWidth);
    spec.outlineWidth = qMax(0, spec.outlineWidth);
    spec.selectedOutlineWidth = qMax(0, spec.selectedOutlineWidth);
    spec.iconSize = qMax(1, spec.iconSize);
    spec.iconSpacing = qMax(0, spec.iconSpacing);
    spec.supportingTopSpacing = qMax(0, spec.supportingTopSpacing);

    if (spec.cornerRadius < 0.0) {
        spec.cornerRadius =
            radiusFor(
                theme,
                spec.shapeRole,
                18.0,
                spec.itemSize.height());
    }

    if (spec.labelFont.family().isEmpty()) {
        spec.labelFont =
            fontFor(
                theme,
                TypeRole::TitleMedium,
                fallback);
    }
    if (spec.supportingFont.family().isEmpty()) {
        spec.supportingFont =
            fontFor(
                theme,
                TypeRole::BodyMedium,
                fallback);
    }

    return spec;
}

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
