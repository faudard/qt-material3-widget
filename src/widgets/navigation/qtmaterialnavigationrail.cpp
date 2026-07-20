#include "qtmaterial/widgets/navigation/qtmaterialnavigationrail.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QStringList>

#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/specs/qtmaterialnavigationrailspecresolver.h"

namespace QtMaterial {

struct QtMaterialNavigationRailPrivate
{
    struct Destination {
        QString text;
        QIcon icon;
        bool enabled = true;
    };

    QVector<Destination> destinations;
    int currentIndex = -1;
    int hoveredIndex = -1;
    int pressedIndex = -1;
    bool labelsVisible = true;
    QString lastAccessibilitySummary;

    mutable bool specDirty = true;
    mutable NavigationRailSpec spec;

    QRect itemRect(int index) const
    {
        const int y =
            spec.topPadding
            + index
                * (spec.itemHeight + spec.itemSpacing);
        return QRect(
            0,
            y,
            spec.railWidth,
            spec.itemHeight);
    }

    QRect indicatorRect(const QRect& item) const
    {
        return QRect(
            item.center().x()
                - spec.indicatorSize.width() / 2,
            item.top() + spec.indicatorTopOffset,
            spec.indicatorSize.width(),
            spec.indicatorSize.height());
    }

    int indexAt(const QPoint& position) const
    {
        for (int index = 0;
             index < destinations.size();
             ++index) {
            if (itemRect(index).contains(position)) {
                return index;
            }
        }
        return -1;
    }

    bool enabledIndex(int index) const noexcept
    {
        return index >= 0
            && index < destinations.size()
            && destinations.at(index).enabled;
    }

    int firstEnabledIndex() const noexcept
    {
        for (int index = 0;
             index < destinations.size();
             ++index) {
            if (destinations.at(index).enabled) {
                return index;
            }
        }
        return -1;
    }

    int lastEnabledIndex() const noexcept
    {
        for (int index = destinations.size() - 1;
             index >= 0;
             --index) {
            if (destinations.at(index).enabled) {
                return index;
            }
        }
        return -1;
    }

    int nextEnabledIndex(
        int from,
        int step) const noexcept
    {
        if (destinations.isEmpty() || step == 0) {
            return -1;
        }

        int index = from;
        if (index < 0 || index >= destinations.size()) {
            index = step > 0
                ? -1
                : destinations.size();
        }

        for (int attempts = 0;
             attempts < destinations.size();
             ++attempts) {
            index =
                (index + step + destinations.size())
                % destinations.size();
            if (destinations.at(index).enabled) {
                return index;
            }
        }
        return -1;
    }
};

namespace {

QColor withOpacity(
    QColor color,
    qreal opacity)
{
    color.setAlphaF(qBound(0.0, opacity, 1.0));
    return color;
}

QPixmap tintedIconPixmap(
    const QIcon& icon,
    int size,
    const QColor& color,
    bool enabled)
{
    if (icon.isNull() || size <= 0) {
        return {};
    }

    QPixmap pixmap = icon.pixmap(
        QSize(size, size),
        enabled ? QIcon::Normal : QIcon::Disabled);
    if (pixmap.isNull()) {
        return {};
    }

    QPainter painter(&pixmap);
    painter.setCompositionMode(
        QPainter::CompositionMode_SourceIn);
    painter.fillRect(pixmap.rect(), color);
    return pixmap;
}

} // namespace

QtMaterialNavigationRail::QtMaterialNavigationRail(
    QWidget* parent)
    : QtMaterialControl(parent)
    , d_ptr(
          std::make_unique<
              QtMaterialNavigationRailPrivate>())
{
    setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setAccessibleName(tr("Navigation rail"));
    syncAccessibility();
}

QtMaterialNavigationRail::~QtMaterialNavigationRail() =
    default;

int QtMaterialNavigationRail::addDestination(
    const QString& text,
    const QIcon& icon)
{
    insertDestination(
        d_ptr->destinations.size(),
        text,
        icon);
    return d_ptr->destinations.size() - 1;
}

void QtMaterialNavigationRail::insertDestination(
    int index,
    const QString& text,
    const QIcon& icon)
{
    index = qBound(
        0,
        index,
        d_ptr->destinations.size());
    d_ptr->destinations.insert(
        index,
        QtMaterialNavigationRailPrivate::Destination{
            text,
            icon,
            true});

    if (d_ptr->currentIndex >= index) {
        ++d_ptr->currentIndex;
    }
    if (d_ptr->hoveredIndex >= index) {
        ++d_ptr->hoveredIndex;
    }
    if (d_ptr->pressedIndex >= index) {
        ++d_ptr->pressedIndex;
    }

    updateGeometry();
    update();
    syncAccessibility();
}

void QtMaterialNavigationRail::removeDestination(
    int index)
{
    if (index < 0
        || index >= d_ptr->destinations.size()) {
        return;
    }

    const bool removedCurrent =
        d_ptr->currentIndex == index;
    d_ptr->destinations.removeAt(index);

    if (d_ptr->hoveredIndex == index) {
        d_ptr->hoveredIndex = -1;
    } else if (d_ptr->hoveredIndex > index) {
        --d_ptr->hoveredIndex;
    }

    if (d_ptr->pressedIndex == index) {
        d_ptr->pressedIndex = -1;
    } else if (d_ptr->pressedIndex > index) {
        --d_ptr->pressedIndex;
    }

    int nextCurrent = d_ptr->currentIndex;
    if (removedCurrent) {
        nextCurrent = -1;
        if (!d_ptr->destinations.isEmpty()) {
            const int start = qMin(
                index,
                d_ptr->destinations.size() - 1);
            for (int candidate = start;
                 candidate < d_ptr->destinations.size();
                 ++candidate) {
                if (d_ptr->destinations.at(candidate).enabled) {
                    nextCurrent = candidate;
                    break;
                }
            }
            if (nextCurrent < 0) {
                for (int candidate = start - 1;
                     candidate >= 0;
                     --candidate) {
                    if (d_ptr->destinations
                            .at(candidate).enabled) {
                        nextCurrent = candidate;
                        break;
                    }
                }
            }
        }
    } else if (d_ptr->currentIndex > index) {
        nextCurrent = d_ptr->currentIndex - 1;
    }

    if (d_ptr->currentIndex != nextCurrent) {
        d_ptr->currentIndex = nextCurrent;
        Q_EMIT currentIndexChanged(nextCurrent);
    }

    updateGeometry();
    update();
    syncAccessibility();
}

void QtMaterialNavigationRail::clearDestinations()
{
    if (d_ptr->destinations.isEmpty()
        && d_ptr->currentIndex == -1) {
        return;
    }

    d_ptr->destinations.clear();
    d_ptr->hoveredIndex = -1;
    d_ptr->pressedIndex = -1;

    const bool currentChanged =
        d_ptr->currentIndex != -1;
    d_ptr->currentIndex = -1;

    updateGeometry();
    update();
    if (currentChanged) {
        Q_EMIT currentIndexChanged(-1);
    }
    syncAccessibility();
}

int QtMaterialNavigationRail::count() const noexcept
{
    return d_ptr->destinations.size();
}

QString QtMaterialNavigationRail::destinationText(
    int index) const
{
    return index >= 0
        && index < d_ptr->destinations.size()
        ? d_ptr->destinations.at(index).text
        : QString();
}

QIcon QtMaterialNavigationRail::destinationIcon(
    int index) const
{
    return index >= 0
        && index < d_ptr->destinations.size()
        ? d_ptr->destinations.at(index).icon
        : QIcon();
}

bool QtMaterialNavigationRail::isDestinationEnabled(
    int index) const noexcept
{
    return d_ptr->enabledIndex(index);
}

void QtMaterialNavigationRail::setDestinationEnabled(
    int index,
    bool enabled)
{
    if (index < 0
        || index >= d_ptr->destinations.size()
        || d_ptr->destinations.at(index).enabled
            == enabled) {
        return;
    }

    d_ptr->destinations[index].enabled = enabled;
    Q_EMIT destinationEnabledChanged(index, enabled);

    if (!enabled) {
        if (d_ptr->hoveredIndex == index) {
            d_ptr->hoveredIndex = -1;
        }
        if (d_ptr->pressedIndex == index) {
            d_ptr->pressedIndex = -1;
        }
        if (d_ptr->currentIndex == index) {
            const int replacement =
                d_ptr->nextEnabledIndex(index, 1);
            setCurrentIndex(
                replacement == index
                    ? -1
                    : replacement);
        }
    }

    update();
    syncAccessibility();
}

QString
QtMaterialNavigationRail::destinationAccessibleText(
    int index) const
{
    if (index < 0
        || index >= d_ptr->destinations.size()) {
        return {};
    }

    const auto& destination =
        d_ptr->destinations.at(index);
    QStringList parts;
    parts << destination.text;
    parts << tr("%1 of %2")
                 .arg(index + 1)
                 .arg(d_ptr->destinations.size());
    if (index == d_ptr->currentIndex) {
        parts << tr("selected");
    }
    if (!destination.enabled) {
        parts << tr("disabled");
    }
    return parts.join(QStringLiteral(", "));
}

QString
QtMaterialNavigationRail::accessibilitySummary() const
{
    QString summary =
        tr("%n destination(s)",
           nullptr,
           d_ptr->destinations.size());
    if (d_ptr->currentIndex >= 0
        && d_ptr->currentIndex
            < d_ptr->destinations.size()) {
        summary += tr(", selected %1")
            .arg(
                d_ptr->destinations
                    .at(d_ptr->currentIndex)
                    .text);
    }
    return summary;
}

int QtMaterialNavigationRail::currentIndex() const noexcept
{
    return d_ptr->currentIndex;
}

void QtMaterialNavigationRail::setCurrentIndex(
    int index)
{
    if (index < -1
        || index >= d_ptr->destinations.size()
        || d_ptr->currentIndex == index) {
        return;
    }
    if (index >= 0
        && !d_ptr->destinations.at(index).enabled) {
        return;
    }

    d_ptr->currentIndex = index;
    update();
    Q_EMIT currentIndexChanged(index);
    syncAccessibility();
}

bool QtMaterialNavigationRail::labelsVisible() const noexcept
{
    return d_ptr->labelsVisible;
}

void QtMaterialNavigationRail::setLabelsVisible(
    bool visible)
{
    if (d_ptr->labelsVisible == visible) {
        return;
    }

    d_ptr->labelsVisible = visible;
    updateGeometry();
    update();
    Q_EMIT labelsVisibleChanged(visible);
    syncAccessibility();
}

const NavigationRailSpec&
QtMaterialNavigationRail::resolvedSpec() const
{
    ensureSpecResolved();
    return d_ptr->spec;
}

QSize QtMaterialNavigationRail::sizeHint() const
{
    ensureSpecResolved();

    const int destinationCount =
        d_ptr->destinations.size();
    const int height =
        d_ptr->spec.topPadding
        + d_ptr->spec.bottomPadding
        + destinationCount * d_ptr->spec.itemHeight
        + qMax(0, destinationCount - 1)
            * d_ptr->spec.itemSpacing;

    return QSize(
        d_ptr->spec.railWidth,
        qMax(120, height));
}

QSize QtMaterialNavigationRail::minimumSizeHint() const
{
    ensureSpecResolved();
    return QSize(d_ptr->spec.railWidth, 120);
}

void QtMaterialNavigationRail::mouseMoveEvent(
    QMouseEvent* event)
{
    ensureSpecResolved();

    const int candidate = d_ptr->indexAt(event->pos());
    const int nextHover =
        d_ptr->enabledIndex(candidate)
        && isEnabled()
            ? candidate
            : -1;

    if (d_ptr->hoveredIndex != nextHover) {
        d_ptr->hoveredIndex = nextHover;
        update();
    }

    QtMaterialControl::mouseMoveEvent(event);
}

void QtMaterialNavigationRail::mousePressEvent(
    QMouseEvent* event)
{
    ensureSpecResolved();

    if (event->button() == Qt::LeftButton) {
        const int candidate =
            d_ptr->indexAt(event->pos());
        if (d_ptr->enabledIndex(candidate)
            && isEnabled()) {
            d_ptr->pressedIndex = candidate;
            update();
            event->accept();
            return;
        }
    }

    QtMaterialControl::mousePressEvent(event);
}

void QtMaterialNavigationRail::mouseReleaseEvent(
    QMouseEvent* event)
{
    ensureSpecResolved();

    const int pressed = d_ptr->pressedIndex;
    d_ptr->pressedIndex = -1;

    if (event->button() == Qt::LeftButton
        && pressed >= 0
        && pressed == d_ptr->indexAt(event->pos())
        && d_ptr->enabledIndex(pressed)
        && isEnabled()) {
        setCurrentIndex(pressed);
        Q_EMIT destinationActivated(pressed);
        update();
        event->accept();
        return;
    }

    update();
    QtMaterialControl::mouseReleaseEvent(event);
}

void QtMaterialNavigationRail::leaveEvent(
    QEvent* event)
{
    d_ptr->hoveredIndex = -1;
    d_ptr->pressedIndex = -1;
    update();
    QtMaterialControl::leaveEvent(event);
}

void QtMaterialNavigationRail::keyPressEvent(
    QKeyEvent* event)
{
    const int key = event->key();

    if (key == Qt::Key_Home) {
        setCurrentIndex(d_ptr->firstEnabledIndex());
        event->accept();
        return;
    }
    if (key == Qt::Key_End) {
        setCurrentIndex(d_ptr->lastEnabledIndex());
        event->accept();
        return;
    }
    if (key == Qt::Key_Up
        || key == Qt::Key_Left
        || key == Qt::Key_Down
        || key == Qt::Key_Right) {
        const int step =
            key == Qt::Key_Down
                || key == Qt::Key_Right
            ? 1
            : -1;
        setCurrentIndex(
            d_ptr->nextEnabledIndex(
                d_ptr->currentIndex,
                step));
        event->accept();
        return;
    }
    if ((key == Qt::Key_Return
         || key == Qt::Key_Enter
         || key == Qt::Key_Space)
        && d_ptr->enabledIndex(
            d_ptr->currentIndex)
        && isEnabled()) {
        Q_EMIT destinationActivated(
            d_ptr->currentIndex);
        event->accept();
        return;
    }

    QtMaterialControl::keyPressEvent(event);
}

void QtMaterialNavigationRail::paintEvent(QPaintEvent*)
{
    ensureSpecResolved();

    QPainter painter(this);
    painter.setRenderHint(
        QPainter::Antialiasing,
        true);
    painter.fillRect(
        rect(),
        d_ptr->spec.containerColor);
    painter.setFont(d_ptr->spec.labelFont);

    if (d_ptr->spec.showDivider) {
        painter.setPen(
            QPen(
                d_ptr->spec.dividerColor,
                d_ptr->spec.dividerWidth));
        const int x =
            layoutDirection() == Qt::RightToLeft
            ? 0
            : width() - d_ptr->spec.dividerWidth;
        painter.drawLine(x, 0, x, height());
    }

    const qreal indicatorRadius =
        d_ptr->spec.indicatorRadius >= 0.0
        ? d_ptr->spec.indicatorRadius
        : d_ptr->spec.indicatorSize.height() / 2.0;

    for (int index = 0;
         index < d_ptr->destinations.size();
         ++index) {
        const QRect item = d_ptr->itemRect(index);
        const QRect indicator =
            d_ptr->indicatorRect(item);
        const bool selected =
            index == d_ptr->currentIndex;
        const bool destinationEnabled =
            isEnabled()
            && d_ptr->destinations.at(index).enabled;

        QPainterPath indicatorPath;
        indicatorPath.addRoundedRect(
            QRectF(indicator),
            indicatorRadius,
            indicatorRadius);

        if (selected) {
            painter.fillPath(
                indicatorPath,
                d_ptr->spec.indicatorColor);
        }

        qreal stateOpacity = 0.0;
        if (destinationEnabled
            && index == d_ptr->pressedIndex) {
            stateOpacity =
                d_ptr->spec.pressStateLayerOpacity;
        } else if (destinationEnabled
                   && index == d_ptr->hoveredIndex) {
            stateOpacity =
                d_ptr->spec.hoverStateLayerOpacity;
        } else if (destinationEnabled
                   && selected
                   && hasFocus()) {
            stateOpacity =
                d_ptr->spec.focusStateLayerOpacity;
        }
        if (stateOpacity > 0.0) {
            painter.fillPath(
                indicatorPath,
                withOpacity(
                    d_ptr->spec.stateLayerColor,
                    stateOpacity));
        }

        QColor iconColor;
        QColor textColor;
        if (!destinationEnabled) {
            iconColor =
                d_ptr->spec.disabledIconColor;
            textColor =
                d_ptr->spec.disabledLabelColor;
        } else if (selected) {
            iconColor =
                d_ptr->spec.selectedIconColor;
            textColor =
                d_ptr->spec.selectedLabelColor;
        } else {
            iconColor =
                d_ptr->spec.unselectedIconColor;
            textColor =
                d_ptr->spec.unselectedLabelColor;
        }

        const QRect iconRect(
            item.center().x()
                - d_ptr->spec.iconSize / 2,
            item.top() + d_ptr->spec.iconTopOffset,
            d_ptr->spec.iconSize,
            d_ptr->spec.iconSize);

        const QPixmap iconPixmap = tintedIconPixmap(
            d_ptr->destinations.at(index).icon,
            d_ptr->spec.iconSize,
            iconColor,
            destinationEnabled);
        if (!iconPixmap.isNull()) {
            painter.drawPixmap(iconRect, iconPixmap);
        } else {
            painter.setPen(Qt::NoPen);
            painter.setBrush(iconColor);
            painter.drawEllipse(
                iconRect.adjusted(4, 4, -4, -4));
        }

        if (d_ptr->labelsVisible) {
            painter.setPen(textColor);
            painter.drawText(
                QRect(
                    item.left() + 4,
                    item.top()
                        + d_ptr->spec.labelTopOffset,
                    item.width() - 8,
                    d_ptr->spec.labelHeight),
                Qt::AlignCenter,
                d_ptr->destinations.at(index).text);
        }
    }

    if (hasFocus()
        && d_ptr->enabledIndex(
            d_ptr->currentIndex)) {
        const QRect currentItem =
            d_ptr->itemRect(d_ptr->currentIndex);
        const QRectF focusRect =
            QRectF(
                d_ptr->indicatorRect(currentItem))
                .adjusted(
                    -d_ptr->spec.focusRingWidth,
                    -d_ptr->spec.focusRingWidth,
                    d_ptr->spec.focusRingWidth,
                    d_ptr->spec.focusRingWidth);

        QPainterPath focusPath;
        focusPath.addRoundedRect(
            focusRect,
            indicatorRadius
                + d_ptr->spec.focusRingWidth,
            indicatorRadius
                + d_ptr->spec.focusRingWidth);
        QtMaterialFocusIndicator::paintPathFocusRing(
            &painter,
            focusPath,
            d_ptr->spec.focusRingColor,
            d_ptr->spec.focusRingWidth);
    }
}

void QtMaterialNavigationRail::themeChangedEvent(
    const Theme& theme)
{
    QtMaterialControl::themeChangedEvent(theme);
    invalidateResolvedSpec();
}

void QtMaterialNavigationRail::invalidateResolvedSpec()
{
    d_ptr->specDirty = true;
    updateGeometry();
    update();
}

void QtMaterialNavigationRail::ensureSpecResolved() const
{
    if (!d_ptr->specDirty) {
        return;
    }

    d_ptr->spec =
        NavigationRailSpecResolver()
            .navigationRailSpec(theme());
    d_ptr->specDirty = false;
}

void QtMaterialNavigationRail::syncAccessibility()
{
    const QString summary = accessibilitySummary();
    setAccessibleDescription(summary);

    if (summary != d_ptr->lastAccessibilitySummary) {
        d_ptr->lastAccessibilitySummary = summary;
        Q_EMIT accessibilitySummaryChanged(summary);
    }
}

} // namespace QtMaterial
