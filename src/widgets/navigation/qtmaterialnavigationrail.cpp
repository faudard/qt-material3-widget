#include "qtmaterial/widgets/navigation/qtmaterialnavigationrail.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QStringList>
#include <QtGlobal>

#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {

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
    setFocusPolicy(Qt::StrongFocus);
    setAccessibleName(tr("Navigation rail"));
    syncAccessibility();
}

QtMaterialNavigationRail::~QtMaterialNavigationRail() = default;

int QtMaterialNavigationRail::addDestination(const QString& text, const QIcon& icon)
{
    insertDestination(m_destinations.size(), text, icon);
    return m_destinations.size() - 1;
}

void QtMaterialNavigationRail::insertDestination(int index, const QString& text, const QIcon& icon)
{
    index = qBound(0, index, m_destinations.size());
    m_destinations.insert(index, Destination{text, icon, true});

    if (m_currentIndex >= index) {
        ++m_currentIndex;
    }

    updateGeometry();
    update();
    syncAccessibility();
}

void QtMaterialNavigationRail::removeDestination(int index)
{
    if (index < 0 || index >= m_destinations.size()) {
        return;
    }

    const bool removedCurrent = m_currentIndex == index;
    m_destinations.removeAt(index);

    int newCurrentIndex = m_currentIndex;
    if (removedCurrent) {
        newCurrentIndex = -1;
        if (!m_destinations.isEmpty()) {
            const int start = qMin(index, m_destinations.size() - 1);
            for (int i = start; i < m_destinations.size(); ++i) {
                if (m_destinations.at(i).enabled) {
                    newCurrentIndex = i;
                    break;
                }
            }
            if (newCurrentIndex < 0) {
                for (int i = start - 1; i >= 0; --i) {
                    if (m_destinations.at(i).enabled) {
                        newCurrentIndex = i;
                        break;
                    }
                }
            }
        }
    } else if (m_currentIndex > index) {
        newCurrentIndex = m_currentIndex - 1;
    }

    if (m_currentIndex != newCurrentIndex) {
        m_currentIndex = newCurrentIndex;
        emit currentIndexChanged(m_currentIndex);
    }

    updateGeometry();
    update();
    syncAccessibility();
}

void QtMaterialNavigationRail::clearDestinations()
{
    if (m_destinations.isEmpty() && m_currentIndex == -1) {
        return;
    }

    m_destinations.clear();
    const bool changed = m_currentIndex != -1;
    m_currentIndex = -1;

    updateGeometry();
    update();

    if (changed) {
        emit currentIndexChanged(-1);
    }

    syncAccessibility();
}

int QtMaterialNavigationRail::count() const noexcept
{
    return m_destinations.size();
}

QString QtMaterialNavigationRail::destinationText(int index) const
{
    return index >= 0 && index < m_destinations.size() ? m_destinations.at(index).text : QString();
}

QIcon QtMaterialNavigationRail::destinationIcon(int index) const
{
    return index >= 0 && index < m_destinations.size() ? m_destinations.at(index).icon : QIcon();
}

bool QtMaterialNavigationRail::isDestinationEnabled(int index) const noexcept
{
    return index >= 0 && index < m_destinations.size() ? m_destinations.at(index).enabled : false;
}

void QtMaterialNavigationRail::setDestinationEnabled(int index, bool enabled)
{
    if (index < 0 || index >= m_destinations.size() || m_destinations.at(index).enabled == enabled) {
        return;
    }

    m_destinations[index].enabled = enabled;
    emit destinationEnabledChanged(index, enabled);

    if (!enabled && m_currentIndex == index) {
        const int replacement = nextEnabledIndex(index, 1);
        setCurrentIndex(replacement == index ? -1 : replacement);
    }

    update();
    syncAccessibility();
}

QString QtMaterialNavigationRail::destinationAccessibleText(int index) const
{
    if (index < 0 || index >= m_destinations.size()) {
        return QString();
    }

    const auto& destination = m_destinations.at(index);
    QStringList parts;
    parts << destination.text;
    parts << tr("%1 of %2").arg(index + 1).arg(m_destinations.size());

    if (index == m_currentIndex) {
        parts << tr("selected");
    }
    if (!destination.enabled) {
        parts << tr("disabled");
    }

    return parts.join(QStringLiteral(", "));
}

QString QtMaterialNavigationRail::accessibilitySummary() const
{
    QString summary = tr("%n destination(s)", nullptr, m_destinations.size());
    if (m_currentIndex >= 0 && m_currentIndex < m_destinations.size()) {
        summary += tr(", selected %1").arg(m_destinations.at(m_currentIndex).text);
    }
    return summary;
}

int QtMaterialNavigationRail::currentIndex() const noexcept
{
    return m_currentIndex;
}

void QtMaterialNavigationRail::setCurrentIndex(int index)
{
    if (index < -1 || index >= m_destinations.size() || m_currentIndex == index) {
        return;
    }

    if (index >= 0 && !m_destinations.at(index).enabled) {
        return;
    }

    m_currentIndex = index;
    update();
    emit currentIndexChanged(index);
    syncAccessibility();
}

bool QtMaterialNavigationRail::labelsVisible() const noexcept
{
    return m_labelsVisible;
}

void QtMaterialNavigationRail::setLabelsVisible(bool visible)
{
    if (m_labelsVisible == visible) {
        return;
    }

    m_labelsVisible = visible;
    updateGeometry();
    update();
    emit labelsVisibleChanged(visible);
    syncAccessibility();
}

void QtMaterialNavigationRail::themeChangedEvent(const Theme& theme)
{
    QtMaterialControl::themeChangedEvent(theme);
    invalidateResolvedSpec();
}

void QtMaterialNavigationRail::invalidateResolvedSpec()
{
    m_specDirty = true;
}

void QtMaterialNavigationRail::ensureSpecResolved() const
{
    if (!m_specDirty) {
        return;
    }

    SpecFactory factory;
    m_spec = factory.navigationRailSpec(theme());
    m_specDirty = false;
}

QRect QtMaterialNavigationRail::itemRect(int index) const
{
    ensureSpecResolved();
    const int y = m_spec.topPadding + index * (m_spec.itemHeight + m_spec.itemSpacing);
    return QRect(0, y, m_spec.railWidth, m_spec.itemHeight);
}

QRect QtMaterialNavigationRail::indicatorRect(const QRect& item) const
{
    ensureSpecResolved();
    return QRect(item.center().x() - m_spec.indicatorSize.width() / 2,
                 item.top() + 4,
                 m_spec.indicatorSize.width(),
                 m_spec.indicatorSize.height());
}

int QtMaterialNavigationRail::indexAt(const QPoint& pos) const
{
    for (int i = 0; i < m_destinations.size(); ++i) {
        if (itemRect(i).contains(pos)) {
            return i;
        }
    }
    return -1;
}

int QtMaterialNavigationRail::firstEnabledIndex() const noexcept
{
    for (int i = 0; i < m_destinations.size(); ++i) {
        if (m_destinations.at(i).enabled) {
            return i;
        }
    }
    return -1;
}

int QtMaterialNavigationRail::lastEnabledIndex() const noexcept
{
    for (int i = m_destinations.size() - 1; i >= 0; --i) {
        if (m_destinations.at(i).enabled) {
            return i;
        }
    }
    return -1;
}

int QtMaterialNavigationRail::nextEnabledIndex(int from, int step) const noexcept
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

void QtMaterialNavigationRail::syncAccessibility()
{
    const QString summary = accessibilitySummary();
    setAccessibleDescription(summary);

    if (summary != m_lastAccessibilitySummary) {
        m_lastAccessibilitySummary = summary;
        emit accessibilitySummaryChanged(summary);
    }
}

QSize QtMaterialNavigationRail::sizeHint() const
{
    ensureSpecResolved();
    const int height = m_spec.topPadding + m_spec.bottomPadding
        + m_destinations.size() * m_spec.itemHeight
        + qMax(0, m_destinations.size() - 1) * m_spec.itemSpacing;
    return QSize(m_spec.railWidth, qMax(120, height));
}

QSize QtMaterialNavigationRail::minimumSizeHint() const
{
    ensureSpecResolved();
    return QSize(m_spec.railWidth, 120);
}

void QtMaterialNavigationRail::mousePressEvent(QMouseEvent* event)
{
    const int index = indexAt(event->pos());
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
        setCurrentIndex(firstEnabledIndex());
        event->accept();
        return;
    }

    if (key == Qt::Key_End) {
        setCurrentIndex(lastEnabledIndex());
        event->accept();
        return;
    }

    if (key == Qt::Key_Up || key == Qt::Key_Left || key == Qt::Key_Down || key == Qt::Key_Right) {
        const int step = (key == Qt::Key_Down || key == Qt::Key_Right) ? 1 : -1;
        setCurrentIndex(nextEnabledIndex(m_currentIndex, step));
        event->accept();
        return;
    }

    if ((key == Qt::Key_Return || key == Qt::Key_Enter || key == Qt::Key_Space)
        && m_currentIndex >= 0 && isDestinationEnabled(m_currentIndex)) {
        emit destinationActivated(m_currentIndex);
        event->accept();
        return;
    }

    QtMaterialControl::keyPressEvent(event);
}

void QtMaterialNavigationRail::paintEvent(QPaintEvent*)
{
    ensureSpecResolved();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), m_spec.containerColor);

    QFont labelFont = font();
    if (theme().typography().contains(m_spec.labelTypeRole)) {
        labelFont = theme().typography().style(m_spec.labelTypeRole).font;
    }
    painter.setFont(labelFont);

    const qreal indicatorRadius = theme().shapes().contains(m_spec.indicatorShapeRole)
        ? theme().shapes().radius(m_spec.indicatorShapeRole)
        : m_spec.indicatorSize.height() / 2.0;

    for (int i = 0; i < m_destinations.size(); ++i) {
        const QRect item = itemRect(i);
        const bool selected = i == m_currentIndex;
        const bool destinationEnabled = m_destinations.at(i).enabled && isEnabled();

        if (selected) {
            QPainterPath indicator;
            indicator.addRoundedRect(QRectF(indicatorRect(item)), indicatorRadius, indicatorRadius);
            painter.fillPath(indicator, m_spec.indicatorColor);
        }

        QColor iconColor = selected ? m_spec.selectedIconColor : m_spec.unselectedIconColor;
        QColor textColor = selected ? m_spec.selectedLabelColor : m_spec.unselectedLabelColor;
        if (!destinationEnabled) {
            iconColor = disabledColor(iconColor);
            textColor = disabledColor(textColor);
        }

        const QRect iconRect(item.center().x() - m_spec.iconSize / 2,
                             item.top() + 8,
                             m_spec.iconSize,
                             m_spec.iconSize);

        if (!m_destinations.at(i).icon.isNull()) {
            m_destinations.at(i).icon.paint(&painter,
                                            iconRect,
                                            Qt::AlignCenter,
                                            destinationEnabled ? QIcon::Normal : QIcon::Disabled);
        } else {
            painter.setPen(Qt::NoPen);
            painter.setBrush(iconColor);
            painter.drawEllipse(iconRect.adjusted(4, 4, -4, -4));
        }

        if (m_labelsVisible) {
            painter.setPen(textColor);
            painter.drawText(QRect(item.left() + 4, item.top() + 36, item.width() - 8, 22),
                             Qt::AlignCenter,
                             m_destinations.at(i).text);
        }
    }

    if (hasFocus() && m_currentIndex >= 0) {
        QPainterPath focusPath;
        focusPath.addRoundedRect(QRectF(indicatorRect(itemRect(m_currentIndex))).adjusted(-2, -2, 2, 2),
                                 indicatorRadius,
                                 indicatorRadius);
        QtMaterialFocusIndicator::paintPathFocusRing(&painter, focusPath, m_spec.focusRingColor, 2.0);
    }
}

} // namespace QtMaterial
