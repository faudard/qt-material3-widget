#include "qtmaterial/widgets/navigation/qtmaterialnavigationrail.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QStringList>
#include <QtGlobal>

#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include <memory>

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
    bool m_labelsVisible = true;
    QString m_lastAccessibilitySummary;
    mutable bool m_specDirty = true;
    mutable NavigationRailSpec m_spec;


    void ensureSpecResolved(const Theme& theme) const
    {
        if (!m_specDirty) {
            return;
        }

        SpecFactory factory;
        m_spec = factory.navigationRailSpec(theme);
        m_specDirty = false;
    }

    QRect itemRect(const Theme& theme, int index) const
    {
        ensureSpecResolved(theme);
        const int y = m_spec.topPadding + index * (m_spec.itemHeight + m_spec.itemSpacing);
        return QRect(0, y, m_spec.railWidth, m_spec.itemHeight);
    }

    QRect indicatorRect(const Theme& theme, const QRect& item) const
    {
        ensureSpecResolved(theme);
        return QRect(item.center().x() - m_spec.indicatorSize.width() / 2,
                     item.top() + 4,
                     m_spec.indicatorSize.width(),
                     m_spec.indicatorSize.height());
    }

    int indexAt(const Theme& theme, const QPoint& pos) const
    {
        for (int i = 0; i < m_destinations.size(); ++i) {
            if (itemRect(theme, i).contains(pos)) {
                return i;
            }
        }
        return -1;
    }

    int firstEnabledIndex() const noexcept
    {
        for (int i = 0; i < m_destinations.size(); ++i) {
            if (m_destinations.at(i).enabled) {
                return i;
            }
        }
        return -1;
    }

    int lastEnabledIndex() const noexcept
    {
        for (int i = m_destinations.size() - 1; i >= 0; --i) {
            if (m_destinations.at(i).enabled) {
                return i;
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

        for (int attempts = 0; attempts < m_destinations.size(); ++attempts) {
            index = (index + step + m_destinations.size()) % m_destinations.size();
            if (m_destinations.at(index).enabled) {
                return index;
            }
        }
        return -1;
    }
};

void syncAccessibility(QtMaterialNavigationRail* rail, QtMaterialNavigationRailPrivate* d);

namespace {
QColor disabledColor(QColor color)
{
    color.setAlphaF(color.alphaF() * 0.38);
    return color;
}
} // namespace

QtMaterialNavigationRail::QtMaterialNavigationRail(QWidget* parent)
    : QtMaterialControl(parent)
{
    d_ptr = std::make_unique<QtMaterialNavigationRailPrivate>();

    setFocusPolicy(Qt::StrongFocus);
    setAccessibleName(tr("Navigation rail"));
    syncAccessibility(this, d_ptr.get());
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

void QtMaterialNavigationRail::themeChangedEvent(const Theme& theme)
{
    QtMaterialControl::themeChangedEvent(theme);
    invalidateResolvedSpec();
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

QSize QtMaterialNavigationRail::sizeHint() const
{
    d_ptr->ensureSpecResolved(theme());
    const int height = d_ptr->m_spec.topPadding + d_ptr->m_spec.bottomPadding
        + d_ptr->m_destinations.size() * d_ptr->m_spec.itemHeight
        + qMax(0, d_ptr->m_destinations.size() - 1) * d_ptr->m_spec.itemSpacing;
    return QSize(d_ptr->m_spec.railWidth, qMax(120, height));
}

QSize QtMaterialNavigationRail::minimumSizeHint() const
{
    d_ptr->ensureSpecResolved(theme());
    return QSize(d_ptr->m_spec.railWidth, 120);
}

void QtMaterialNavigationRail::mousePressEvent(QMouseEvent* event)
{
    const int index = d_ptr->indexAt(theme(), event->pos());
    if (index >= 0 && isDestinationEnabled(index)) {
        setCurrentIndex(index);
        emit destinationActivated(index);
        event->accept();
        return;
    }

    QtMaterialControl::mousePressEvent(event);
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
    d_ptr->ensureSpecResolved(theme());

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), d_ptr->m_spec.containerColor);

    QFont labelFont = font();
    if (theme().typography().contains(d_ptr->m_spec.labelTypeRole)) {
        labelFont = theme().typography().style(d_ptr->m_spec.labelTypeRole).font;
    }
    painter.setFont(labelFont);

    const qreal indicatorRadius = theme().shapes().contains(d_ptr->m_spec.indicatorShapeRole)
        ? theme().shapes().radius(d_ptr->m_spec.indicatorShapeRole)
        : d_ptr->m_spec.indicatorSize.height() / 2.0;

    for (int i = 0; i < d_ptr->m_destinations.size(); ++i) {
        const QRect item = d_ptr->itemRect(theme(), i);
        const bool selected = i == d_ptr->m_currentIndex;
        const bool destinationEnabled = d_ptr->m_destinations.at(i).enabled && isEnabled();

        if (selected) {
            QPainterPath indicator;
            indicator.addRoundedRect(QRectF(d_ptr->indicatorRect(theme(), item)), indicatorRadius, indicatorRadius);
            painter.fillPath(indicator, d_ptr->m_spec.indicatorColor);
        }

        QColor iconColor = selected ? d_ptr->m_spec.selectedIconColor : d_ptr->m_spec.unselectedIconColor;
        QColor textColor = selected ? d_ptr->m_spec.selectedLabelColor : d_ptr->m_spec.unselectedLabelColor;
        if (!destinationEnabled) {
            iconColor = disabledColor(iconColor);
            textColor = disabledColor(textColor);
        }

        const QRect iconRect(item.center().x() - d_ptr->m_spec.iconSize / 2,
                             item.top() + 8,
                             d_ptr->m_spec.iconSize,
                             d_ptr->m_spec.iconSize);

        if (!d_ptr->m_destinations.at(i).icon.isNull()) {
            d_ptr->m_destinations.at(i).icon.paint(&painter,
                                            iconRect,
                                            Qt::AlignCenter,
                                            destinationEnabled ? QIcon::Normal : QIcon::Disabled);
        } else {
            painter.setPen(Qt::NoPen);
            painter.setBrush(iconColor);
            painter.drawEllipse(iconRect.adjusted(4, 4, -4, -4));
        }

        if (d_ptr->m_labelsVisible) {
            painter.setPen(textColor);
            painter.drawText(QRect(item.left() + 4, item.top() + 36, item.width() - 8, 22),
                             Qt::AlignCenter,
                             d_ptr->m_destinations.at(i).text);
        }
    }

    if (hasFocus() && d_ptr->m_currentIndex >= 0) {
        QPainterPath focusPath;
        const QRect focusItem = d_ptr->itemRect(theme(), d_ptr->m_currentIndex);
        focusPath.addRoundedRect(QRectF(d_ptr->indicatorRect(theme(), focusItem)).adjusted(-2, -2, 2, 2),
                                  indicatorRadius,
                                  indicatorRadius);
        QtMaterialFocusIndicator::paintPathFocusRing(&painter, focusPath, d_ptr->m_spec.focusRingColor, 2.0);
    }
}

} // namespace QtMaterial
