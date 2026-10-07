#include "qtmaterial/widgets/navigation/qtmaterialnavigationbar.h"

#include <QEvent>
#include <QFocusEvent>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QStyle>
#include <QStringList>
#include <QVector>

#include "../resolution/qtmaterialmissingmaterial3specresolution_p.h"
#include "qtmaterialitemaccessibility_p.h"

namespace QtMaterial {

namespace {

constexpr int kPreferredHeight = 80;
constexpr int kMinimumHeight = 64;
constexpr int kMinimumWidth = 240;
constexpr int kIndicatorHeight = 32;
constexpr int kIndicatorWidth = 64;
constexpr int kIconSize = 24;

QPixmap tintedIcon(
    const QIcon& icon,
    const QSize& size,
    const QColor& color,
    bool enabled)
{
    if (icon.isNull()) {
        return {};
    }

    QPixmap pixmap = icon.pixmap(
        size,
        enabled ? QIcon::Normal : QIcon::Disabled);
    if (pixmap.isNull()) {
        return {};
    }

    QPainter painter(&pixmap);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(pixmap.rect(), color);
    return pixmap;
}

} // namespace

class QtMaterialNavigationBarPrivate
{
public:
    struct Destination {
        QString text;
        QIcon icon;
        bool enabled = true;
    };

    QVector<Destination> destinations;
    int currentIndex = -1;
    int pendingCurrentIndex = -1;
    int hoveredIndex = -1;
    int pressedIndex = -1;
    bool labelsVisible = true;
    QString lastAccessibilitySummary;

    QRect itemRect(const QtMaterialNavigationBar* bar, int index) const
    {
        if (!bar || destinations.isEmpty() || index < 0 || index >= destinations.size()) {
            return {};
        }

        const int itemWidth = qMax(1, bar->width() / destinations.size());
        const int left = index * itemWidth;
        const int width = index == destinations.size() - 1
            ? bar->width() - left
            : itemWidth;
        const QRect logical(left, 0, width, bar->height());
        return QStyle::visualRect(bar->layoutDirection(), bar->rect(), logical);
    }

    int indexAt(const QtMaterialNavigationBar* bar, const QPoint& pos) const
    {
        for (int i = 0; i < destinations.size(); ++i) {
            if (itemRect(bar, i).contains(pos)) {
                return i;
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

    int nextEnabledIndex(int from, int step) const noexcept
    {
        if (destinations.isEmpty() || step == 0) {
            return -1;
        }

        int index = from;
        if (index < 0 || index >= destinations.size()) {
            index = step > 0 ? -1 : destinations.size();
        }

        for (int tries = 0; tries < destinations.size(); ++tries) {
            index = (index + step + destinations.size()) % destinations.size();
            if (destinations.at(index).enabled) {
                return index;
            }
        }
        return -1;
    }

    int firstEnabled() const noexcept
    {
        for (int i = 0; i < destinations.size(); ++i) {
            if (destinations.at(i).enabled) {
                return i;
            }
        }
        return -1;
    }

    int lastEnabled() const noexcept
    {
        for (int i = destinations.size() - 1; i >= 0; --i) {
            if (destinations.at(i).enabled) {
                return i;
            }
        }
        return -1;
    }
};

QtMaterialNavigationBar::QtMaterialNavigationBar(QWidget* parent)
    : QtMaterialControl(parent)
    , d_ptr(std::make_unique<QtMaterialNavigationBarPrivate>())
{
#ifndef QT_NO_ACCESSIBILITY
    static const bool accessibilityInstalled = []() {
        QAccessible::installFactory([](const QString&, QObject* object) -> QAccessibleInterface* {
            auto* bar = qobject_cast<QtMaterialNavigationBar*>(object);
            if (!bar) {
                return nullptr;
            }

            QtMaterialItemAccessibility::ItemAccess access;
            access.count = [bar]() { return bar->count(); };
            access.current = [bar]() { return bar->currentIndex(); };
            access.text = [bar](int index, QAccessible::Text type) {
                if (type == QAccessible::Name) {
                    return bar->destinationText(index);
                }
                return type == QAccessible::Description
                    ? bar->destinationAccessibleText(index)
                    : QString();
            };
            access.rect = [bar](int index) {
                return bar->d_ptr->itemRect(bar, index);
            };
            access.role = [](int) {
                return QAccessible::ListItem;
            };
            access.state = [bar](int index) {
                QAccessible::State state;
                state.disabled = !bar->isDestinationEnabled(index);
                state.focusable = true;
                state.selectable = true;
                state.selected = bar->currentIndex() == index;
                return state;
            };
            access.select = [bar](int index) {
                bar->setCurrentIndex(index);
            };
            return new QtMaterialItemAccessibility::ItemWidgetInterface(
                bar,
                QAccessible::List,
                std::move(access));
        });
        return true;
    }();
    Q_UNUSED(accessibilityInstalled);
#endif

    setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setAccessibleName(tr("Navigation bar"));
    syncAccessibility();
}

QtMaterialNavigationBar::~QtMaterialNavigationBar() = default;

int QtMaterialNavigationBar::addDestination(const QString& text, const QIcon& icon)
{
    insertDestination(d_ptr->destinations.size(), text, icon);
    return d_ptr->destinations.size() - 1;
}

void QtMaterialNavigationBar::insertDestination(int index, const QString& text, const QIcon& icon)
{
    index = qBound(0, index, d_ptr->destinations.size());
    d_ptr->destinations.insert(index, {text, icon, true});

    if (d_ptr->currentIndex >= index) {
        ++d_ptr->currentIndex;
    }
    if (d_ptr->hoveredIndex >= index) {
        ++d_ptr->hoveredIndex;
    }
    if (d_ptr->pressedIndex >= index) {
        ++d_ptr->pressedIndex;
    }

#ifndef QT_NO_ACCESSIBILITY
    QtMaterialItemAccessibility::notifyStructure(this);
#endif
    updateGeometry();
    update();
    syncAccessibility();
    Q_EMIT destinationLabelsChanged(destinationLabels());
}

void QtMaterialNavigationBar::removeDestination(int index)
{
    if (index < 0 || index >= d_ptr->destinations.size()) {
        return;
    }

    const bool removedCurrent = d_ptr->currentIndex == index;
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
        nextCurrent = d_ptr->destinations.isEmpty()
            ? -1
            : qMin(index, d_ptr->destinations.size() - 1);
        while (nextCurrent >= 0 && !d_ptr->enabledIndex(nextCurrent)) {
            --nextCurrent;
        }
        if (nextCurrent < 0) {
            nextCurrent = d_ptr->firstEnabled();
        }
    } else if (nextCurrent > index) {
        --nextCurrent;
    }

    if (d_ptr->currentIndex != nextCurrent) {
        d_ptr->currentIndex = nextCurrent;
        Q_EMIT currentIndexChanged(nextCurrent);
    }

#ifndef QT_NO_ACCESSIBILITY
    QtMaterialItemAccessibility::notifyStructure(this);
#endif
    updateGeometry();
    update();
    syncAccessibility();
    Q_EMIT destinationLabelsChanged(destinationLabels());
}

void QtMaterialNavigationBar::clearDestinations()
{
    const bool hadDestinations = !d_ptr->destinations.isEmpty();
    if (!hadDestinations && d_ptr->currentIndex == -1) {
        return;
    }

    d_ptr->destinations.clear();
    d_ptr->hoveredIndex = -1;
    d_ptr->pressedIndex = -1;
    const bool changed = d_ptr->currentIndex != -1;
    d_ptr->currentIndex = -1;
    if (changed) {
        Q_EMIT currentIndexChanged(-1);
    }
#ifndef QT_NO_ACCESSIBILITY
    QtMaterialItemAccessibility::notifyStructure(this);
#endif
    updateGeometry();
    update();
    syncAccessibility();
    if (hadDestinations) {
        Q_EMIT destinationLabelsChanged(destinationLabels());
    }
}

QStringList QtMaterialNavigationBar::destinationLabels() const
{
    QStringList labels;
    labels.reserve(d_ptr->destinations.size());
    for (const auto& destination : d_ptr->destinations) {
        labels.append(destination.text);
    }
    return labels;
}

void QtMaterialNavigationBar::setDestinationLabels(const QStringList& labels)
{
    if (destinationLabels() == labels) {
        return;
    }

    const int previousIndex = d_ptr->currentIndex;
    const int requestedIndex =
        d_ptr->pendingCurrentIndex >= 0
            ? d_ptr->pendingCurrentIndex
            : previousIndex;
    d_ptr->destinations.clear();
    d_ptr->hoveredIndex = -1;
    d_ptr->pressedIndex = -1;
    for (const QString& label : labels) {
        d_ptr->destinations.append({label, QIcon(), true});
    }
    const int nextIndex = labels.isEmpty()
        ? -1
        : qBound(0, requestedIndex < 0 ? 0 : requestedIndex, int(labels.size()) - 1);
    if (!labels.isEmpty()) {
        d_ptr->pendingCurrentIndex = -1;
    }
    if (d_ptr->currentIndex != nextIndex) {
        d_ptr->currentIndex = nextIndex;
        Q_EMIT currentIndexChanged(nextIndex);
    }
#ifndef QT_NO_ACCESSIBILITY
    QtMaterialItemAccessibility::notifyStructure(this);
#endif
    updateGeometry();
    update();
    syncAccessibility();
    Q_EMIT destinationLabelsChanged(destinationLabels());
}

int QtMaterialNavigationBar::count() const noexcept
{
    return d_ptr->destinations.size();
}

QString QtMaterialNavigationBar::destinationText(int index) const
{
    return index >= 0 && index < d_ptr->destinations.size()
        ? d_ptr->destinations.at(index).text
        : QString();
}

QIcon QtMaterialNavigationBar::destinationIcon(int index) const
{
    return index >= 0 && index < d_ptr->destinations.size()
        ? d_ptr->destinations.at(index).icon
        : QIcon();
}

bool QtMaterialNavigationBar::isDestinationEnabled(int index) const noexcept
{
    return d_ptr->enabledIndex(index);
}

QString QtMaterialNavigationBar::destinationAccessibleText(int index) const
{
    if (index < 0 || index >= d_ptr->destinations.size()) {
        return {};
    }

    const auto& destination = d_ptr->destinations.at(index);
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

void QtMaterialNavigationBar::setDestinationEnabled(int index, bool enabled)
{
    if (index < 0 || index >= d_ptr->destinations.size()
        || d_ptr->destinations[index].enabled == enabled) {
        return;
    }

    d_ptr->destinations[index].enabled = enabled;
    if (!enabled && d_ptr->currentIndex == index) {
        const int replacement = d_ptr->nextEnabledIndex(index, 1);
        d_ptr->currentIndex = replacement;
        Q_EMIT currentIndexChanged(replacement);
    }

    Q_EMIT destinationEnabledChanged(index, enabled);
#ifndef QT_NO_ACCESSIBILITY
    QtMaterialItemAccessibility::notifyItems(this);
#endif
    update();
    syncAccessibility();
}

int QtMaterialNavigationBar::currentIndex() const noexcept
{
    return d_ptr->currentIndex;
}

void QtMaterialNavigationBar::setCurrentIndex(int index)
{
    if (d_ptr->destinations.isEmpty() && index >= 0) {
        d_ptr->pendingCurrentIndex = index;
        return;
    }

    if (index == -1) {
        d_ptr->pendingCurrentIndex = -1;
        if (d_ptr->currentIndex == -1) {
            return;
        }
        d_ptr->currentIndex = -1;
        Q_EMIT currentIndexChanged(-1);
        update();
        syncAccessibility();
        return;
    }

    if (!d_ptr->enabledIndex(index) || d_ptr->currentIndex == index) {
        return;
    }

    d_ptr->pendingCurrentIndex = -1;
    d_ptr->currentIndex = index;
    Q_EMIT currentIndexChanged(index);
#ifndef QT_NO_ACCESSIBILITY
    QtMaterialItemAccessibility::notifyItems(this);
#endif
    update();
    syncAccessibility();
}

bool QtMaterialNavigationBar::labelsVisible() const noexcept
{
    return d_ptr->labelsVisible;
}

void QtMaterialNavigationBar::setLabelsVisible(bool visible)
{
    if (d_ptr->labelsVisible == visible) {
        return;
    }

    d_ptr->labelsVisible = visible;
    Q_EMIT labelsVisibleChanged(visible);
    updateGeometry();
    update();
}

QString QtMaterialNavigationBar::accessibilitySummary() const
{
    if (d_ptr->destinations.isEmpty()) {
        return tr("Navigation bar, no destinations");
    }

    QString selected;
    if (d_ptr->currentIndex >= 0) {
        selected = d_ptr->destinations.at(d_ptr->currentIndex).text;
    }
    return selected.isEmpty()
        ? tr("Navigation bar, %1 destinations").arg(d_ptr->destinations.size())
        : tr("Navigation bar, %1 destinations, selected %2")
              .arg(d_ptr->destinations.size())
              .arg(selected);
}

QSize QtMaterialNavigationBar::sizeHint() const
{
    const int width = qMax(kMinimumWidth, d_ptr->destinations.size() * 96);
    return QSize(width, d_ptr->labelsVisible ? kPreferredHeight : kMinimumHeight);
}

QSize QtMaterialNavigationBar::minimumSizeHint() const
{
    return QSize(kMinimumWidth, kMinimumHeight);
}

void QtMaterialNavigationBar::mouseMoveEvent(QMouseEvent* event)
{
    const int hovered = d_ptr->indexAt(this, event->pos());
    if (hovered != d_ptr->hoveredIndex) {
        d_ptr->hoveredIndex = hovered;
        update();
    }
    QtMaterialControl::mouseMoveEvent(event);
}

void QtMaterialNavigationBar::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        const int index = d_ptr->indexAt(this, event->pos());
        if (d_ptr->enabledIndex(index)) {
            d_ptr->pressedIndex = index;
            update();
            event->accept();
            return;
        }
    }
    QtMaterialControl::mousePressEvent(event);
}

void QtMaterialNavigationBar::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && d_ptr->pressedIndex >= 0) {
        const int pressed = d_ptr->pressedIndex;
        d_ptr->pressedIndex = -1;
        const int released = d_ptr->indexAt(this, event->pos());
        update();

        if (released == pressed && d_ptr->enabledIndex(pressed)) {
            setCurrentIndex(pressed);
            Q_EMIT destinationActivated(pressed);
        }
        event->accept();
        return;
    }
    QtMaterialControl::mouseReleaseEvent(event);
}

void QtMaterialNavigationBar::leaveEvent(QEvent* event)
{
    d_ptr->hoveredIndex = -1;
    d_ptr->pressedIndex = -1;
    update();
    QtMaterialControl::leaveEvent(event);
}

void QtMaterialNavigationBar::focusInEvent(QFocusEvent* event)
{
    QtMaterialControl::focusInEvent(event);
    syncAccessibility();
    update();
}

void QtMaterialNavigationBar::focusOutEvent(QFocusEvent* event)
{
    QtMaterialControl::focusOutEvent(event);
    syncAccessibility();
    update();
}

void QtMaterialNavigationBar::keyPressEvent(QKeyEvent* event)
{
    int next = -1;
    const int key = event->key();

    if (key == Qt::Key_Home) {
        next = d_ptr->firstEnabled();
    } else if (key == Qt::Key_End) {
        next = d_ptr->lastEnabled();
    } else if (key == Qt::Key_Left || key == Qt::Key_Right) {
        int step = key == Qt::Key_Right ? 1 : -1;
        if (layoutDirection() == Qt::RightToLeft) {
            step = -step;
        }
        next = d_ptr->nextEnabledIndex(d_ptr->currentIndex, step);
    } else if (key == Qt::Key_Space
               || key == Qt::Key_Return
               || key == Qt::Key_Enter) {
        if (d_ptr->enabledIndex(d_ptr->currentIndex)) {
            Q_EMIT destinationActivated(d_ptr->currentIndex);
            event->accept();
            return;
        }
    }

    if (next >= 0) {
        setCurrentIndex(next);
        event->accept();
        return;
    }

    QtMaterialControl::keyPressEvent(event);
}

void QtMaterialNavigationBar::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const auto spec =
        MissingMaterial3SpecResolution::navigationBarSpec(
            theme(),
            palette());

    painter.fillRect(rect(), spec.containerColor);

    const QFontMetrics fm(font());
    for (int i = 0; i < d_ptr->destinations.size(); ++i) {
        const auto& destination = d_ptr->destinations.at(i);
        const QRect item = d_ptr->itemRect(this, i);
        const bool selected = i == d_ptr->currentIndex;
        const bool hovered = i == d_ptr->hoveredIndex;
        const bool pressed = i == d_ptr->pressedIndex;

        QRect indicator(
            item.center().x() - qMin(kIndicatorWidth, item.width() - 16) / 2,
            item.top() + 12,
            qMin(kIndicatorWidth, item.width() - 16),
            kIndicatorHeight);

        if (!d_ptr->labelsVisible) {
            indicator.moveCenter(item.center());
        }

        if (selected) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(spec.activeIndicatorColor);
            painter.drawRoundedRect(indicator, indicator.height() / 2.0, indicator.height() / 2.0);
        }

        if (hovered || pressed) {
            QColor state = selected ? spec.activeIconColor : spec.inactiveColor;
            state.setAlphaF(pressed ? 0.12 : 0.08);
            painter.setPen(Qt::NoPen);
            painter.setBrush(state);
            painter.drawRoundedRect(indicator, indicator.height() / 2.0, indicator.height() / 2.0);
        }

        QColor contentColor = selected ? spec.activeIconColor : spec.inactiveColor;
        if (!destination.enabled) {
            contentColor.setAlphaF(0.38);
        }

        if (!destination.icon.isNull()) {
            const QPixmap icon = tintedIcon(
                destination.icon,
                QSize(kIconSize, kIconSize),
                contentColor,
                destination.enabled);
            const QPoint iconTopLeft(
                indicator.center().x() - kIconSize / 2,
                indicator.center().y() - kIconSize / 2);
            painter.drawPixmap(iconTopLeft, icon);
        }

        if (d_ptr->labelsVisible) {
            painter.setPen(selected ? spec.activeLabelColor : spec.inactiveColor);
            if (!destination.enabled) {
                QColor disabled = painter.pen().color();
                disabled.setAlphaF(0.38);
                painter.setPen(disabled);
            }
            const QRect labelRect(
                item.left() + 6,
                indicator.bottom() + 4,
                item.width() - 12,
                qMax(0, item.bottom() - indicator.bottom() - 6));
            painter.drawText(
                labelRect,
                Qt::AlignHCenter | Qt::AlignTop,
                fm.elidedText(destination.text, Qt::ElideRight, labelRect.width()));
        }
    }

    if (hasFocus()) {
        QPen focusPen(spec.focusRingColor);
        focusPen.setWidth(2);
        painter.setPen(focusPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(rect().adjusted(1, 1, -2, -2), 10, 10);
    }
}

void QtMaterialNavigationBar::changeEvent(QEvent* event)
{
    QtMaterialControl::changeEvent(event);
    switch (event->type()) {
    case QEvent::LayoutDirectionChange:
    case QEvent::FontChange:
    case QEvent::PaletteChange:
    case QEvent::EnabledChange:
        updateGeometry();
        update();
        syncAccessibility();
        break;
    default:
        break;
    }
}

void QtMaterialNavigationBar::themeChangedEvent(const QtMaterial::Theme& theme)
{
    QtMaterialControl::themeChangedEvent(theme);
    update();
}

void QtMaterialNavigationBar::syncAccessibility()
{
#ifndef QT_NO_ACCESSIBILITY
    QtMaterialItemAccessibility::notifyItems(this);
#endif
    const QString summary = accessibilitySummary();
    setAccessibleDescription(summary);
    if (summary != d_ptr->lastAccessibilitySummary) {
        d_ptr->lastAccessibilitySummary = summary;
        Q_EMIT accessibilitySummaryChanged(summary);
    }
}

} // namespace QtMaterial
