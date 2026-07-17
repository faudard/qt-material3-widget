#include "qtmaterial/widgets/data/qtmateriallistitem.h"

#include <QEvent>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>

#include "qtmaterial/specs/qtmaterialdataspecresolver.h"

namespace QtMaterial {

class QtMaterialListItemPrivate
{
public:
    QString headlineText;
    QString supportingText;
    QIcon leadingIcon;
    QIcon trailingIcon;

    bool selected = false;
    bool dividerVisible = false;

    QtMaterialListItem::DensityVariant densityVariant =
        QtMaterialListItem::DensityVariant::Standard;

    mutable ListItemSpec resolvedSpec;
    mutable bool specDirty = true;
};

namespace {

QColor withOpacity(
    QColor color,
    qreal opacity)
{
    color.setAlphaF(
        qBound<qreal>(
            0.0,
            opacity,
            1.0));
    return color;
}

void fillRoundedRect(
    QPainter* painter,
    const QRectF& rect,
    qreal radius,
    const QColor& color)
{
    if (
        !painter
        || !color.isValid()
        || color.alpha() == 0
        || rect.isEmpty()) {
        return;
    }

    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(color);

    if (radius > 0.0) {
        painter->drawRoundedRect(
            rect,
            radius,
            radius);
    } else {
        painter->drawRect(rect);
    }

    painter->restore();
}

void paintTintedIcon(
    QPainter* painter,
    const QIcon& icon,
    const QRect& rect,
    const QColor& color,
    bool enabled)
{
    if (
        !painter
        || icon.isNull()
        || rect.isEmpty()) {
        return;
    }

    QPixmap pixmap =
        icon.pixmap(
            rect.size(),
            enabled
                ? QIcon::Normal
                : QIcon::Disabled,
            QIcon::Off);

    if (pixmap.isNull()) {
        return;
    }

    if (color.isValid()) {
        QPainter tintPainter(&pixmap);
        tintPainter.setCompositionMode(
            QPainter::CompositionMode_SourceIn);
        tintPainter.fillRect(
            pixmap.rect(),
            color);
    }

    painter->drawPixmap(
        rect,
        pixmap);
}

} // namespace

QtMaterialListItem::QtMaterialListItem(
    QWidget* parent)
    : QtMaterialControl(parent)
    , d_ptr(
          std::make_unique<
              QtMaterialListItemPrivate>())
{
    setMaterialComponent(
        QStringLiteral("ListItem"));
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    ensureSpecResolved();
    setAccessibleName(
        QStringLiteral("List item"));
}

QtMaterialListItem::QtMaterialListItem(
    const QString& headline,
    QWidget* parent)
    : QtMaterialListItem(parent)
{
    setHeadlineText(headline);
}

QtMaterialListItem::~QtMaterialListItem() =
    default;

QString QtMaterialListItem::headlineText() const
{
    return d_ptr->headlineText;
}

void QtMaterialListItem::setHeadlineText(
    const QString& text)
{
    if (d_ptr->headlineText == text) {
        return;
    }

    d_ptr->headlineText = text;
    contentChangedEvent();
}

QString QtMaterialListItem::supportingText() const
{
    return d_ptr->supportingText;
}

void QtMaterialListItem::setSupportingText(
    const QString& text)
{
    if (d_ptr->supportingText == text) {
        return;
    }

    d_ptr->supportingText = text;
    contentChangedEvent();
}

QIcon QtMaterialListItem::leadingIcon() const
{
    return d_ptr->leadingIcon;
}

void QtMaterialListItem::setLeadingIcon(
    const QIcon& icon)
{
    if (
        d_ptr->leadingIcon.cacheKey()
        == icon.cacheKey()) {
        return;
    }

    d_ptr->leadingIcon = icon;
    updateGeometry();
    update();
}

QIcon QtMaterialListItem::trailingIcon() const
{
    return d_ptr->trailingIcon;
}

void QtMaterialListItem::setTrailingIcon(
    const QIcon& icon)
{
    if (
        d_ptr->trailingIcon.cacheKey()
        == icon.cacheKey()) {
        return;
    }

    d_ptr->trailingIcon = icon;
    updateGeometry();
    update();
}

bool QtMaterialListItem::isSelected() const noexcept
{
    return d_ptr->selected;
}

void QtMaterialListItem::setSelected(
    bool selected)
{
    if (d_ptr->selected == selected) {
        return;
    }

    d_ptr->selected = selected;
    Q_EMIT selectionChanged(selected);
    update();
}

bool QtMaterialListItem::isDividerVisible() const noexcept
{
    return d_ptr->dividerVisible;
}

void QtMaterialListItem::setDividerVisible(
    bool visible)
{
    if (d_ptr->dividerVisible == visible) {
        return;
    }

    d_ptr->dividerVisible = visible;
    update();
}

QtMaterialListItem::DensityVariant
QtMaterialListItem::densityVariant() const noexcept
{
    return d_ptr->densityVariant;
}

void QtMaterialListItem::setDensityVariant(
    DensityVariant variant)
{
    if (d_ptr->densityVariant == variant) {
        return;
    }

    d_ptr->densityVariant = variant;
    updateGeometry();
    update();
}

const ListItemSpec&
QtMaterialListItem::resolvedSpec() const
{
    ensureSpecResolved();
    return d_ptr->resolvedSpec;
}

QSize QtMaterialListItem::sizeHint() const
{
    ensureSpecResolved();

    return QSize(
        qMax(
            240,
            d_ptr->resolvedSpec.touchTarget.width()),
        resolvedHeight());
}

QSize QtMaterialListItem::minimumSizeHint() const
{
    ensureSpecResolved();

    return QSize(
        qMax(
            120,
            d_ptr->resolvedSpec.touchTarget.width()),
        resolvedHeight());
}

void QtMaterialListItem::paintEvent(
    QPaintEvent*)
{
    ensureSpecResolved();

    const ListItemSpec& spec =
        d_ptr->resolvedSpec;
    const QtMaterialInteractionState& state =
        interactionState();

    QPainter painter(this);
    painter.setRenderHint(
        QPainter::Antialiasing,
        spec.cornerRadius > 0.0);
    painter.setClipRect(rect());

    const QRectF containerRect(rect());

    QColor containerColor =
        d_ptr->selected
        ? spec.selectedContainerColor
        : spec.containerColor;

    if (state.isEnabled()) {
        if (
            state.isPressed()
            && spec.pressedContainerColor.isValid()) {
            containerColor =
                spec.pressedContainerColor;
        } else if (
            state.isHovered()
            && spec.hoveredContainerColor.isValid()) {
            containerColor =
                spec.hoveredContainerColor;
        }
    }

    fillRoundedRect(
        &painter,
        containerRect,
        spec.cornerRadius,
        containerColor);

    qreal stateOpacity = 0.0;
    if (state.isEnabled()) {
        if (state.isPressed()) {
            stateOpacity = spec.pressedOpacity;
        } else if (state.isHovered()) {
            stateOpacity = spec.hoverOpacity;
        } else if (state.isFocused()) {
            stateOpacity = spec.focusOpacity;
        }
    }

    if (
        stateOpacity > 0.0
        && spec.stateLayerColor.isValid()) {
        fillRoundedRect(
            &painter,
            containerRect,
            spec.cornerRadius,
            withOpacity(
                spec.stateLayerColor,
                stateOpacity));
    }

    if (
        state.isFocused()
        && spec.focusRingWidth > 0
        && spec.focusRingColor.isValid()) {
        painter.save();
        QPen focusPen(
            spec.focusRingColor,
            spec.focusRingWidth);
        focusPen.setJoinStyle(Qt::RoundJoin);
        painter.setPen(focusPen);
        painter.setBrush(Qt::NoBrush);

        const qreal halfWidth =
            static_cast<qreal>(
                spec.focusRingWidth) / 2.0;
        const QRectF focusRect =
            containerRect.adjusted(
                halfWidth,
                halfWidth,
                -halfWidth,
                -halfWidth);

        if (spec.cornerRadius > 0.0) {
            painter.drawRoundedRect(
                focusRect,
                spec.cornerRadius,
                spec.cornerRadius);
        } else {
            painter.drawRect(focusRect);
        }
        painter.restore();
    }

    const bool enabled = isEnabled();

    const QColor headlineColor =
        enabled
        ? spec.headlineColor
        : spec.disabledHeadlineColor;
    const QColor supportingColor =
        enabled
        ? spec.supportingTextColor
        : spec.disabledSupportingTextColor;
    const QColor leadingIconColor =
        enabled
        ? spec.leadingIconColor
        : spec.disabledIconColor;
    const QColor trailingIconColor =
        enabled
        ? spec.trailingIconColor
        : spec.disabledIconColor;

    const int horizontalPadding =
        spec.horizontalPadding;
    const int iconSize =
        spec.iconSize;

    int textLeft = horizontalPadding;

    const bool leadingSlot =
        !d_ptr->leadingIcon.isNull()
        || spec.reserveLeadingSlot;

    if (leadingSlot) {
        const int slotWidth =
            qMax(
                iconSize,
                spec.leadingSlotWidth);

        const QRect iconRect(
            horizontalPadding
                + (slotWidth - iconSize) / 2,
            (height() - iconSize) / 2,
            iconSize,
            iconSize);

        paintTintedIcon(
            &painter,
            d_ptr->leadingIcon,
            iconRect,
            leadingIconColor,
            enabled);

        textLeft +=
            slotWidth
            + spec.contentSpacing;
    }

    int textRight =
        width()
        - horizontalPadding;

    const bool trailingSlot =
        !d_ptr->trailingIcon.isNull()
        || spec.reserveTrailingSlot;

    if (trailingSlot) {
        const int slotWidth =
            qMax(
                iconSize,
                spec.trailingSlotWidth);

        const QRect iconRect(
            width()
                - horizontalPadding
                - slotWidth
                + (slotWidth - iconSize) / 2,
            (height() - iconSize) / 2,
            iconSize,
            iconSize);

        paintTintedIcon(
            &painter,
            d_ptr->trailingIcon,
            iconRect,
            trailingIconColor,
            enabled);

        textRight -=
            slotWidth
            + spec.contentSpacing;
    }

    const int availableWidth =
        qMax(
            0,
            textRight - textLeft);

    const QString headline =
        d_ptr->headlineText.isEmpty()
        ? QStringLiteral("List item")
        : d_ptr->headlineText;

    const bool hasSupporting =
        !d_ptr->supportingText.isEmpty();

    painter.setPen(headlineColor);
    painter.setFont(spec.headlineFont);

    if (hasSupporting) {
        const QFontMetrics headlineMetrics(
            spec.headlineFont);
        const QFontMetrics supportingMetrics(
            spec.supportingFont);

        const int headlineHeight =
            headlineMetrics.height();
        const int supportingHeight =
            supportingMetrics.height();
        const int contentHeight =
            headlineHeight
            + spec.supportingTopSpacing
            + supportingHeight;
        const int top =
            qMax(
                spec.verticalPadding,
                (height() - contentHeight) / 2);

        painter.drawText(
            QRect(
                textLeft,
                top,
                availableWidth,
                headlineHeight),
            Qt::AlignLeft
                | Qt::AlignVCenter,
            headline);

        painter.setPen(supportingColor);
        painter.setFont(spec.supportingFont);
        painter.drawText(
            QRect(
                textLeft,
                top
                    + headlineHeight
                    + spec.supportingTopSpacing,
                availableWidth,
                supportingHeight),
            Qt::AlignLeft
                | Qt::AlignVCenter,
            d_ptr->supportingText);
    } else {
        painter.drawText(
            QRect(
                textLeft,
                0,
                availableWidth,
                height()),
            Qt::AlignLeft
                | Qt::AlignVCenter,
            headline);
    }

    if (
        d_ptr->dividerVisible
        || spec.showDivider) {
        const int startInset =
            qMax(
                0,
                spec.dividerInsetStart);
        const int endInset =
            qMax(
                0,
                spec.dividerInsetEnd);
        const int dividerWidth =
            qMax(
                0,
                width()
                    - startInset
                    - endInset);

        painter.fillRect(
            QRect(
                startInset,
                qMax(0, height() - 1),
                dividerWidth,
                1),
            spec.dividerColor);
    }
}

void QtMaterialListItem::mousePressEvent(
    QMouseEvent* event)
{
    if (
        event
        && event->button() == Qt::LeftButton
        && rect().contains(event->pos())) {
        interactionState().setPressed(true);
        stateChangedEvent();
    }

    QtMaterialControl::mousePressEvent(event);
}

void QtMaterialListItem::mouseReleaseEvent(
    QMouseEvent* event)
{
    const bool activate =
        event
        && event->button() == Qt::LeftButton
        && rect().contains(event->pos());

    if (interactionState().isPressed()) {
        interactionState().setPressed(false);
        stateChangedEvent();
    }

    if (activate) {
        Q_EMIT clicked();
        Q_EMIT activated();
    }

    QtMaterialControl::mouseReleaseEvent(event);
}

void QtMaterialListItem::keyPressEvent(
    QKeyEvent* event)
{
    if (!event) {
        return;
    }

    switch (event->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space:
        Q_EMIT clicked();
        Q_EMIT activated();
        event->accept();
        return;
    default:
        break;
    }

    QtMaterialControl::keyPressEvent(event);
}

void QtMaterialListItem::changeEvent(
    QEvent* event)
{
    QtMaterialControl::changeEvent(event);

    if (
        event
        && (
            event->type() == QEvent::FontChange
            || event->type() == QEvent::LayoutDirectionChange)) {
        invalidateResolvedSpec();
        updateGeometry();
        update();
    }
}

void QtMaterialListItem::themeChangedEvent(
    const QtMaterial::Theme& theme)
{
    QtMaterialControl::themeChangedEvent(theme);
}

void QtMaterialListItem::invalidateResolvedSpec()
{
    d_ptr->specDirty = true;
}

void QtMaterialListItem::stateChangedEvent()
{
    QtMaterialControl::stateChangedEvent();
    update();
}

void QtMaterialListItem::ensureSpecResolved() const
{
    if (!d_ptr->specDirty) {
        return;
    }

    const DataSpecResolver resolver;
    d_ptr->resolvedSpec =
        resolver.listItemSpec(
            theme(),
            density());
    d_ptr->specDirty = false;
}

int QtMaterialListItem::resolvedHeight() const
{
    ensureSpecResolved();

    int heightValue = 0;

    switch (d_ptr->densityVariant) {
    case DensityVariant::Compact:
        heightValue =
            d_ptr->resolvedSpec.compactMinHeight;
        break;
    case DensityVariant::Large:
        heightValue =
            d_ptr->resolvedSpec.largeMinHeight;
        break;
    case DensityVariant::Standard:
    default:
        heightValue =
            d_ptr->resolvedSpec.minHeight;
        break;
    }

    if (!d_ptr->supportingText.isEmpty()) {
        heightValue =
            qMax(
                heightValue,
                d_ptr->resolvedSpec.largeMinHeight);
    }

    return heightValue;
}

void QtMaterialListItem::contentChangedEvent()
{
    const QString accessibleHeadline =
        d_ptr->headlineText.trimmed();

    setAccessibleName(
        accessibleHeadline.isEmpty()
            ? QStringLiteral("List item")
            : accessibleHeadline);
    setAccessibleDescription(
        d_ptr->supportingText.trimmed());

    updateGeometry();
    update();
}

} // namespace QtMaterial
