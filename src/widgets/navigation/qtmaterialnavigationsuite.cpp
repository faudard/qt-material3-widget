#include "qtmaterial/widgets/navigation/qtmaterialnavigationsuite.h"

#include <QDataStream>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QStringList>

#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/specs/qtmaterialnavigationrailspec.h"
#include "qtmaterial/specs/qtmaterialnavigationrailspecresolver.h"
#include "qtmaterialitemaccessibility_p.h"

namespace QtMaterial {

struct QtMaterialNavigationSuitePrivate
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
    WindowWidthSizeClass widthClass = WindowWidthSizeClass::Compact;
    QString lastAccessibilitySummary;

    mutable bool specDirty = true;
    mutable NavigationRailSpec spec;

    NavigationSuiteType navigationType() const noexcept
    {
        return widthClass == WindowWidthSizeClass::Compact
            ? NavigationSuiteType::NavigationBar
            : NavigationSuiteType::NavigationRail;
    }

    QRect itemRect(const QSize& size, int index) const
    {
        if (navigationType() == NavigationSuiteType::NavigationBar) {
            const int count = qMax(1, destinations.size());
            const int left = (index * size.width()) / count;
            const int right = ((index + 1) * size.width()) / count;
            return QRect(left, 0, qMax(0, right - left), qMin(80, size.height()));
        }

        const int y =
            spec.topPadding
            + index * (spec.itemHeight + spec.itemSpacing);
        return QRect(0, y, spec.railWidth, spec.itemHeight);
    }

    QRect indicatorRect(const QSize& size, int index) const
    {
        const QRect item = itemRect(size, index);
        if (navigationType() == NavigationSuiteType::NavigationBar) {
            const int width = qMax(24, qMin(64, item.width() - 8));
            return QRect(
                item.center().x() - width / 2,
                item.top() + 8,
                width,
                32);
        }

        return QRect(
            item.center().x() - spec.indicatorSize.width() / 2,
            item.top() + spec.indicatorTopOffset,
            spec.indicatorSize.width(),
            spec.indicatorSize.height());
    }

    int indexAt(const QSize& size, const QPoint& position) const
    {
        for (int index = 0; index < destinations.size(); ++index) {
            if (itemRect(size, index).contains(position)) {
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
        for (int index = 0; index < destinations.size(); ++index) {
            if (destinations.at(index).enabled) {
                return index;
            }
        }
        return -1;
    }

    int lastEnabledIndex() const noexcept
    {
        for (int index = destinations.size() - 1; index >= 0; --index) {
            if (destinations.at(index).enabled) {
                return index;
            }
        }
        return -1;
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

        for (int attempts = 0; attempts < destinations.size(); ++attempts) {
            index = (index + step + destinations.size()) % destinations.size();
            if (destinations.at(index).enabled) {
                return index;
            }
        }
        return -1;
    }
};

namespace {

QColor withOpacity(QColor color, qreal opacity)
{
    color.setAlphaF(qBound<qreal>(0.0, opacity, 1.0));
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
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(pixmap.rect(), color);
    return pixmap;
}

} // namespace

QtMaterialNavigationSuite::QtMaterialNavigationSuite(QWidget* parent)
    : QtMaterialControl(parent)
    , d_ptr(std::make_unique<QtMaterialNavigationSuitePrivate>())
{
#ifndef QT_NO_ACCESSIBILITY
    static const bool accessibilityInstalled = []() {
        QAccessible::installFactory([](const QString&, QObject* object) -> QAccessibleInterface* {
            auto* suite = qobject_cast<QtMaterialNavigationSuite*>(object);
            if (!suite) {
                return nullptr;
            }

            QtMaterialItemAccessibility::ItemAccess access;
            access.count = [suite]() { return suite->count(); };
            access.current = [suite]() { return suite->currentIndex(); };
            access.text = [suite](int index, QAccessible::Text type) {
                if (type == QAccessible::Name) {
                    return suite->destinationText(index);
                }
                return type == QAccessible::Description
                    ? suite->destinationAccessibleText(index)
                    : QString();
            };
            access.rect = [suite](int index) {
                suite->ensureSpecResolved();
                return suite->d_ptr->itemRect(suite->size(), index);
            };
            access.role = [](int) { return QAccessible::ListItem; };
            access.state = [suite](int index) {
                QAccessible::State state;
                state.disabled = !suite->isDestinationEnabled(index);
                state.focusable = true;
                state.selectable = true;
                state.selected = suite->currentIndex() == index;
                return state;
            };
            access.select = [suite](int index) { suite->setCurrentIndex(index); };

            return new QtMaterialItemAccessibility::ItemWidgetInterface(
                suite,
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
    setAccessibleName(tr("Navigation"));
    setMaterialComponent(QStringLiteral("navigation-suite"));
    setMaterialVariant(QStringLiteral("bar"));
    setMaterialRole(QStringLiteral("navigation"));
    syncAccessibility();
}

QtMaterialNavigationSuite::~QtMaterialNavigationSuite() = default;

int QtMaterialNavigationSuite::addDestination(const QString& text, const QIcon& icon)
{
    insertDestination(d_ptr->destinations.size(), text, icon);
    return d_ptr->destinations.size() - 1;
}

void QtMaterialNavigationSuite::insertDestination(
    int index,
    const QString& text,
    const QIcon& icon)
{
    index = qBound(0, index, d_ptr->destinations.size());
    d_ptr->destinations.insert(
        index,
        QtMaterialNavigationSuitePrivate::Destination{text, icon, true});

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
}

void QtMaterialNavigationSuite::removeDestination(int index)
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
        nextCurrent = d_ptr->firstEnabledIndex();
    } else if (d_ptr->currentIndex > index) {
        --nextCurrent;
    }

    if (d_ptr->currentIndex != nextCurrent) {
        d_ptr->currentIndex = nextCurrent;
        emit currentIndexChanged(nextCurrent);
    }

#ifndef QT_NO_ACCESSIBILITY
    QtMaterialItemAccessibility::notifyStructure(this);
#endif
    updateGeometry();
    update();
    syncAccessibility();
}

void QtMaterialNavigationSuite::clearDestinations()
{
    if (d_ptr->destinations.isEmpty() && d_ptr->currentIndex == -1) {
        return;
    }

    d_ptr->destinations.clear();
    d_ptr->hoveredIndex = -1;
    d_ptr->pressedIndex = -1;

    const bool changed = d_ptr->currentIndex != -1;
    d_ptr->currentIndex = -1;

#ifndef QT_NO_ACCESSIBILITY
    QtMaterialItemAccessibility::notifyStructure(this);
#endif
    if (changed) {
        emit currentIndexChanged(-1);
    }
    updateGeometry();
    update();
    syncAccessibility();
}

int QtMaterialNavigationSuite::count() const noexcept
{
    return d_ptr->destinations.size();
}

QString QtMaterialNavigationSuite::destinationText(int index) const
{
    return index >= 0 && index < d_ptr->destinations.size()
        ? d_ptr->destinations.at(index).text
        : QString();
}

QIcon QtMaterialNavigationSuite::destinationIcon(int index) const
{
    return index >= 0 && index < d_ptr->destinations.size()
        ? d_ptr->destinations.at(index).icon
        : QIcon();
}

bool QtMaterialNavigationSuite::isDestinationEnabled(int index) const noexcept
{
    return d_ptr->enabledIndex(index);
}

void QtMaterialNavigationSuite::setDestinationEnabled(int index, bool enabled)
{
    if (index < 0
        || index >= d_ptr->destinations.size()
        || d_ptr->destinations.at(index).enabled == enabled) {
        return;
    }

    d_ptr->destinations[index].enabled = enabled;
    emit destinationEnabledChanged(index, enabled);

    if (!enabled) {
        if (d_ptr->hoveredIndex == index) {
            d_ptr->hoveredIndex = -1;
        }
        if (d_ptr->pressedIndex == index) {
            d_ptr->pressedIndex = -1;
        }
        if (d_ptr->currentIndex == index) {
            const int replacement = d_ptr->nextEnabledIndex(index, 1);
            setCurrentIndex(replacement == index ? -1 : replacement);
        }
    }

    update();
    syncAccessibility();
}

int QtMaterialNavigationSuite::currentIndex() const noexcept
{
    return d_ptr->currentIndex;
}

void QtMaterialNavigationSuite::setCurrentIndex(int index)
{
    if (index < -1
        || index >= d_ptr->destinations.size()
        || d_ptr->currentIndex == index) {
        return;
    }
    if (index >= 0 && !d_ptr->destinations.at(index).enabled) {
        return;
    }

    d_ptr->currentIndex = index;
    update();
    emit currentIndexChanged(index);
    syncAccessibility();
}

WindowWidthSizeClass QtMaterialNavigationSuite::windowWidthSizeClass() const noexcept
{
    return d_ptr->widthClass;
}

void QtMaterialNavigationSuite::setWindowWidthSizeClass(WindowWidthSizeClass sizeClass)
{
    if (d_ptr->widthClass == sizeClass) {
        return;
    }

    const NavigationSuiteType previousType = d_ptr->navigationType();
    d_ptr->widthClass = sizeClass;
    d_ptr->hoveredIndex = -1;
    d_ptr->pressedIndex = -1;
    updateGeometry();
    update();
    emit windowWidthSizeClassChanged(sizeClass);

    const NavigationSuiteType nextType = d_ptr->navigationType();
    if (previousType != nextType) {
        setMaterialVariant(
            nextType == NavigationSuiteType::NavigationBar
                ? QStringLiteral("bar")
                : QStringLiteral("rail"));
        emit navigationTypeChanged(nextType);
    }

    syncAccessibility();
}

NavigationSuiteType QtMaterialNavigationSuite::navigationType() const noexcept
{
    return d_ptr->navigationType();
}

QString QtMaterialNavigationSuite::destinationAccessibleText(int index) const
{
    if (index < 0 || index >= d_ptr->destinations.size()) {
        return {};
    }

    const auto& destination = d_ptr->destinations.at(index);
    QStringList parts;
    parts << destination.text;
    parts << tr("%1 of %2").arg(index + 1).arg(d_ptr->destinations.size());
    if (index == d_ptr->currentIndex) {
        parts << tr("selected");
    }
    if (!destination.enabled) {
        parts << tr("disabled");
    }
    return parts.join(QStringLiteral(", "));
}

QString QtMaterialNavigationSuite::accessibilitySummary() const
{
    QString summary =
        tr("%n destination(s)", nullptr, d_ptr->destinations.size());
    summary += navigationType() == NavigationSuiteType::NavigationBar
        ? tr(", bottom navigation")
        : tr(", navigation rail");

    if (d_ptr->currentIndex >= 0
        && d_ptr->currentIndex < d_ptr->destinations.size()) {
        summary += tr(", selected %1")
            .arg(d_ptr->destinations.at(d_ptr->currentIndex).text);
    }
    return summary;
}

QByteArray QtMaterialNavigationSuite::saveWorkspaceState() const
{
    constexpr quint32 magic = 0x514d4e57; // QMNW
    constexpr quint32 version = 1;

    QStringList destinationTexts;
    destinationTexts.reserve(count());
    for (int index = 0; index < count(); ++index) {
        destinationTexts.push_back(destinationText(index));
    }

    QByteArray state;
    QDataStream stream(&state, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_12);
    stream
        << magic
        << version
        << destinationTexts
        << qint32(currentIndex())
        << qint32(int(windowWidthSizeClass()))
        << qint32(count());

    for (int index = 0; index < count(); ++index) {
        stream << isDestinationEnabled(index);
    }

    return state;
}

bool QtMaterialNavigationSuite::restoreWorkspaceState(
    const QByteArray& state)
{
    constexpr quint32 magic = 0x514d4e57; // QMNW
    constexpr quint32 version = 1;

    QDataStream stream(state);
    stream.setVersion(QDataStream::Qt_5_12);

    quint32 storedMagic = 0;
    quint32 storedVersion = 0;
    QStringList destinationTexts;
    qint32 storedCurrentIndex = -1;
    qint32 storedWidthClass = int(WindowWidthSizeClass::Compact);
    qint32 enabledCount = 0;

    stream
        >> storedMagic
        >> storedVersion
        >> destinationTexts
        >> storedCurrentIndex
        >> storedWidthClass
        >> enabledCount;

    if (
        stream.status() != QDataStream::Ok
        || storedMagic != magic
        || storedVersion != version
        || destinationTexts.size() != count()
        || enabledCount != count()
        || storedCurrentIndex < -1
        || storedCurrentIndex >= count()
        || storedWidthClass < int(WindowWidthSizeClass::Compact)
        || storedWidthClass > int(WindowWidthSizeClass::ExtraLarge)) {
        return false;
    }

    for (int index = 0; index < count(); ++index) {
        if (destinationTexts.at(index) != destinationText(index)) {
            return false;
        }
    }

    QVector<bool> enabled;
    enabled.reserve(enabledCount);
    for (qint32 index = 0; index < enabledCount; ++index) {
        bool value = false;
        stream >> value;
        if (stream.status() != QDataStream::Ok) {
            return false;
        }
        enabled.push_back(value);
    }

    if (
        !stream.atEnd()
        || (
            storedCurrentIndex >= 0
            && !enabled.at(storedCurrentIndex))) {
        return false;
    }

    for (int index = 0; index < enabled.size(); ++index) {
        setDestinationEnabled(index, enabled.at(index));
    }
    setWindowWidthSizeClass(
        static_cast<WindowWidthSizeClass>(storedWidthClass));
    setCurrentIndex(storedCurrentIndex);
    syncAccessibility();
    return true;
}

QSize QtMaterialNavigationSuite::sizeHint() const
{
    ensureSpecResolved();

    if (navigationType() == NavigationSuiteType::NavigationBar) {
        return QSize(360, 80);
    }

    const int count = d_ptr->destinations.size();
    const int height =
        d_ptr->spec.topPadding
        + d_ptr->spec.bottomPadding
        + count * d_ptr->spec.itemHeight
        + qMax(0, count - 1) * d_ptr->spec.itemSpacing;
    return QSize(d_ptr->spec.railWidth, qMax(120, height));
}

QSize QtMaterialNavigationSuite::minimumSizeHint() const
{
    ensureSpecResolved();
    return navigationType() == NavigationSuiteType::NavigationBar
        ? QSize(120, 64)
        : QSize(d_ptr->spec.railWidth, 120);
}

void QtMaterialNavigationSuite::mouseMoveEvent(QMouseEvent* event)
{
    ensureSpecResolved();

    const int candidate = d_ptr->indexAt(size(), event->pos());
    const int nextHover =
        isEnabled() && d_ptr->enabledIndex(candidate)
            ? candidate
            : -1;

    if (d_ptr->hoveredIndex != nextHover) {
        d_ptr->hoveredIndex = nextHover;
        update();
    }

    QtMaterialControl::mouseMoveEvent(event);
}

void QtMaterialNavigationSuite::mousePressEvent(QMouseEvent* event)
{
    ensureSpecResolved();

    if (event->button() == Qt::LeftButton) {
        const int candidate = d_ptr->indexAt(size(), event->pos());
        if (isEnabled() && d_ptr->enabledIndex(candidate)) {
            d_ptr->pressedIndex = candidate;
            update();
            event->accept();
            return;
        }
    }

    QtMaterialControl::mousePressEvent(event);
}

void QtMaterialNavigationSuite::mouseReleaseEvent(QMouseEvent* event)
{
    ensureSpecResolved();

    const int pressed = d_ptr->pressedIndex;
    d_ptr->pressedIndex = -1;

    if (event->button() == Qt::LeftButton
        && pressed >= 0
        && pressed == d_ptr->indexAt(size(), event->pos())
        && isEnabled()
        && d_ptr->enabledIndex(pressed)) {
        setCurrentIndex(pressed);
        emit destinationActivated(pressed);
        event->accept();
        return;
    }

    update();
    QtMaterialControl::mouseReleaseEvent(event);
}

void QtMaterialNavigationSuite::leaveEvent(QEvent* event)
{
    d_ptr->hoveredIndex = -1;
    d_ptr->pressedIndex = -1;
    update();
    QtMaterialControl::leaveEvent(event);
}

void QtMaterialNavigationSuite::focusInEvent(QFocusEvent* event)
{
    QtMaterialControl::focusInEvent(event);
    syncAccessibility();
    update();
}

void QtMaterialNavigationSuite::focusOutEvent(QFocusEvent* event)
{
    QtMaterialControl::focusOutEvent(event);
    syncAccessibility();
    update();
}

void QtMaterialNavigationSuite::keyPressEvent(QKeyEvent* event)
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

    int step = 0;
    if (navigationType() == NavigationSuiteType::NavigationBar
        && (key == Qt::Key_Left || key == Qt::Key_Right)) {
        const bool forward = key == Qt::Key_Right;
        const bool rtl = layoutDirection() == Qt::RightToLeft;
        step = forward != rtl ? 1 : -1;
    } else if (navigationType() == NavigationSuiteType::NavigationRail
               && (key == Qt::Key_Up || key == Qt::Key_Down)) {
        step = key == Qt::Key_Down ? 1 : -1;
    }

    if (step != 0) {
        setCurrentIndex(d_ptr->nextEnabledIndex(d_ptr->currentIndex, step));
        event->accept();
        return;
    }

    if ((key == Qt::Key_Return || key == Qt::Key_Enter || key == Qt::Key_Space)
        && isEnabled()
        && d_ptr->enabledIndex(d_ptr->currentIndex)) {
        emit destinationActivated(d_ptr->currentIndex);
        event->accept();
        return;
    }

    QtMaterialControl::keyPressEvent(event);
}

void QtMaterialNavigationSuite::paintEvent(QPaintEvent*)
{
    ensureSpecResolved();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), d_ptr->spec.containerColor);
    painter.setFont(d_ptr->spec.labelFont);

    const bool bar = navigationType() == NavigationSuiteType::NavigationBar;

    painter.setPen(QPen(d_ptr->spec.dividerColor, 1.0));
    if (bar) {
        painter.drawLine(0, 0, width(), 0);
    } else {
        const int x = layoutDirection() == Qt::RightToLeft ? 0 : width() - 1;
        painter.drawLine(x, 0, x, height());
    }

    for (int index = 0; index < d_ptr->destinations.size(); ++index) {
        const QRect item = d_ptr->itemRect(size(), index);
        const QRect indicator = d_ptr->indicatorRect(size(), index);
        const bool selected = index == d_ptr->currentIndex;
        const bool destinationEnabled =
            isEnabled() && d_ptr->destinations.at(index).enabled;

        QPainterPath indicatorPath;
        indicatorPath.addRoundedRect(
            QRectF(indicator),
            indicator.height() / 2.0,
            indicator.height() / 2.0);

        if (selected) {
            painter.fillPath(indicatorPath, d_ptr->spec.indicatorColor);
        }

        qreal stateOpacity = 0.0;
        if (destinationEnabled && index == d_ptr->pressedIndex) {
            stateOpacity = d_ptr->spec.pressStateLayerOpacity;
        } else if (destinationEnabled && index == d_ptr->hoveredIndex) {
            stateOpacity = d_ptr->spec.hoverStateLayerOpacity;
        } else if (destinationEnabled && selected && hasFocus()) {
            stateOpacity = d_ptr->spec.focusStateLayerOpacity;
        }

        if (stateOpacity > 0.0) {
            painter.fillPath(
                indicatorPath,
                withOpacity(d_ptr->spec.stateLayerColor, stateOpacity));
        }

        QColor iconColor;
        QColor textColor;
        if (!destinationEnabled) {
            iconColor = d_ptr->spec.disabledIconColor;
            textColor = d_ptr->spec.disabledLabelColor;
        } else if (selected) {
            iconColor = d_ptr->spec.selectedIconColor;
            textColor = d_ptr->spec.selectedLabelColor;
        } else {
            iconColor = d_ptr->spec.unselectedIconColor;
            textColor = d_ptr->spec.unselectedLabelColor;
        }

        const int iconTop = bar ? item.top() + 12 : item.top() + d_ptr->spec.iconTopOffset;
        const QRect iconRect(
            item.center().x() - d_ptr->spec.iconSize / 2,
            iconTop,
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
            painter.drawEllipse(iconRect.adjusted(4, 4, -4, -4));
        }

        painter.setPen(textColor);
        const int labelTop = bar ? item.top() + 44 : item.top() + d_ptr->spec.labelTopOffset;
        const int labelHeight = bar ? 24 : d_ptr->spec.labelHeight;
        painter.drawText(
            QRect(item.left() + 4, labelTop, item.width() - 8, labelHeight),
            Qt::AlignCenter,
            d_ptr->destinations.at(index).text);
    }

    if (QtMaterialFocusIndicator::shouldShow(
            interactionState(),
            focusReason(),
            theme().interactions())
        && d_ptr->enabledIndex(d_ptr->currentIndex)) {
        const QRectF focusRect =
            QRectF(d_ptr->indicatorRect(size(), d_ptr->currentIndex))
                .adjusted(
                    -d_ptr->spec.focusRingWidth,
                    -d_ptr->spec.focusRingWidth,
                    d_ptr->spec.focusRingWidth,
                    d_ptr->spec.focusRingWidth);

        QPainterPath focusPath;
        const qreal radius = focusRect.height() / 2.0;
        focusPath.addRoundedRect(focusRect, radius, radius);
        QtMaterialFocusIndicator::paintPathFocusRing(
            &painter,
            focusPath,
            d_ptr->spec.focusRingColor,
            d_ptr->spec.focusRingWidth);
    }
}

void QtMaterialNavigationSuite::themeChangedEvent(const Theme& theme)
{
    QtMaterialControl::themeChangedEvent(theme);
    invalidateResolvedSpec();
}

void QtMaterialNavigationSuite::invalidateResolvedSpec()
{
    d_ptr->specDirty = true;
    updateGeometry();
    update();
}

void QtMaterialNavigationSuite::ensureSpecResolved() const
{
    if (!d_ptr->specDirty) {
        return;
    }

    d_ptr->spec = NavigationRailSpecResolver().navigationRailSpec(theme());
    d_ptr->specDirty = false;
}

void QtMaterialNavigationSuite::syncAccessibility()
{
#ifndef QT_NO_ACCESSIBILITY
    QtMaterialItemAccessibility::notifyItems(this);
#endif
    const QString summary = accessibilitySummary();
    setAccessibleDescription(summary);
    if (summary != d_ptr->lastAccessibilitySummary) {
        d_ptr->lastAccessibilitySummary = summary;
        emit accessibilitySummaryChanged(summary);
    }
}

} // namespace QtMaterial
