#include "qtmaterial/widgets/navigation/qtmaterialnavigationrail.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {

QtMaterialNavigationRail::QtMaterialNavigationRail(QWidget* parent)
    : QtMaterialControl(parent)
{
    setFocusPolicy(Qt::StrongFocus);
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
    m_destinations.insert(index, Destination{text, icon});
    if (m_currentIndex >= index) {
        ++m_currentIndex;
    }
    updateGeometry();
    update();
}

void QtMaterialNavigationRail::removeDestination(int index)
{
    if (index < 0 || index >= m_destinations.size()) {
        return;
    }
    m_destinations.removeAt(index);
    if (m_currentIndex == index) {
        m_currentIndex = -1;
        emit currentIndexChanged(-1);
    } else if (m_currentIndex > index) {
        --m_currentIndex;
    }
    updateGeometry();
    update();
}

void QtMaterialNavigationRail::clearDestinations()
{
    m_destinations.clear();
    m_currentIndex = -1;
    updateGeometry();
    update();
    emit currentIndexChanged(-1);
}

int QtMaterialNavigationRail::count() const noexcept { return m_destinations.size(); }

QString QtMaterialNavigationRail::destinationText(int index) const
{
    return index >= 0 && index < m_destinations.size() ? m_destinations.at(index).text : QString();
}

QIcon QtMaterialNavigationRail::destinationIcon(int index) const
{
    return index >= 0 && index < m_destinations.size() ? m_destinations.at(index).icon : QIcon();
}

int QtMaterialNavigationRail::currentIndex() const noexcept { return m_currentIndex; }

void QtMaterialNavigationRail::setCurrentIndex(int index)
{
    if (index < -1 || index >= m_destinations.size() || m_currentIndex == index) {
        return;
    }
    m_currentIndex = index;
    update();
    emit currentIndexChanged(index);
}

bool QtMaterialNavigationRail::labelsVisible() const noexcept { return m_labelsVisible; }

void QtMaterialNavigationRail::setLabelsVisible(bool visible)
{
    if (m_labelsVisible == visible) {
        return;
    }
    m_labelsVisible = visible;
    updateGeometry();
    update();
    emit labelsVisibleChanged(visible);
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
    if (index >= 0) {
        setCurrentIndex(index);
        emit destinationActivated(index);
        event->accept();
        return;
    }
    QtMaterialControl::mousePressEvent(event);
}

void QtMaterialNavigationRail::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down) {
        if (!m_destinations.isEmpty()) {
            const int delta = event->key() == Qt::Key_Down ? 1 : -1;
            const int base = m_currentIndex < 0 ? 0 : m_currentIndex;
            setCurrentIndex((base + delta + m_destinations.size()) % m_destinations.size());
        }
        event->accept();
        return;
    }
    if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter || event->key() == Qt::Key_Space) && m_currentIndex >= 0) {
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
        if (selected) {
            QPainterPath indicator;
            indicator.addRoundedRect(QRectF(indicatorRect(item)), indicatorRadius, indicatorRadius);
            painter.fillPath(indicator, m_spec.indicatorColor);
        }

        const QColor iconColor = selected ? m_spec.selectedIconColor : m_spec.unselectedIconColor;
        const QColor textColor = selected ? m_spec.selectedLabelColor : m_spec.unselectedLabelColor;
        const QRect iconRect(item.center().x() - m_spec.iconSize / 2,
                             item.top() + 8,
                             m_spec.iconSize,
                             m_spec.iconSize);
        if (!m_destinations.at(i).icon.isNull()) {
            m_destinations.at(i).icon.paint(&painter, iconRect, Qt::AlignCenter, isEnabled() ? QIcon::Normal : QIcon::Disabled);
        } else {
            painter.setPen(Qt::NoPen);
            painter.setBrush(iconColor);
            painter.drawEllipse(iconRect.adjusted(4, 4, -4, -4));
        }
        if (m_labelsVisible) {
            painter.setPen(textColor);
            painter.drawText(QRect(item.left() + 4, item.top() + 36, item.width() - 8, 22), Qt::AlignCenter, m_destinations.at(i).text);
        }
    }

    if (hasFocus() && m_currentIndex >= 0) {
        QPainterPath focusPath;
        focusPath.addRoundedRect(QRectF(indicatorRect(itemRect(m_currentIndex))).adjusted(-2, -2, 2, 2), indicatorRadius, indicatorRadius);
        QtMaterialFocusIndicator::paintPathFocusRing(&painter, focusPath, m_spec.focusRingColor, 2.0);
    }
}

} // namespace QtMaterial
