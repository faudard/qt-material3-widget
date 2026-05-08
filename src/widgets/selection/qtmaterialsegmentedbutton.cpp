#include "qtmaterial/widgets/selection/qtmaterialsegmentedbutton.h"
#include <memory>
#include <QVector>

#include <QAccessible>
#include <QStringList>

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/specs/qtmaterialsegmentedbuttonspec.h"

namespace QtMaterial {

struct QtMaterialSegmentedButtonPrivate
{
    struct Segment
    {
        QString text;
        QIcon icon;
        bool checked = false;
        bool enabled = true;
    };

    QVector<Segment> segments;
    int currentIndex = -1;
    bool multiSelection = false;
    mutable bool specDirty = true;
    mutable SegmentedButtonSpec spec;
};

using Segment = QtMaterialSegmentedButtonPrivate::Segment;


QtMaterialSegmentedButton::QtMaterialSegmentedButton(QWidget* parent)
    : QtMaterialControl(parent)
    , d(std::make_unique<QtMaterialSegmentedButtonPrivate>())
{
    setFocusPolicy(Qt::StrongFocus);
    setAccessibleName(tr("Segmented button"));
    syncAccessibility();
}

QtMaterialSegmentedButton::~QtMaterialSegmentedButton() = default;

int QtMaterialSegmentedButton::count() const noexcept { return d->segments.size(); }

void QtMaterialSegmentedButton::addSegment(const QString& text, const QIcon& icon)
{
    insertSegment(d->segments.size(), text, icon);
}

void QtMaterialSegmentedButton::insertSegment(int index, const QString& text, const QIcon& icon)
{
    index = qBound(0, index, d->segments.size());
    d->segments.insert(index, Segment{text, icon, false, true});
    if (d->currentIndex >= index) {
        ++d->currentIndex;
    }
    updateGeometry();
    update();
}

void QtMaterialSegmentedButton::removeSegment(int index)
{
    if (index < 0 || index >= d->segments.size()) {
        return;
    }
    d->segments.removeAt(index);
    if (d->currentIndex == index) {
        d->currentIndex = -1;
        emit currentIndexChanged(-1);
    } else if (d->currentIndex > index) {
        --d->currentIndex;
    }
    updateGeometry();
    update();
}

void QtMaterialSegmentedButton::clearSegments()
{
    if (d->segments.isEmpty()) {
        return;
    }
    d->segments.clear();
    d->currentIndex = -1;
    updateGeometry();
    update();
    emit currentIndexChanged(-1);
}

QString QtMaterialSegmentedButton::segmentText(int index) const
{
    return (index >= 0 && index < d->segments.size()) ? d->segments.at(index).text : QString();
}

void QtMaterialSegmentedButton::setSegmentText(int index, const QString& text)
{
    if (index < 0 || index >= d->segments.size() || d->segments[index].text == text) {
        return;
    }
    d->segments[index].text = text;
    updateGeometry();
    update();
}

QIcon QtMaterialSegmentedButton::segmentIcon(int index) const
{
    return (index >= 0 && index < d->segments.size()) ? d->segments.at(index).icon : QIcon();
}

void QtMaterialSegmentedButton::setSegmentIcon(int index, const QIcon& icon)
{
    if (index < 0 || index >= d->segments.size()) {
        return;
    }
    d->segments[index].icon = icon;
    updateGeometry();
    update();
}

int QtMaterialSegmentedButton::currentIndex() const noexcept { return d->currentIndex; }

void QtMaterialSegmentedButton::setCurrentIndex(int index)
{
    if (index < -1 || index >= d->segments.size() || d->currentIndex == index
        || (index >= 0 && !d->segments.at(index).enabled)) {
        return;
    }
    if (!d->multiSelection) {
        for (int i = 0; i < d->segments.size(); ++i) {
            d->segments[i].checked = (i == index);
        }
    }
    d->currentIndex = index;
    update();
    emit currentIndexChanged(index);
}

bool QtMaterialSegmentedButton::isSegmentChecked(int index) const
{
    return index >= 0 && index < d->segments.size() && d->segments.at(index).checked;
}

void QtMaterialSegmentedButton::setSegmentChecked(int index, bool checked)
{
    if (index < 0 || index >= d->segments.size() || d->segments[index].checked == checked
        || (checked && !d->segments.at(index).enabled)) {
        return;
    }
    if (!d->multiSelection && checked) {
        for (int i = 0; i < d->segments.size(); ++i) {
            d->segments[i].checked = false;
        }
        d->currentIndex = index;
        emit currentIndexChanged(index);
    }
    d->segments[index].checked = checked;
    update();
    emit segmentToggled(index, checked);
}

bool QtMaterialSegmentedButton::isMultiSelection() const noexcept { return d->multiSelection; }

void QtMaterialSegmentedButton::setMultiSelection(bool enabled)
{
    if (d->multiSelection == enabled) {
        return;
    }
    d->multiSelection = enabled;
    emit multiSelectionChanged(enabled);
}


bool QtMaterialSegmentedButton::isSegmentEnabled(int index) const
{
    return index >= 0 && index < d->segments.size() && d->segments.at(index).enabled;
}

void QtMaterialSegmentedButton::setSegmentEnabled(int index, bool enabled)
{
    if (index < 0 || index >= d->segments.size() || d->segments[index].enabled == enabled) {
        return;
    }

    d->segments[index].enabled = enabled;

    if (!enabled) {
        d->segments[index].checked = false;
        if (d->currentIndex == index) {
            const int replacement = firstEnabledIndex();
            d->currentIndex = replacement;
            if (replacement >= 0 && !d->multiSelection) {
                d->segments[replacement].checked = true;
            }
            emit currentIndexChanged(d->currentIndex);
        }
    }

    syncAccessibility();
    update();
    emit segmentEnabledChanged(index, enabled);
}

QString QtMaterialSegmentedButton::segmentAccessibleText(int index) const
{
    if (index < 0 || index >= d->segments.size()) {
        return QString();
    }

    const Segment& segment = d->segments.at(index);
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
    return segmentAccessibleText(d->currentIndex);
}

QString QtMaterialSegmentedButton::accessibilitySummary() const
{
    QStringList parts;
    parts << tr("%n segment(s)", nullptr, d->segments.size());

    if (d->currentIndex >= 0) {
        parts << tr("Current: %1").arg(currentSegmentAccessibleText());
    } else {
        parts << tr("No segment selected");
    }

    if (d->multiSelection) {
        QStringList selected;
        for (int i = 0; i < d->segments.size(); ++i) {
            if (d->segments.at(i).checked) {
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
    for (int i = 0; i < d->segments.size(); ++i) {
        if (d->segments.at(i).enabled) {
            return i;
        }
    }
    return -1;
}

int QtMaterialSegmentedButton::lastEnabledIndex() const noexcept
{
    for (int i = d->segments.size() - 1; i >= 0; --i) {
        if (d->segments.at(i).enabled) {
            return i;
        }
    }
    return -1;
}

int QtMaterialSegmentedButton::nextEnabledIndex(int start, int delta) const noexcept
{
    if (d->segments.isEmpty() || delta == 0) {
        return -1;
    }

    int index = start;
    for (int step = 0; step < d->segments.size(); ++step) {
        index = (index + delta + d->segments.size()) % d->segments.size();
        if (d->segments.at(index).enabled) {
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
    d->specDirty = true;
}

namespace {

void ensureSegmentedButtonSpecResolved(const Theme& theme,
                                       const QtMaterialSegmentedButton& self,
                                       QtMaterialSegmentedButtonPrivate& d)
{
    if (!d.specDirty) {
        return;
    }

    SpecFactory factory;
    d.spec = factory.segmentedButtonSpec(theme, self.density());
    d.specDirty = false;
}

QRect segmentedButtonSegmentRect(const QtMaterialSegmentedButton& self,
                                 const Theme& theme,
                                 QtMaterialSegmentedButtonPrivate& d,
                                 int index)
{
    ensureSegmentedButtonSpecResolved(theme, self, d);

    if (d.segments.isEmpty()) {
        return QRect();
    }

    const int segmentWidth = qMax(d.spec.minSegmentWidth, self.width() / qMax(1, d.segments.size()));
    const int totalWidth = segmentWidth * d.segments.size();
    const int startX = (self.width() - totalWidth) / 2;
    const int y = (self.height() - d.spec.segmentHeight) / 2;
    return QRect(startX + index * segmentWidth, y, segmentWidth, d.spec.segmentHeight);
}

int segmentedButtonIndexAt(const QtMaterialSegmentedButton& self,
                           const Theme& theme,
                           QtMaterialSegmentedButtonPrivate& d,
                           const QPoint& pos)
{
    for (int i = 0; i < d.segments.size(); ++i) {
        if (segmentedButtonSegmentRect(self, theme, d, i).contains(pos)) {
            return i;
        }
    }

    return -1;
}

} // namespace

void QtMaterialSegmentedButton::toggleIndex(int index)
{
    if (index < 0 || index >= d->segments.size() || !d->segments.at(index).enabled) {
        return;
    }
    if (d->multiSelection) {
        setSegmentChecked(index, !d->segments.at(index).checked);
    } else {
        setCurrentIndex(index);
        emit segmentToggled(index, true);
    }
    syncAccessibility();
}

QSize QtMaterialSegmentedButton::sizeHint() const
{
    ensureSegmentedButtonSpecResolved(theme(), *this, *d);
    int width = 0;
    QFont resolvedFont = font();
    if (theme().typography().contains(d->spec.labelTypeRole)) {
        resolvedFont = theme().typography().style(d->spec.labelTypeRole).font;
    }
    const QFontMetrics fm(resolvedFont);
    for (const Segment& segment : d->segments) {
        int itemWidth = d->spec.horizontalPadding * 2 + fm.horizontalAdvance(segment.text);
        if (!segment.icon.isNull()) {
            itemWidth += d->spec.iconSize + d->spec.iconSpacing;
        }
        width += qMax(d->spec.minSegmentWidth, itemWidth);
    }
    return QSize(qMax(width, d->spec.minSegmentWidth), d->spec.touchTarget.height());
}

QSize QtMaterialSegmentedButton::minimumSizeHint() const
{
    ensureSegmentedButtonSpecResolved(theme(), *this, *d);
    return QSize(d->spec.minSegmentWidth * qMax(1, d->segments.size()), d->spec.touchTarget.height());
}

void QtMaterialSegmentedButton::mousePressEvent(QMouseEvent* event)
{
    toggleIndex(segmentedButtonIndexAt(*this, theme(), *d, event->pos()));
    QtMaterialControl::mousePressEvent(event);
}

void QtMaterialSegmentedButton::keyPressEvent(QKeyEvent* event)
{
    if (d->segments.isEmpty()) {
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
        const int base = d->currentIndex < 0 ? firstEnabledIndex() : d->currentIndex;
        const int next = nextEnabledIndex(base, delta);
        if (next >= 0) {
            setCurrentIndex(next);
            syncAccessibility();
        }
        event->accept();
        return;
    }

    if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        const int target = d->currentIndex < 0 ? firstEnabledIndex() : d->currentIndex;
        toggleIndex(target);
        event->accept();
        return;
    }

    QtMaterialControl::keyPressEvent(event);
}

void QtMaterialSegmentedButton::paintEvent(QPaintEvent*)
{
    ensureSegmentedButtonSpecResolved(theme(), *this, *d);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QFont resolvedFont = font();
    if (theme().typography().contains(d->spec.labelTypeRole)) {
        resolvedFont = theme().typography().style(d->spec.labelTypeRole).font;
    }
    painter.setFont(resolvedFont);

    const qreal radius = theme().shapes().contains(d->spec.shapeRole)
        ? theme().shapes().radius(d->spec.shapeRole)
        : d->spec.segmentHeight / 2.0;

    for (int i = 0; i < d->segments.size(); ++i) {
        const QRect r = segmentedButtonSegmentRect(*this, theme(), *d, i);
        const bool checked = d->segments.at(i).checked;
        QPainterPath path;
        path.addRoundedRect(QRectF(r).adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);

        painter.fillPath(path, checked ? d->spec.selectedContainerColor : d->spec.containerColor);
        painter.setPen(QPen(isEnabled() ? d->spec.outlineColor : d->spec.disabledOutlineColor, d->spec.outlineWidth));
        painter.drawPath(path);

        const QColor textColor = !isEnabled()
            ? d->spec.disabledLabelColor
            : (checked ? d->spec.selectedLabelColor : d->spec.labelColor);
        painter.setPen(textColor);
        painter.drawText(r.adjusted(d->spec.horizontalPadding, 0, -d->spec.horizontalPadding, 0),
                         Qt::AlignCenter, d->segments.at(i).text);
    }

    if (hasFocus() && d->currentIndex >= 0) {
        QPainterPath focusPath;
        focusPath.addRoundedRect(QRectF(segmentedButtonSegmentRect(*this, theme(), *d, d->currentIndex)).adjusted(1, 1, -1, -1), radius, radius);
        QtMaterialFocusIndicator::paintPathFocusRing(&painter, focusPath, d->spec.focusRingColor, 2.0);
    }
}

} // namespace QtMaterial
