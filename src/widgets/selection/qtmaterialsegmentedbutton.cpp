#include "qtmaterial/widgets/selection/qtmaterialsegmentedbutton.h"

#include <QAccessible>
#include <QStringList>

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
    setAccessibleName(tr("Segmented button"));
    syncAccessibility();
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
    m_segments.insert(index, Segment{text, icon, false, true});
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
    if (index < -1 || index >= m_segments.size() || m_currentIndex == index
        || (index >= 0 && !m_segments.at(index).enabled)) {
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
    if (index < 0 || index >= m_segments.size() || m_segments[index].checked == checked
        || (checked && !m_segments.at(index).enabled)) {
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


bool QtMaterialSegmentedButton::isSegmentEnabled(int index) const
{
    return index >= 0 && index < m_segments.size() && m_segments.at(index).enabled;
}

void QtMaterialSegmentedButton::setSegmentEnabled(int index, bool enabled)
{
    if (index < 0 || index >= m_segments.size() || m_segments[index].enabled == enabled) {
        return;
    }

    m_segments[index].enabled = enabled;

    if (!enabled) {
        m_segments[index].checked = false;
        if (m_currentIndex == index) {
            const int replacement = firstEnabledIndex();
            m_currentIndex = replacement;
            if (replacement >= 0 && !m_multiSelection) {
                m_segments[replacement].checked = true;
            }
            emit currentIndexChanged(m_currentIndex);
        }
    }

    syncAccessibility();
    update();
    emit segmentEnabledChanged(index, enabled);
}

QString QtMaterialSegmentedButton::segmentAccessibleText(int index) const
{
    if (index < 0 || index >= m_segments.size()) {
        return QString();
    }

    const Segment& segment = m_segments.at(index);
    QStringList parts;
    parts << (segment.text.isEmpty() ? tr("Segment %1").arg(index + 1) : segment.text);

    if (segment.checked) {
        parts << tr("selected");
    }
    if (!segment.enabled) {
        parts << tr("disabled");
    }

    return parts.join(QStringLiteral(", "));
}

QString QtMaterialSegmentedButton::currentSegmentAccessibleText() const
{
    return segmentAccessibleText(m_currentIndex);
}

QString QtMaterialSegmentedButton::accessibilitySummary() const
{
    QStringList parts;
    parts << tr("%n segment(s)", nullptr, m_segments.size());

    if (m_currentIndex >= 0) {
        parts << tr("Current: %1").arg(currentSegmentAccessibleText());
    } else {
        parts << tr("No segment selected");
    }

    if (m_multiSelection) {
        QStringList selected;
        for (int i = 0; i < m_segments.size(); ++i) {
            if (m_segments.at(i).checked) {
                selected << segmentAccessibleText(i);
            }
        }
        if (!selected.isEmpty()) {
            parts << tr("Selected: %1").arg(selected.join(QStringLiteral("; ")));
        }
    }

    return parts.join(QStringLiteral(". "));
}

void QtMaterialSegmentedButton::syncAccessibility()
{
    const QString summary = accessibilitySummary();
    setAccessibleDescription(summary);
    QAccessibleEvent event(this, QAccessible::DescriptionChanged);
    QAccessible::updateAccessibility(&event);
    emit accessibilitySummaryChanged(summary);
}

int QtMaterialSegmentedButton::firstEnabledIndex() const noexcept
{
    for (int i = 0; i < m_segments.size(); ++i) {
        if (m_segments.at(i).enabled) {
            return i;
        }
    }
    return -1;
}

int QtMaterialSegmentedButton::lastEnabledIndex() const noexcept
{
    for (int i = m_segments.size() - 1; i >= 0; --i) {
        if (m_segments.at(i).enabled) {
            return i;
        }
    }
    return -1;
}

int QtMaterialSegmentedButton::nextEnabledIndex(int start, int delta) const noexcept
{
    if (m_segments.isEmpty() || delta == 0) {
        return -1;
    }

    int index = start;
    for (int step = 0; step < m_segments.size(); ++step) {
        index = (index + delta + m_segments.size()) % m_segments.size();
        if (m_segments.at(index).enabled) {
            return index;
        }
    }

    return -1;
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
    if (index < 0 || index >= m_segments.size() || !m_segments.at(index).enabled) {
        return;
    }
    if (m_multiSelection) {
        setSegmentChecked(index, !m_segments.at(index).checked);
    } else {
        setCurrentIndex(index);
        emit segmentToggled(index, true);
    }
    syncAccessibility();
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
    if (m_segments.isEmpty()) {
        QtMaterialControl::keyPressEvent(event);
        return;
    }

    if (event->key() == Qt::Key_Home) {
        setCurrentIndex(firstEnabledIndex());
        syncAccessibility();
        event->accept();
        return;
    }

    if (event->key() == Qt::Key_End) {
        setCurrentIndex(lastEnabledIndex());
        syncAccessibility();
        event->accept();
        return;
    }

    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Right) {
        const bool rtl = layoutDirection() == Qt::RightToLeft;
        const bool forwardKey = event->key() == Qt::Key_Right;
        const int delta = (forwardKey ^ rtl) ? 1 : -1;
        const int base = m_currentIndex < 0 ? firstEnabledIndex() : m_currentIndex;
        const int next = nextEnabledIndex(base, delta);
        if (next >= 0) {
            setCurrentIndex(next);
            syncAccessibility();
        }
        event->accept();
        return;
    }

    if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        const int target = m_currentIndex < 0 ? firstEnabledIndex() : m_currentIndex;
        toggleIndex(target);
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
