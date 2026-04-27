#include "qtmaterial/widgets/selection/qtmaterialsegmentedbutton.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {

QtMaterialSegmentedButton::QtMaterialSegmentedButton(QWidget* parent)
    : QtMaterialControl(parent)
{
    setFocusPolicy(Qt::StrongFocus);
}

QtMaterialSegmentedButton::~QtMaterialSegmentedButton() = default;

int QtMaterialSegmentedButton::count() const noexcept { return m_segments.size(); }

void QtMaterialSegmentedButton::addSegment(const QString& text, const QIcon& icon)
{
    insertSegment(m_segments.size(), text, icon);
}

void QtMaterialSegmentedButton::insertSegment(int index, const QString& text, const QIcon& icon)
{
    index = qBound(0, index, m_segments.size());
    m_segments.insert(index, Segment{text, icon, false});
    if (m_currentIndex >= index) {
        ++m_currentIndex;
    }
    updateGeometry();
    update();
}

void QtMaterialSegmentedButton::removeSegment(int index)
{
    if (index < 0 || index >= m_segments.size()) {
        return;
    }
    m_segments.removeAt(index);
    if (m_currentIndex == index) {
        m_currentIndex = -1;
        emit currentIndexChanged(-1);
    } else if (m_currentIndex > index) {
        --m_currentIndex;
    }
    updateGeometry();
    update();
}

void QtMaterialSegmentedButton::clearSegments()
{
    if (m_segments.isEmpty()) {
        return;
    }
    m_segments.clear();
    m_currentIndex = -1;
    updateGeometry();
    update();
    emit currentIndexChanged(-1);
}

QString QtMaterialSegmentedButton::segmentText(int index) const
{
    return (index >= 0 && index < m_segments.size()) ? m_segments.at(index).text : QString();
}

void QtMaterialSegmentedButton::setSegmentText(int index, const QString& text)
{
    if (index < 0 || index >= m_segments.size() || m_segments[index].text == text) {
        return;
    }
    m_segments[index].text = text;
    updateGeometry();
    update();
}

QIcon QtMaterialSegmentedButton::segmentIcon(int index) const
{
    return (index >= 0 && index < m_segments.size()) ? m_segments.at(index).icon : QIcon();
}

void QtMaterialSegmentedButton::setSegmentIcon(int index, const QIcon& icon)
{
    if (index < 0 || index >= m_segments.size()) {
        return;
    }
    m_segments[index].icon = icon;
    updateGeometry();
    update();
}

int QtMaterialSegmentedButton::currentIndex() const noexcept { return m_currentIndex; }

void QtMaterialSegmentedButton::setCurrentIndex(int index)
{
    if (index < -1 || index >= m_segments.size() || m_currentIndex == index) {
        return;
    }
    if (!m_multiSelection) {
        for (int i = 0; i < m_segments.size(); ++i) {
            m_segments[i].checked = (i == index);
        }
    }
    m_currentIndex = index;
    update();
    emit currentIndexChanged(index);
}

bool QtMaterialSegmentedButton::isSegmentChecked(int index) const
{
    return index >= 0 && index < m_segments.size() && m_segments.at(index).checked;
}

void QtMaterialSegmentedButton::setSegmentChecked(int index, bool checked)
{
    if (index < 0 || index >= m_segments.size() || m_segments[index].checked == checked) {
        return;
    }
    if (!m_multiSelection && checked) {
        for (int i = 0; i < m_segments.size(); ++i) {
            m_segments[i].checked = false;
        }
        m_currentIndex = index;
        emit currentIndexChanged(index);
    }
    m_segments[index].checked = checked;
    update();
    emit segmentToggled(index, checked);
}

bool QtMaterialSegmentedButton::isMultiSelection() const noexcept { return m_multiSelection; }

void QtMaterialSegmentedButton::setMultiSelection(bool enabled)
{
    if (m_multiSelection == enabled) {
        return;
    }
    m_multiSelection = enabled;
    emit multiSelectionChanged(enabled);
}

void QtMaterialSegmentedButton::themeChangedEvent(const Theme& theme)
{
    QtMaterialControl::themeChangedEvent(theme);
    invalidateResolvedSpec();
}

void QtMaterialSegmentedButton::invalidateResolvedSpec()
{
    m_specDirty = true;
}

void QtMaterialSegmentedButton::ensureSpecResolved() const
{
    if (!m_specDirty) {
        return;
    }
    SpecFactory factory;
    m_spec = factory.segmentedButtonSpec(theme(), density());
    m_specDirty = false;
}

QRect QtMaterialSegmentedButton::segmentRect(int index) const
{
    ensureSpecResolved();
    if (m_segments.isEmpty()) {
        return QRect();
    }
    const int segmentWidth = qMax(m_spec.minSegmentWidth, width() / qMax(1, m_segments.size()));
    const int totalWidth = segmentWidth * m_segments.size();
    const int startX = (width() - totalWidth) / 2;
    const int y = (height() - m_spec.segmentHeight) / 2;
    return QRect(startX + index * segmentWidth, y, segmentWidth, m_spec.segmentHeight);
}

int QtMaterialSegmentedButton::indexAt(const QPoint& pos) const
{
    for (int i = 0; i < m_segments.size(); ++i) {
        if (segmentRect(i).contains(pos)) {
            return i;
        }
    }
    return -1;
}

void QtMaterialSegmentedButton::toggleIndex(int index)
{
    if (index < 0 || index >= m_segments.size()) {
        return;
    }
    if (m_multiSelection) {
        setSegmentChecked(index, !m_segments.at(index).checked);
    } else {
        setCurrentIndex(index);
        emit segmentToggled(index, true);
    }
}

QSize QtMaterialSegmentedButton::sizeHint() const
{
    ensureSpecResolved();
    int width = 0;
    QFont resolvedFont = font();
    if (theme().typography().contains(m_spec.labelTypeRole)) {
        resolvedFont = theme().typography().style(m_spec.labelTypeRole).font;
    }
    const QFontMetrics fm(resolvedFont);
    for (const Segment& segment : m_segments) {
        int itemWidth = m_spec.horizontalPadding * 2 + fm.horizontalAdvance(segment.text);
        if (!segment.icon.isNull()) {
            itemWidth += m_spec.iconSize + m_spec.iconSpacing;
        }
        width += qMax(m_spec.minSegmentWidth, itemWidth);
    }
    return QSize(qMax(width, m_spec.minSegmentWidth), m_spec.touchTarget.height());
}

QSize QtMaterialSegmentedButton::minimumSizeHint() const
{
    ensureSpecResolved();
    return QSize(m_spec.minSegmentWidth * qMax(1, m_segments.size()), m_spec.touchTarget.height());
}

void QtMaterialSegmentedButton::mousePressEvent(QMouseEvent* event)
{
    toggleIndex(indexAt(event->pos()));
    QtMaterialControl::mousePressEvent(event);
}

void QtMaterialSegmentedButton::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Right) {
        if (!m_segments.isEmpty()) {
            const int delta = event->key() == Qt::Key_Right ? 1 : -1;
            const int base = m_currentIndex < 0 ? 0 : m_currentIndex;
            setCurrentIndex((base + delta + m_segments.size()) % m_segments.size());
        }
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        toggleIndex(m_currentIndex < 0 ? 0 : m_currentIndex);
        event->accept();
        return;
    }
    QtMaterialControl::keyPressEvent(event);
}

void QtMaterialSegmentedButton::paintEvent(QPaintEvent*)
{
    ensureSpecResolved();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QFont resolvedFont = font();
    if (theme().typography().contains(m_spec.labelTypeRole)) {
        resolvedFont = theme().typography().style(m_spec.labelTypeRole).font;
    }
    painter.setFont(resolvedFont);

    const qreal radius = theme().shapes().contains(m_spec.shapeRole)
        ? theme().shapes().radius(m_spec.shapeRole)
        : m_spec.segmentHeight / 2.0;

    for (int i = 0; i < m_segments.size(); ++i) {
        const QRect r = segmentRect(i);
        const bool checked = m_segments.at(i).checked;
        QPainterPath path;
        path.addRoundedRect(QRectF(r).adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);

        painter.fillPath(path, checked ? m_spec.selectedContainerColor : m_spec.containerColor);
        painter.setPen(QPen(isEnabled() ? m_spec.outlineColor : m_spec.disabledOutlineColor, m_spec.outlineWidth));
        painter.drawPath(path);

        const QColor textColor = !isEnabled()
            ? m_spec.disabledLabelColor
            : (checked ? m_spec.selectedLabelColor : m_spec.labelColor);
        painter.setPen(textColor);
        painter.drawText(r.adjusted(m_spec.horizontalPadding, 0, -m_spec.horizontalPadding, 0),
                         Qt::AlignCenter, m_segments.at(i).text);
    }

    if (hasFocus() && m_currentIndex >= 0) {
        QPainterPath focusPath;
        focusPath.addRoundedRect(QRectF(segmentRect(m_currentIndex)).adjusted(1, 1, -1, -1), radius, radius);
        QtMaterialFocusIndicator::paintPathFocusRing(&painter, focusPath, m_spec.focusRingColor, 2.0);
    }
}

} // namespace QtMaterial
