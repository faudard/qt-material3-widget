#include "qtmaterial/widgets/navigation/qtmaterialnavigationrail.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QStringList>
#include <QtGlobal>

#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include <memory>
#include <QEvent>
#include <QPixmap>
#include "qtmaterial/specs/qtmaterialnavigationrailspecresolver.h"

namespace QtMaterial {

struct QtMaterialNavigationRailPrivate
{
    struct Destination
    {
        QString text;
        QIcon icon;
        bool enabled = true;
    };

    QVector<Destination> m_destinations;
    int m_currentIndex = -1;
    int m_hoveredIndex = -1;
    int m_pressedIndex = -1;
    bool m_labelsVisible = true;
    QString m_lastAccessibilitySummary;
    mutable bool m_specDirty = true;
    mutable NavigationRailSpec m_spec;

    QRect itemRect(int index) const
    {
        const int y =
            m_spec.topPadding
            + index
                * (m_spec.itemHeight + m_spec.itemSpacing);
        return QRect(
            0,
            y,
            m_spec.railWidth,
            m_spec.itemHeight);
    }

    QRect indicatorRect(const QRect& item) const
    {
        return QRect(
            item.center().x()
                - m_spec.indicatorSize.width() / 2,
            item.top() + m_spec.indicatorTopOffset,
            m_spec.indicatorSize.width(),
            m_spec.indicatorSize.height());
    }

    int indexAt(const QPoint& position) const
    {
        for (int index = 0;
             index < m_destinations.size();
             ++index) {
            if (itemRect(index).contains(position)) {
                return index;
            }
        }
        return -1;
    }

    int firstEnabledIndex() const noexcept
    {
        for (int index = 0;
             index < m_destinations.size();
             ++index) {
            if (m_destinations.at(index).enabled) {
                return index;
            }
        }
        return -1;
    }

    int lastEnabledIndex() const noexcept
    {
        for (int index = m_destinations.size() - 1;
             index >= 0;
             --index) {
            if (m_destinations.at(index).enabled) {
                return index;
            }
        }
        return -1;
    }

    int nextEnabledIndex(int from, int step) const noexcept
    {
        if (m_destinations.isEmpty() || step == 0) {
            return -1;
        }

        int index = from;
        if (index < 0 || index >= m_destinations.size()) {
            index = step > 0 ? -1 : m_destinations.size();
        }

        for (int attempts = 0;
             attempts < m_destinations.size();
             ++attempts) {
            index =
                (index + step + m_destinations.size())
                % m_destinations.size();
            if (m_destinations.at(index).enabled) {
                return index;
            }
        }
        return -1;
    }
};

void syncAccessibility(QtMaterialNavigationRail* rail, QtMaterialNavigationRailPrivate* d);

namespace {
QColor withOpacity(QColor color, qreal opacity)
{
    color.setAlphaF(
        qBound<qreal>(
            0.0,
            color.alphaF() * opacity,
            1.0));
    return color;
}

void paintTintedIcon(
    QPainter* painter,
    const QIcon& icon,
    const QRect& target,
    const QColor& color,
    bool enabled)
{
    if (!painter || icon.isNull() || !target.isValid()) {
        return;
    }

    QPixmap pixmap =
        icon.pixmap(
            target.size(),
            enabled ? QIcon::Normal : QIcon::Disabled,
            QIcon::Off);
    if (pixmap.isNull()) {
        return;
    }

    QPainter tintPainter(&pixmap);
    tintPainter.setCompositionMode(
        QPainter::CompositionMode_SourceIn);
    tintPainter.fillRect(pixmap.rect(), color);
    tintPainter.end();

    painter->drawPixmap(target, pixmap);
}

} // namespace

QtMaterialNavigationRail::QtMaterialNavigationRail(QWidget* parent)
    : QtMaterialControl(parent)
{
    d_ptr = std::make_unique<QtMaterialNavigationRailPrivate>();

    setFocusPolicy(Qt::StrongFocus);
    setAccessibleName(tr("Navigation rail"));
    syncAccessibility(this, d_ptr.get());

    setMouseTracking(true);
}

QtMaterialNavigationRail::~QtMaterialNavigationRail() = default;

int QtMaterialNavigationRail::addDestination(const QString& text, const QIcon& icon)
{
    insertDestination(d_ptr->m_destinations.size(), text, icon);
    return d_ptr->m_destinations.size() - 1;
}

void QtMaterialNavigationRail::insertDestination(int index, const QString& text, const QIcon& icon)
{
    index = qBound(0, index, d_ptr->m_destinations.size());
    d_ptr->m_destinations.insert(index, QtMaterialNavigationRailPrivate::Destination{text, icon, true});

    if (d_ptr->m_currentIndex >= index) {
        ++d_ptr->m_currentIndex;
    }

    updateGeometry();
    update();
    syncAccessibility(this, d_ptr.get());
}

void QtMaterialNavigationRail::removeDestination(int index)
{
    if (index < 0 || index >= d_ptr->m_destinations.size()) {
        return;
    }

    const bool removedCurrent = d_ptr->m_currentIndex == index;
    d_ptr->m_destinations.removeAt(index);

    int newCurrentIndex = d_ptr->m_currentIndex;
    if (removedCurrent) {
        newCurrentIndex = -1;
        if (!d_ptr->m_destinations.isEmpty()) {
            const int start = qMin(index, d_ptr->m_destinations.size() - 1);
            for (int i = start; i < d_ptr->m_destinations.size(); ++i) {
                if (d_ptr->m_destinations.at(i).enabled) {
                    newCurrentIndex = i;
                    break;
                }
            }
            if (newCurrentIndex < 0) {
                for (int i = start - 1; i >= 0; --i) {
                    if (d_ptr->m_destinations.at(i).enabled) {
                        newCurrentIndex = i;
                        break;
                    }
                }
            }
        }
    } else if (d_ptr->m_currentIndex > index) {
        newCurrentIndex = d_ptr->m_currentIndex - 1;
    }

    if (d_ptr->m_currentIndex != newCurrentIndex) {
        d_ptr->m_currentIndex = newCurrentIndex;
        emit currentIndexChanged(d_ptr->m_currentIndex);
    }

    updateGeometry();
    update();
    syncAccessibility(this, d_ptr.get());
}

void QtMaterialNavigationRail::clearDestinations()
{
    if (d_ptr->m_destinations.isEmpty() && d_ptr->m_currentIndex == -1) {
        return;
    }

    d_ptr->m_destinations.clear();
    const bool changed = d_ptr->m_currentIndex != -1;
    d_ptr->m_currentIndex = -1;

    updateGeometry();
    update();

    if (changed) {
        emit currentIndexChanged(-1);
    }

    syncAccessibility(this, d_ptr.get());
}

int QtMaterialNavigationRail::count() const noexcept
{
    return d_ptr->m_destinations.size();
}

QString QtMaterialNavigationRail::destinationText(int index) const
{
    return index >= 0 && index < d_ptr->m_destinations.size() ? d_ptr->m_destinations.at(index).text : QString();
}

QIcon QtMaterialNavigationRail::destinationIcon(int index) const
{
    return index >= 0 && index < d_ptr->m_destinations.size() ? d_ptr->m_destinations.at(index).icon : QIcon();
}

bool QtMaterialNavigationRail::isDestinationEnabled(int index) const noexcept
{
    return index >= 0 && index < d_ptr->m_destinations.size() ? d_ptr->m_destinations.at(index).enabled : false;
}

void QtMaterialNavigationRail::setDestinationEnabled(int index, bool enabled)
{
    if (index < 0 || index >= d_ptr->m_destinations.size() || d_ptr->m_destinations.at(index).enabled == enabled) {
        return;
    }

    d_ptr->m_destinations[index].enabled = enabled;
    emit destinationEnabledChanged(index, enabled);

    if (!enabled && d_ptr->m_currentIndex == index) {
        const int replacement = d_ptr->nextEnabledIndex(index, 1);
        setCurrentIndex(replacement == index ? -1 : replacement);
    }

    update();
    syncAccessibility(this, d_ptr.get());
}

QString QtMaterialNavigationRail::destinationAccessibleText(int index) const
{
    if (index < 0 || index >= d_ptr->m_destinations.size()) {
        return QString();
    }

    const auto& destination = d_ptr->m_destinations.at(index);
    QStringList parts;
    parts << destination.text;
    parts << tr("%1 of %2").arg(index + 1).arg(d_ptr->m_destinations.size());

    if (index == d_ptr->m_currentIndex) {
        parts << tr("selected");
    }
    if (!destination.enabled) {
        parts << tr("disabled");
    }

    return parts.join(QStringLiteral(", "));
}

QString QtMaterialNavigationRail::accessibilitySummary() const
{
    QString summary = tr("%n destination(s)", nullptr, d_ptr->m_destinations.size());
    if (d_ptr->m_currentIndex >= 0 && d_ptr->m_currentIndex < d_ptr->m_destinations.size()) {
        summary += tr(", selected %1").arg(d_ptr->m_destinations.at(d_ptr->m_currentIndex).text);
    }
    return summary;
}

int QtMaterialNavigationRail::currentIndex() const noexcept
{
    return d_ptr->m_currentIndex;
}

void QtMaterialNavigationRail::setCurrentIndex(int index)
{
    if (index < -1 || index >= d_ptr->m_destinations.size() || d_ptr->m_currentIndex == index) {
        return;
    }

    if (index >= 0 && !d_ptr->m_destinations.at(index).enabled) {
        return;
    }

    d_ptr->m_currentIndex = index;
    update();
    emit currentIndexChanged(index);
    syncAccessibility(this, d_ptr.get());
}

bool QtMaterialNavigationRail::labelsVisible() const noexcept
{
    return d_ptr->m_labelsVisible;
}

void QtMaterialNavigationRail::setLabelsVisible(bool visible)
{
    if (d_ptr->m_labelsVisible == visible) {
        return;
    }

    d_ptr->m_labelsVisible = visible;
    updateGeometry();
    update();
    emit labelsVisibleChanged(visible);
    syncAccessibility(this, d_ptr.get());
}

void QtMaterialNavigationRail::themeChangedEvent(
    const Theme& theme)
{
    QtMaterialControl::themeChangedEvent(theme);
    invalidateResolvedSpec();
    ensureSpecResolved();
    updateGeometry();
    update();
}

void QtMaterialNavigationRail::invalidateResolvedSpec()
{
    d_ptr->m_specDirty = true;
}


void syncAccessibility(QtMaterialNavigationRail* rail, QtMaterialNavigationRailPrivate* d)
{
    const QString summary = rail->accessibilitySummary();
    rail->setAccessibleDescription(summary);
    if (summary != d->m_lastAccessibilitySummary) {
        d->m_lastAccessibilitySummary = summary;
        emit rail->accessibilitySummaryChanged(summary);
    }
}

void QtMaterialNavigationRail::ensureSpecResolved() const
{
    if (!d_ptr->m_specDirty) {
        return;
    }

    const NavigationRailSpecResolver resolver;
    d_ptr->m_spec =
        resolver.navigationRailSpec(theme());
    d_ptr->m_specDirty = false;
}

const NavigationRailSpec&
QtMaterialNavigationRail::resolvedSpec() const
{
    ensureSpecResolved();
    return d_ptr->m_spec;
}

QSize QtMaterialNavigationRail::sizeHint() const
{
    ensureSpecResolved();

    const int height =
        d_ptr->m_spec.topPadding
        + d_ptr->m_spec.bottomPadding
        + d_ptr->m_destinations.size()
            * d_ptr->m_spec.itemHeight
        + qMax(
            0,
            d_ptr->m_destinations.size() - 1)
            * d_ptr->m_spec.itemSpacing;

    return QSize(
        d_ptr->m_spec.railWidth,
        qMax(120, height));
}

QSize QtMaterialNavigationRail::minimumSizeHint() const
{
    ensureSpecResolved();
    return QSize(d_ptr->m_spec.railWidth, 120);
}

void QtMaterialNavigationRail::mousePressEvent(
    QMouseEvent* event)
{
    if (!event) {
        return;
    }

    ensureSpecResolved();
    const int index =
        d_ptr->indexAt(event->pos());

    if (index >= 0 && isDestinationEnabled(index)) {
        d_ptr->m_pressedIndex = index;
        update();
        setCurrentIndex(index);
        emit destinationActivated(index);
        event->accept();
        return;
    }

    QtMaterialControl::mousePressEvent(event);
}

void QtMaterialNavigationRail::mouseMoveEvent(
    QMouseEvent* event)
{
    if (!event) {
        return;
    }

    ensureSpecResolved();
    const int hovered =
        d_ptr->indexAt(event->pos());

    if (d_ptr->m_hoveredIndex != hovered) {
        d_ptr->m_hoveredIndex = hovered;
        update();
    }

    QtMaterialControl::mouseMoveEvent(event);
}

void QtMaterialNavigationRail::mouseReleaseEvent(
    QMouseEvent* event)
{
    if (d_ptr->m_pressedIndex >= 0) {
        d_ptr->m_pressedIndex = -1;
        update();
    }

    QtMaterialControl::mouseReleaseEvent(event);
}

void QtMaterialNavigationRail::leaveEvent(
    QEvent* event)
{
    d_ptr->m_hoveredIndex = -1;
    d_ptr->m_pressedIndex = -1;
    update();
    QtMaterialControl::leaveEvent(event);
}

void QtMaterialNavigationRail::keyPressEvent(QKeyEvent* event)
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

    if (key == Qt::Key_Up || key == Qt::Key_Left || key == Qt::Key_Down || key == Qt::Key_Right) {
        const int step = (key == Qt::Key_Down || key == Qt::Key_Right) ? 1 : -1;
        setCurrentIndex(d_ptr->nextEnabledIndex(d_ptr->m_currentIndex, step));
        event->accept();
        return;
    }

    if ((key == Qt::Key_Return || key == Qt::Key_Enter || key == Qt::Key_Space)
        && d_ptr->m_currentIndex >= 0 && isDestinationEnabled(d_ptr->m_currentIndex)) {
        emit destinationActivated(d_ptr->m_currentIndex);
        event->accept();
        return;
    }

    QtMaterialControl::keyPressEvent(event);
}

void QtMaterialNavigationRail::paintEvent(QPaintEvent*)
{
    ensureSpecResolved();
    const NavigationRailSpec& spec = d_ptr->m_spec;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), spec.containerColor);

    if (spec.showDivider && spec.dividerWidth > 0) {
        painter.fillRect(
            QRect(
                width() - spec.dividerWidth,
                0,
                spec.dividerWidth,
                height()),
            spec.dividerColor);
    }

    painter.setFont(
        spec.hasResolvedLabelFont
        ? spec.labelFont
        : font());

    const qreal radius =
        spec.indicatorRadius < 0.0
        ? spec.indicatorSize.height() / 2.0
        : qMin(
            spec.indicatorRadius,
            spec.indicatorSize.height() / 2.0);

    for (int index = 0;
         index < d_ptr->m_destinations.size();
         ++index) {
        const QRect item = d_ptr->itemRect(index);
        const QRect indicatorRect =
            d_ptr->indicatorRect(item);
        const bool selected =
            index == d_ptr->m_currentIndex;
        const bool enabled =
            d_ptr->m_destinations.at(index).enabled
            && isEnabled();

        QPainterPath indicatorPath;
        indicatorPath.addRoundedRect(
            QRectF(indicatorRect),
            radius,
            radius);

        if (selected) {
            painter.fillPath(
                indicatorPath,
                spec.indicatorColor);
        }

        qreal stateOpacity = 0.0;
        if (enabled) {
            if (index == d_ptr->m_pressedIndex) {
                stateOpacity =
                    spec.pressStateLayerOpacity;
            } else if (index == d_ptr->m_hoveredIndex) {
                stateOpacity =
                    spec.hoverStateLayerOpacity;
            } else if (
                hasFocus()
                && index == d_ptr->m_currentIndex) {
                stateOpacity =
                    spec.focusStateLayerOpacity;
            }
        }

        if (stateOpacity > 0.0) {
            painter.fillPath(
                indicatorPath,
                withOpacity(
                    spec.stateLayerColor,
                    stateOpacity));
        }

        QColor iconColor =
            selected
            ? spec.selectedIconColor
            : spec.unselectedIconColor;
        QColor labelColor =
            selected
            ? spec.selectedLabelColor
            : spec.unselectedLabelColor;

        if (!enabled) {
            iconColor =
                withOpacity(
                    iconColor,
                    spec.disabledOpacity);
            labelColor =
                withOpacity(
                    labelColor,
                    spec.disabledOpacity);
        }

        const QRect iconRect(
            item.center().x() - spec.iconSize / 2,
            item.top() + spec.iconTopOffset,
            spec.iconSize,
            spec.iconSize);

        const QIcon& icon =
            d_ptr->m_destinations.at(index).icon;
        if (!icon.isNull()) {
            paintTintedIcon(
                &painter,
                icon,
                iconRect,
                iconColor,
                enabled);
        } else {
            painter.setPen(Qt::NoPen);
            painter.setBrush(iconColor);
            painter.drawEllipse(
                iconRect.adjusted(4, 4, -4, -4));
        }

        if (d_ptr->m_labelsVisible) {
            painter.setPen(labelColor);
            painter.drawText(
                QRect(
                    item.left() + 4,
                    item.top() + spec.labelTopOffset,
                    item.width() - 8,
                    spec.labelHeight),
                Qt::AlignCenter,
                d_ptr->m_destinations.at(index).text);
        }
    }

    if (hasFocus() && d_ptr->m_currentIndex >= 0) {
        const QRect item =
            d_ptr->itemRect(d_ptr->m_currentIndex);
        const QRect indicator =
            d_ptr->indicatorRect(item);

        QPainterPath focusPath;
        focusPath.addRoundedRect(
            QRectF(indicator).adjusted(-2, -2, 2, 2),
            radius,
            radius);

        QtMaterialFocusIndicator::paintPathFocusRing(
            &painter,
            focusPath,
            spec.focusRingColor,
            spec.focusRingWidth);
    }
}

} // namespace QtMaterial
