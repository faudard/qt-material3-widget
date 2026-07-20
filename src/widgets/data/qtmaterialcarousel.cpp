#include "qtmaterial/widgets/data/qtmaterialcarousel.h"

#include <QEvent>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QStringList>

#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/specs/qtmaterialdataspecresolver.h"

namespace QtMaterial {

struct QtMaterialCarouselPrivate
{
    QVector<QtMaterialCarousel::Item> items;

    mutable CarouselSpec spec;
    mutable bool specDirty = true;
    CarouselSpec explicitSpec;
    bool explicitSpecSet = false;

    int currentIndex = -1;
    int hoveredIndex = -1;
    int pressedIndex = -1;
    int visibleItemCount = 3;

    bool wrapAround = true;

    QString accessibilitySummary;
};

namespace {

QString fallbackCarouselName()
{
    return QStringLiteral("Carousel");
}

bool isValidIndex(
    const QtMaterialCarouselPrivate* data,
    int index) noexcept
{
    return
        data
        && index >= 0
        && index < data->items.size();
}

int firstVisibleIndex(
    const QtMaterialCarouselPrivate* data) noexcept
{
    if (!data || data->items.isEmpty()) {
        return 0;
    }

    const int half =
        data->visibleItemCount / 2;

    return qBound(
        0,
        data->currentIndex - half,
        qMax(
            0,
            data->items.size()
                - data->visibleItemCount));
}

int nextEnabledIndex(
    const QtMaterialCarouselPrivate* data,
    int start,
    int delta) noexcept
{
    if (
        !data
        || data->items.isEmpty()
        || delta == 0) {
        return -1;
    }

    int index =
        isValidIndex(data, start)
        ? start
        : (
            delta > 0
            ? -1
            : data->items.size());

    for (
        int step = 0;
        step < data->items.size();
        ++step) {
        index += delta;

        if (data->wrapAround) {
            if (index >= data->items.size()) {
                index = 0;
            } else if (index < 0) {
                index = data->items.size() - 1;
            }
        }

        if (!isValidIndex(data, index)) {
            return -1;
        }

        if (data->items.at(index).enabled) {
            return index;
        }
    }

    return -1;
}

QRect itemRect(
    const QtMaterialCarousel* widget,
    const QtMaterialCarouselPrivate* data,
    const CarouselSpec& spec,
    int index)
{
    if (!widget || !isValidIndex(data, index)) {
        return QRect();
    }

    const int first = firstVisibleIndex(data);
    const int last = qMin(
        data->items.size(),
        first + data->visibleItemCount);

    if (index < first || index >= last) {
        return QRect();
    }

    const int visibleOffset = index - first;
    const int cards = last - first;
    const int indicatorBand =
        spec.indicatorSize > 0
        ? spec.indicatorSize + spec.pageSpacing
        : 0;

    const int availableWidth =
        widget->width()
        - spec.outerMargins.left()
        - spec.outerMargins.right()
        - qMax(0, cards - 1) * spec.pageSpacing;

    const int cardWidth = qMax(
        spec.minimumItemSize.width(),
        availableWidth / qMax(1, cards));
    const int cardHeight = qMax(
        spec.minimumItemSize.height(),
        widget->height()
            - spec.outerMargins.top()
            - spec.outerMargins.bottom()
            - indicatorBand);

    const int visualOffset =
        widget->layoutDirection() == Qt::RightToLeft
        ? cards - 1 - visibleOffset
        : visibleOffset;
    const int x =
        spec.outerMargins.left()
        + visualOffset * (cardWidth + spec.pageSpacing);

    return QRect(
        x,
        spec.outerMargins.top(),
        cardWidth,
        cardHeight);
}

QPixmap tintedIconPixmap(
    const QIcon& icon,
    int extent,
    const QColor& color,
    bool enabled)
{
    const QSize size(
        qMax(1, extent),
        qMax(1, extent));

    QPixmap pixmap =
        icon.pixmap(
            size,
            enabled
                ? QIcon::Normal
                : QIcon::Disabled,
            QIcon::Off);

    if (
        pixmap.isNull()
        || !color.isValid()) {
        return pixmap;
    }

    QPainter painter(&pixmap);
    painter.setCompositionMode(
        QPainter::CompositionMode_SourceIn);
    painter.fillRect(
        pixmap.rect(),
        color);

    return pixmap;
}

void updateAccessibilitySummary(
    QtMaterialCarousel* widget,
    QtMaterialCarouselPrivate* data)
{
    if (!widget || !data) {
        return;
    }

    QString summary;

    if (data->items.isEmpty()) {
        summary =
            widget->tr("Carousel, empty");
    } else {
        summary =
            widget->tr(
                "Carousel, %1 items, current %2")
            .arg(data->items.size())
            .arg(
                widget
                    ->currentItemAccessibleText());
    }

    if (
        data->accessibilitySummary
        == summary) {
        widget->setAccessibleDescription(
            summary);
        return;
    }

    data->accessibilitySummary =
        summary;

    widget->setAccessibleDescription(
        data->accessibilitySummary);

    Q_EMIT
        widget->accessibilitySummaryChanged(
            data->accessibilitySummary);
}

void emitCurrentIndexChanged(
    QtMaterialCarousel* widget,
    int index)
{
    if (widget) {
        Q_EMIT
            widget->currentIndexChanged(index);
    }
}

} // namespace

QtMaterialCarousel::QtMaterialCarousel(
    QWidget* parent)
    : QtMaterialControl(parent)
    , d_ptr(
        std::make_unique<
            QtMaterialCarouselPrivate>())
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setAttribute(Qt::WA_Hover, true);

    setMaterialComponent(
        QStringLiteral("carousel"));
    setMaterialRole(
        QStringLiteral("data"));

    setAccessibleName(
        fallbackCarouselName());

    updateAccessibilitySummary(
        this,
        d_ptr.get());
}

QtMaterialCarousel::~QtMaterialCarousel() = default;

int QtMaterialCarousel::addItem(
    const QString& title,
    const QString& supportingText,
    const QIcon& icon)
{
    insertItem(
        d_ptr->items.size(),
        title,
        supportingText,
        icon);

    return d_ptr->items.size() - 1;
}

void QtMaterialCarousel::insertItem(
    int index,
    const QString& title,
    const QString& supportingText,
    const QIcon& icon)
{
    index =
        qBound(
            0,
            index,
            d_ptr->items.size());

    d_ptr->items.insert(
        index,
        Item{
            title,
            supportingText,
            icon,
            true
        });

    if (d_ptr->currentIndex < 0) {
        d_ptr->currentIndex = index;
        emitCurrentIndexChanged(
            this,
            d_ptr->currentIndex);
    } else if (index <= d_ptr->currentIndex) {
        ++d_ptr->currentIndex;
        emitCurrentIndexChanged(
            this,
            d_ptr->currentIndex);
    }

    Q_EMIT itemCountChanged(
        d_ptr->items.size());

    updateAccessibilitySummary(
        this,
        d_ptr.get());

    updateGeometry();
    update();
}

void QtMaterialCarousel::removeItem(int index)
{
    if (!isValidIndex(d_ptr.get(), index)) {
        return;
    }

    d_ptr->items.removeAt(index);

    if (d_ptr->items.isEmpty()) {
        if (d_ptr->currentIndex != -1) {
            d_ptr->currentIndex = -1;
            emitCurrentIndexChanged(
                this,
                d_ptr->currentIndex);
        }
    } else if (index < d_ptr->currentIndex) {
        --d_ptr->currentIndex;
        emitCurrentIndexChanged(
            this,
            d_ptr->currentIndex);
    } else if (index == d_ptr->currentIndex) {
        d_ptr->currentIndex =
            qMin(
                index,
                d_ptr->items.size() - 1);

        if (
            !d_ptr->items
                .at(d_ptr->currentIndex)
                .enabled) {
            const int enabled =
                nextEnabledIndex(
                    d_ptr.get(),
                    d_ptr->currentIndex,
                    1);

            if (enabled >= 0) {
                d_ptr->currentIndex = enabled;
            }
        }

        emitCurrentIndexChanged(
            this,
            d_ptr->currentIndex);
    }

    d_ptr->hoveredIndex = -1;
    d_ptr->pressedIndex = -1;

    Q_EMIT itemCountChanged(
        d_ptr->items.size());

    updateAccessibilitySummary(
        this,
        d_ptr.get());

    updateGeometry();
    update();
}

void QtMaterialCarousel::clear()
{
    if (d_ptr->items.isEmpty()) {
        return;
    }

    d_ptr->items.clear();
    d_ptr->currentIndex = -1;
    d_ptr->hoveredIndex = -1;
    d_ptr->pressedIndex = -1;

    Q_EMIT itemCountChanged(0);
    emitCurrentIndexChanged(this, -1);

    updateAccessibilitySummary(
        this,
        d_ptr.get());

    updateGeometry();
    update();
}

int QtMaterialCarousel::count() const noexcept
{
    return d_ptr->items.size();
}

bool QtMaterialCarousel::isEmpty() const noexcept
{
    return d_ptr->items.isEmpty();
}

QString QtMaterialCarousel::itemTitle(
    int index) const
{
    return isValidIndex(d_ptr.get(), index)
        ? d_ptr->items.at(index).title
        : QString();
}

void QtMaterialCarousel::setItemTitle(
    int index,
    const QString& title)
{
    if (
        !isValidIndex(d_ptr.get(), index)
        || d_ptr->items[index].title == title) {
        return;
    }

    d_ptr->items[index].title = title;

    updateAccessibilitySummary(
        this,
        d_ptr.get());
    update();
}

QString QtMaterialCarousel::itemSupportingText(
    int index) const
{
    return isValidIndex(d_ptr.get(), index)
        ? d_ptr->items.at(index).supportingText
        : QString();
}

void QtMaterialCarousel::setItemSupportingText(
    int index,
    const QString& text)
{
    if (
        !isValidIndex(d_ptr.get(), index)
        || d_ptr->items[index].supportingText
            == text) {
        return;
    }

    d_ptr->items[index].supportingText = text;

    updateAccessibilitySummary(
        this,
        d_ptr.get());
    update();
}

QIcon QtMaterialCarousel::itemIcon(
    int index) const
{
    return isValidIndex(d_ptr.get(), index)
        ? d_ptr->items.at(index).icon
        : QIcon();
}

void QtMaterialCarousel::setItemIcon(
    int index,
    const QIcon& icon)
{
    if (!isValidIndex(d_ptr.get(), index)) {
        return;
    }

    d_ptr->items[index].icon = icon;
    update();
}

bool QtMaterialCarousel::isItemEnabled(
    int index) const
{
    return
        isValidIndex(d_ptr.get(), index)
        && d_ptr->items.at(index).enabled;
}

void QtMaterialCarousel::setItemEnabled(
    int index,
    bool enabled)
{
    if (
        !isValidIndex(d_ptr.get(), index)
        || d_ptr->items[index].enabled
            == enabled) {
        return;
    }

    d_ptr->items[index].enabled = enabled;

    if (
        !enabled
        && index == d_ptr->currentIndex) {
        const int replacement =
            nextEnabledIndex(
                d_ptr.get(),
                index,
                1);

        if (replacement >= 0) {
            d_ptr->currentIndex = replacement;
            emitCurrentIndexChanged(
                this,
                d_ptr->currentIndex);
        }
    }

    if (!enabled) {
        if (d_ptr->hoveredIndex == index) {
            d_ptr->hoveredIndex = -1;
        }
        if (d_ptr->pressedIndex == index) {
            d_ptr->pressedIndex = -1;
        }
    }

    updateAccessibilitySummary(
        this,
        d_ptr.get());
    update();
}

int QtMaterialCarousel::currentIndex() const noexcept
{
    return d_ptr->currentIndex;
}

void QtMaterialCarousel::setCurrentIndex(
    int index)
{
    if (
        !isValidIndex(d_ptr.get(), index)
        || !d_ptr->items.at(index).enabled
        || index == d_ptr->currentIndex) {
        return;
    }

    d_ptr->currentIndex = index;

    emitCurrentIndexChanged(
        this,
        d_ptr->currentIndex);

    updateAccessibilitySummary(
        this,
        d_ptr.get());
    update();
}

bool QtMaterialCarousel::wrapAround() const noexcept
{
    return d_ptr->wrapAround;
}

void QtMaterialCarousel::setWrapAround(bool wrap)
{
    if (d_ptr->wrapAround == wrap) {
        return;
    }

    d_ptr->wrapAround = wrap;
    Q_EMIT wrapAroundChanged(
        d_ptr->wrapAround);
}

int QtMaterialCarousel::visibleItemCount() const noexcept
{
    return d_ptr->visibleItemCount;
}

void QtMaterialCarousel::setVisibleItemCount(
    int count)
{
    count = qBound(1, count, 9);

    if (d_ptr->visibleItemCount == count) {
        return;
    }

    d_ptr->visibleItemCount = count;

    Q_EMIT visibleItemCountChanged(
        d_ptr->visibleItemCount);

    updateGeometry();
    update();
}

QString QtMaterialCarousel::itemAccessibleText(
    int index) const
{
    if (!isValidIndex(d_ptr.get(), index)) {
        return QString();
    }

    const Item& item =
        d_ptr->items.at(index);

    QStringList parts;
    parts << item.title;

    if (!item.supportingText.isEmpty()) {
        parts << item.supportingText;
    }

    parts << tr("%1 of %2")
        .arg(index + 1)
        .arg(d_ptr->items.size());

    if (!item.enabled) {
        parts << tr("disabled");
    }

    if (index == d_ptr->currentIndex) {
        parts << tr("current");
    }

    return parts.join(
        QStringLiteral(", "));
}

QString QtMaterialCarousel::
currentItemAccessibleText() const
{
    return itemAccessibleText(
        d_ptr->currentIndex);
}

QString QtMaterialCarousel::
accessibilitySummary() const
{
    return d_ptr->accessibilitySummary;
}

const CarouselSpec&
QtMaterialCarousel::resolvedSpec() const
{
    ensureSpecResolved();
    return d_ptr->spec;
}

CarouselSpec QtMaterialCarousel::spec() const
{
    return resolvedSpec();
}

void QtMaterialCarousel::setSpec(
    const CarouselSpec& spec)
{
    d_ptr->explicitSpec = spec;
    d_ptr->explicitSpecSet = true;
    d_ptr->specDirty = true;
    ensureSpecResolved();
    updateGeometry();
    update();
}

bool QtMaterialCarousel::hasExplicitSpec() const noexcept
{
    return d_ptr->explicitSpecSet;
}

void QtMaterialCarousel::resetSpec()
{
    if (!d_ptr->explicitSpecSet) {
        return;
    }

    d_ptr->explicitSpecSet = false;
    d_ptr->specDirty = true;
    ensureSpecResolved();
    updateGeometry();
    update();
}

QSize QtMaterialCarousel::sizeHint() const
{
    const CarouselSpec& spec = resolvedSpec();
    const int cards = qMax(1, d_ptr->visibleItemCount);
    const int indicatorBand =
        spec.indicatorSize > 0
        ? spec.indicatorSize + spec.pageSpacing
        : 0;

    return QSize(
        spec.outerMargins.left()
            + spec.outerMargins.right()
            + cards * spec.itemSize.width()
            + qMax(0, cards - 1) * spec.pageSpacing,
        spec.outerMargins.top()
            + spec.outerMargins.bottom()
            + spec.itemSize.height()
            + indicatorBand);
}

QSize QtMaterialCarousel::minimumSizeHint() const
{
    const CarouselSpec& spec = resolvedSpec();
    const int indicatorBand =
        spec.indicatorSize > 0
        ? spec.indicatorSize + spec.pageSpacing
        : 0;

    return QSize(
        spec.outerMargins.left()
            + spec.outerMargins.right()
            + spec.minimumItemSize.width(),
        spec.outerMargins.top()
            + spec.outerMargins.bottom()
            + spec.minimumItemSize.height()
            + indicatorBand);
}

void QtMaterialCarousel::next()
{
    const int nextIndex =
        nextEnabledIndex(
            d_ptr.get(),
            d_ptr->currentIndex,
            1);

    if (nextIndex >= 0) {
        setCurrentIndex(nextIndex);
    }
}

void QtMaterialCarousel::previous()
{
    const int previousIndex =
        nextEnabledIndex(
            d_ptr.get(),
            d_ptr->currentIndex,
            -1);

    if (previousIndex >= 0) {
        setCurrentIndex(previousIndex);
    }
}

void QtMaterialCarousel::activateCurrentItem()
{
    if (
        isValidIndex(
            d_ptr.get(),
            d_ptr->currentIndex)
        && d_ptr->items
            .at(d_ptr->currentIndex)
            .enabled) {
        Q_EMIT itemActivated(
            d_ptr->currentIndex);
    }
}

void QtMaterialCarousel::invalidateResolvedSpec()
{
    d_ptr->specDirty = true;
}

void QtMaterialCarousel::stateChangedEvent()
{
    update();
}

void QtMaterialCarousel::themeChangedEvent(
    const QtMaterial::Theme& theme)
{
    QtMaterialControl::themeChangedEvent(theme);
    updateGeometry();
    update();
}

void QtMaterialCarousel::keyPressEvent(
    QKeyEvent* event)
{
    const bool rtl =
        layoutDirection() == Qt::RightToLeft;

    switch (event->key()) {
    case Qt::Key_Right:
        rtl ? previous() : next();
        event->accept();
        return;
    case Qt::Key_Left:
        rtl ? next() : previous();
        event->accept();
        return;
    case Qt::Key_Home:
        for (
            int index = 0;
            index < d_ptr->items.size();
            ++index) {
            if (d_ptr->items.at(index).enabled) {
                setCurrentIndex(index);
                break;
            }
        }
        event->accept();
        return;
    case Qt::Key_End:
        for (
            int index = d_ptr->items.size() - 1;
            index >= 0;
            --index) {
            if (d_ptr->items.at(index).enabled) {
                setCurrentIndex(index);
                break;
            }
        }
        event->accept();
        return;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space:
        activateCurrentItem();
        event->accept();
        return;
    default:
        break;
    }

    QtMaterialControl::keyPressEvent(event);
}

int QtMaterialCarousel::indexAtPosition(
    const QPoint& position) const
{
    const CarouselSpec& spec =
        resolvedSpec();

    for (
        int index = 0;
        index < d_ptr->items.size();
        ++index) {
        if (
            itemRect(
                this,
                d_ptr.get(),
                spec,
                index)
            .contains(position)) {
            return index;
        }
    }

    return -1;
}

void QtMaterialCarousel::mouseMoveEvent(
    QMouseEvent* event)
{
    int hovered =
        indexAtPosition(event->pos());

    if (
        hovered >= 0
        && !d_ptr->items.at(hovered).enabled) {
        hovered = -1;
    }

    if (d_ptr->hoveredIndex != hovered) {
        d_ptr->hoveredIndex = hovered;
        update();
    }

    QtMaterialControl::mouseMoveEvent(event);
}

void QtMaterialCarousel::mousePressEvent(
    QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        const int pressed =
            indexAtPosition(event->pos());

        d_ptr->pressedIndex =
            pressed >= 0
                && d_ptr->items
                    .at(pressed)
                    .enabled
            ? pressed
            : -1;

        if (d_ptr->pressedIndex >= 0) {
            update();
        }
    }

    QtMaterialControl::mousePressEvent(event);
}

void QtMaterialCarousel::mouseReleaseEvent(
    QMouseEvent* event)
{
    const int pressed =
        d_ptr->pressedIndex;
    d_ptr->pressedIndex = -1;

    if (event->button() != Qt::LeftButton) {
        update();
        QtMaterialControl::mouseReleaseEvent(event);
        return;
    }

    const int released =
        indexAtPosition(event->pos());

    if (
        released >= 0
        && released == pressed
        && d_ptr->items.at(released).enabled) {
        setCurrentIndex(released);
        Q_EMIT itemClicked(released);
        Q_EMIT itemActivated(released);
        event->accept();
        update();
        return;
    }

    update();
    QtMaterialControl::mouseReleaseEvent(event);
}

void QtMaterialCarousel::leaveEvent(
    QEvent* event)
{
    d_ptr->hoveredIndex = -1;
    d_ptr->pressedIndex = -1;
    update();

    QtMaterialControl::leaveEvent(event);
}

void QtMaterialCarousel::paintEvent(
    QPaintEvent*)
{
    const CarouselSpec& spec =
        resolvedSpec();

    QPainter painter(this);
    painter.setRenderHint(
        QPainter::Antialiasing,
        true);

    painter.fillRect(
        rect(),
        spec.backgroundColor);

    for (
        int index = 0;
        index < d_ptr->items.size();
        ++index) {
        const QRect itemBounds =
            itemRect(
                this,
                d_ptr.get(),
                spec,
                index);

        if (itemBounds.isNull()) {
            continue;
        }

        const Item& item =
            d_ptr->items.at(index);
        const bool current =
            index == d_ptr->currentIndex;
        const bool enabled = item.enabled;
        const bool hovered =
            index == d_ptr->hoveredIndex;
        const bool pressed =
            index == d_ptr->pressedIndex;

        const QColor fill =
            current
            ? spec.itemSelectedColor
            : spec.itemBackgroundColor;
        const QColor outline =
            current
            ? spec.selectedOutlineColor
            : spec.outlineColor;
        const int outlineWidth =
            current
            ? spec.selectedOutlineWidth
            : spec.outlineWidth;

        const QRectF roundedBounds =
            QRectF(itemBounds)
            .adjusted(
                0.5,
                0.5,
                -0.5,
                -0.5);

        painter.setPen(
            outlineWidth > 0
            ? QPen(outline, outlineWidth)
            : QPen(Qt::NoPen));
        painter.setBrush(fill);
        painter.drawRoundedRect(
            roundedBounds,
            spec.cornerRadius,
            spec.cornerRadius);

        if (enabled && (hovered || pressed)) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(
                pressed
                ? spec.itemPressedColor
                : spec.itemHoverColor);
            painter.drawRoundedRect(
                roundedBounds,
                spec.cornerRadius,
                spec.cornerRadius);
        }

        QRect content =
            itemBounds.marginsRemoved(
                spec.contentMargins);

        const QColor titleColor =
            enabled
            ? (
                current
                ? spec.itemSelectedTextColor
                : spec.foregroundColor)
            : spec.disabledTextColor;
        const QColor supportingColor =
            enabled
            ? (
                current
                ? spec.itemSelectedTextColor
                : spec.supportingTextColor)
            : spec.disabledTextColor;
        const QColor iconColor =
            enabled
            ? (
                current
                ? spec.selectedIconColor
                : spec.iconColor)
            : spec.disabledTextColor;

        if (!item.icon.isNull()) {
            const QSize iconSize(
                spec.iconSize,
                spec.iconSize);
            const QRect iconRect(
                content.topLeft(),
                iconSize);

            const QPixmap iconPixmap =
                tintedIconPixmap(
                    item.icon,
                    spec.iconSize,
                    iconColor,
                    enabled);

            painter.drawPixmap(
                iconRect,
                iconPixmap);

            content.setTop(
                iconRect.bottom()
                + 1
                + spec.iconSpacing);
        }

        painter.setPen(titleColor);
        painter.setFont(spec.labelFont);

        const QFontMetrics titleMetrics(
            spec.labelFont);
        const QString title =
            titleMetrics.elidedText(
                item.title,
                Qt::ElideRight,
                content.width());

        painter.drawText(
            content,
            Qt::AlignLeft | Qt::AlignTop,
            title);

        if (!item.supportingText.isEmpty()) {
            painter.setPen(supportingColor);
            painter.setFont(spec.supportingFont);

            const QRect supportRect =
                content.adjusted(
                    0,
                    titleMetrics.height()
                        + spec.supportingTopSpacing,
                    0,
                    0);

            const QFontMetrics supportingMetrics(
                spec.supportingFont);
            const QString supportingText =
                supportingMetrics.elidedText(
                    item.supportingText,
                    Qt::ElideRight,
                    supportRect.width());

            painter.drawText(
                supportRect,
                Qt::AlignLeft | Qt::AlignTop,
                supportingText);
        }
    }

    // Resolved page indicators.
    if (spec.indicatorSize > 0) {
        const int itemCount = d_ptr->items.size();
        const int totalWidth =
            itemCount * spec.indicatorSize
            + qMax(0, itemCount - 1) * spec.pageSpacing;
        int x = (width() - totalWidth) / 2;
        const int y =
            height()
            - spec.outerMargins.bottom()
            - spec.indicatorSize;

        painter.setPen(Qt::NoPen);
        for (int index = 0; index < itemCount; ++index) {
            painter.setBrush(
                index == d_ptr->currentIndex
                ? spec.activePageIndicatorColor
                : spec.pageIndicatorColor);
            painter.drawEllipse(
                QRect(
                    x,
                    y,
                    spec.indicatorSize,
                    spec.indicatorSize));
            x += spec.indicatorSize + spec.pageSpacing;
        }
    }

    if (
        hasFocus()
        && spec.focusRingWidth > 0) {
        QtMaterialFocusIndicator::
            paintRectFocusRing(
                &painter,
                QRectF(rect()).adjusted(
                    3.0,
                    3.0,
                    -3.0,
                    -3.0),
                spec.focusRingColor,
                spec.cornerRadius,
                spec.focusRingWidth);
    }
}

void QtMaterialCarousel::changeEvent(
    QEvent* event)
{
    QtMaterialControl::changeEvent(event);

    if (
        event->type()
            == QEvent::LayoutDirectionChange) {
        update();
    } else if (
        event->type() == QEvent::FontChange) {
        invalidateResolvedSpec();
        updateGeometry();
        update();
    }
}

void QtMaterialCarousel::ensureSpecResolved() const
{
    if (!d_ptr->specDirty) {
        return;
    }

    if (d_ptr->explicitSpecSet) {
        d_ptr->spec = d_ptr->explicitSpec;
        d_ptr->specDirty = false;
        return;
    }

    d_ptr->spec =
        DataSpecResolver().carouselSpec(
            theme(),
            density());
    d_ptr->specDirty = false;
}

} // namespace QtMaterial
