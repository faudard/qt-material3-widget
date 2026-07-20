#include "qtmaterial/widgets/data/qtmateriallist.h"

#include <QEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPointer>
#include <QStringList>
#include <QVBoxLayout>

#include "qtmaterial/specs/qtmaterialdataspecresolver.h"
#include "qtmaterial/widgets/data/qtmateriallistitem.h"

namespace QtMaterial {

struct QtMaterialListPrivate
{
    QVBoxLayout* layout = nullptr;
    QList<QPointer<QtMaterialListItem>> items;

    mutable ListSpec spec;
    mutable bool specDirty = true;

    bool dividersVisible = false;
    bool dividersExplicit = false;

    QtMaterialList::SelectionMode selectionMode =
        QtMaterialList::SelectionMode::SingleSelection;
    int currentIndex = -1;
    bool syncingSelection = false;
    QString lastAccessibilitySummary;
};

QtMaterialList::QtMaterialList(QWidget* parent)
    : QtMaterialWidget(parent)
    , d_ptr(std::make_unique<QtMaterialListPrivate>())
{
    setMaterialComponent(QStringLiteral("List"));
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover, true);
    setAutoFillBackground(false);
    setAccessibleName(tr("List"));

    d_ptr->layout = new QVBoxLayout(this);
    d_ptr->layout->setContentsMargins(0, 0, 0, 0);
    d_ptr->layout->setSpacing(0);

    ensureSpecResolved();
    applyResolvedSpec();
    syncAccessibility();
}

QtMaterialList::~QtMaterialList() = default;

int QtMaterialList::count() const noexcept
{
    return d_ptr->items.size();
}

bool QtMaterialList::isEmpty() const noexcept
{
    return d_ptr->items.isEmpty();
}

QtMaterialListItem* QtMaterialList::itemAt(
    int index) const
{
    return isValidIndex(index)
        ? d_ptr->items.at(index).data()
        : nullptr;
}

int QtMaterialList::indexOf(
    const QtMaterialListItem* item) const noexcept
{
    if (!item) {
        return -1;
    }

    for (int index = 0;
         index < d_ptr->items.size();
         ++index) {
        if (d_ptr->items.at(index).data() == item) {
            return index;
        }
    }

    return -1;
}

void QtMaterialList::addItem(
    QtMaterialListItem* item)
{
    insertItem(count(), item);
}

QtMaterialListItem* QtMaterialList::addItem(
    const QString& headline)
{
    auto* item =
        new QtMaterialListItem(headline, this);
    addItem(item);
    return item;
}

void QtMaterialList::insertItem(
    int index,
    QtMaterialListItem* item)
{
    if (!item) {
        return;
    }

    const QList<int> selectionBefore =
        selectedIndexes();
    QtMaterialListItem* currentItem =
        itemAt(d_ptr->currentIndex);
    const int oldCurrentIndex =
        d_ptr->currentIndex;

    const int existing = indexOf(item);
    const bool inserted = existing < 0;

    if (existing >= 0) {
        index = qBound(0, index, count());
        if (existing == index
            || (existing + 1 == index
                && index == count())) {
            return;
        }

        d_ptr->items.removeAt(existing);
        d_ptr->layout->removeWidget(item);

        if (existing < index) {
            --index;
        }
    } else {
        if (auto* previousList =
                qobject_cast<QtMaterialList*>(
                    item->parentWidget())) {
            if (previousList != this) {
                const int previousIndex =
                    previousList->indexOf(item);
                if (previousIndex >= 0) {
                    previousList->takeItem(
                        previousIndex);
                }
            }
        }

        initialiseItem(item);
    }

    index = qBound(
        0,
        index,
        d_ptr->items.size());

    if (item->parentWidget() != this) {
        item->setParent(this);
    }

    d_ptr->items.insert(index, item);
    d_ptr->layout->insertWidget(index, item);

    if (currentItem) {
        d_ptr->currentIndex =
            indexOf(currentItem);
    } else if (
        d_ptr->currentIndex < 0
        && item->isEnabled()) {
        d_ptr->currentIndex = index;
    }

    syncItemSelection();
    synchronizeItemDividers();
    updateGeometry();
    update();

    if (inserted) {
        Q_EMIT countChanged(count());
    }
    if (oldCurrentIndex != d_ptr->currentIndex) {
        Q_EMIT currentIndexChanged(
            d_ptr->currentIndex);
    }
    if (selectionBefore != selectedIndexes()) {
        Q_EMIT selectionChanged();
    }

    syncAccessibility();
}

QtMaterialListItem* QtMaterialList::takeItem(
    int index)
{
    if (!isValidIndex(index)) {
        return nullptr;
    }

    const QList<int> selectionBefore =
        selectedIndexes();
    const int oldCurrentIndex =
        d_ptr->currentIndex;

    QtMaterialListItem* item =
        d_ptr->items.at(index).data();
    d_ptr->items.removeAt(index);

    if (item) {
        d_ptr->layout->removeWidget(item);
        item->removeEventFilter(this);
        QObject::disconnect(
            item,
            nullptr,
            this,
            nullptr);

        d_ptr->syncingSelection = true;
        item->setSelected(false);
        d_ptr->syncingSelection = false;
        item->setParent(nullptr);
    }

    if (oldCurrentIndex == index) {
        d_ptr->currentIndex = -1;
    } else if (oldCurrentIndex > index) {
        d_ptr->currentIndex =
            oldCurrentIndex - 1;
    }

    synchronizeItemDividers();
    updateGeometry();
    update();

    Q_EMIT countChanged(count());
    if (oldCurrentIndex != d_ptr->currentIndex) {
        Q_EMIT currentIndexChanged(
            d_ptr->currentIndex);
    }
    if (selectionBefore != selectedIndexes()) {
        Q_EMIT selectionChanged();
    }

    syncAccessibility();
    return item;
}

void QtMaterialList::removeItem(int index)
{
    delete takeItem(index);
}

void QtMaterialList::removeItem(
    QtMaterialListItem* item)
{
    const int index = indexOf(item);
    if (index < 0) {
        return;
    }

    takeItem(index);
}

void QtMaterialList::clear()
{
    if (d_ptr->items.isEmpty()) {
        return;
    }

    const bool hadSelection =
        !selectedIndexes().isEmpty();
    const bool hadCurrent =
        d_ptr->currentIndex != -1;

    const auto items = d_ptr->items;
    d_ptr->items.clear();
    d_ptr->currentIndex = -1;

    for (const QPointer<QtMaterialListItem>& pointer
         : items) {
        QtMaterialListItem* item = pointer.data();
        if (!item) {
            continue;
        }

        d_ptr->layout->removeWidget(item);
        item->removeEventFilter(this);
        QObject::disconnect(
            item,
            nullptr,
            this,
            nullptr);
        delete item;
    }

    updateGeometry();
    update();

    Q_EMIT countChanged(0);
    if (hadCurrent) {
        Q_EMIT currentIndexChanged(-1);
    }
    if (hadSelection) {
        Q_EMIT selectionChanged();
    }

    syncAccessibility();
}

int QtMaterialList::currentIndex() const noexcept
{
    return d_ptr->currentIndex;
}

void QtMaterialList::setCurrentIndex(int index)
{
    if (index < -1
        || index >= count()
        || (index >= 0
            && !isItemEnabled(index))) {
        return;
    }

    const QList<int> selectionBefore =
        selectedIndexes();
    const int oldCurrentIndex =
        d_ptr->currentIndex;

    d_ptr->currentIndex = index;

    d_ptr->syncingSelection = true;
    switch (d_ptr->selectionMode) {
    case SelectionMode::NoSelection:
        for (const auto& pointer : d_ptr->items) {
            if (pointer) {
                pointer->setSelected(false);
            }
        }
        break;

    case SelectionMode::SingleSelection:
        for (int itemIndex = 0;
             itemIndex < count();
             ++itemIndex) {
            if (auto* item = itemAt(itemIndex)) {
                item->setSelected(
                    itemIndex == index);
            }
        }
        break;

    case SelectionMode::MultiSelection:
        if (auto* item = itemAt(index)) {
            item->setSelected(true);
        }
        break;
    }
    d_ptr->syncingSelection = false;

    if (oldCurrentIndex != index) {
        Q_EMIT currentIndexChanged(index);
    }
    if (selectionBefore != selectedIndexes()) {
        Q_EMIT selectionChanged();
    }

    syncAccessibility();
}

QtMaterialList::SelectionMode
QtMaterialList::selectionMode() const noexcept
{
    return d_ptr->selectionMode;
}

void QtMaterialList::setSelectionMode(
    SelectionMode mode)
{
    if (d_ptr->selectionMode == mode) {
        return;
    }

    const QList<int> selectionBefore =
        selectedIndexes();
    d_ptr->selectionMode = mode;
    syncItemSelection();

    Q_EMIT selectionModeChanged(mode);
    if (selectionBefore != selectedIndexes()) {
        Q_EMIT selectionChanged();
    }

    syncAccessibility();
}

QList<int> QtMaterialList::selectedIndexes() const
{
    QList<int> result;
    for (int index = 0;
         index < count();
         ++index) {
        const QtMaterialListItem* item =
            itemAt(index);
        if (item && item->isSelected()) {
            result.append(index);
        }
    }
    return result;
}

void QtMaterialList::clearSelection()
{
    const QList<int> selectionBefore =
        selectedIndexes();
    if (selectionBefore.isEmpty()) {
        return;
    }

    d_ptr->syncingSelection = true;
    for (const auto& pointer : d_ptr->items) {
        if (pointer) {
            pointer->setSelected(false);
        }
    }
    d_ptr->syncingSelection = false;

    Q_EMIT selectionChanged();
    syncAccessibility();
}

bool QtMaterialList::isItemEnabled(
    int index) const
{
    const QtMaterialListItem* item =
        itemAt(index);
    return item && item->isEnabled();
}

void QtMaterialList::setItemEnabled(
    int index,
    bool enabled)
{
    QtMaterialListItem* item =
        itemAt(index);
    if (!item || item->isEnabled() == enabled) {
        return;
    }

    const QList<int> selectionBefore =
        selectedIndexes();
    const int oldCurrentIndex =
        d_ptr->currentIndex;

    item->setEnabled(enabled);

    if (!enabled && item->isSelected()) {
        d_ptr->syncingSelection = true;
        item->setSelected(false);
        d_ptr->syncingSelection = false;
    }

    if (!enabled && oldCurrentIndex == index) {
        int replacement =
            nextEnabledIndex(index, 1);
        if (replacement < 0) {
            replacement =
                nextEnabledIndex(index, -1);
        }
        d_ptr->currentIndex = replacement;
        syncItemSelection();
    }

    if (oldCurrentIndex != d_ptr->currentIndex) {
        Q_EMIT currentIndexChanged(
            d_ptr->currentIndex);
    }
    if (selectionBefore != selectedIndexes()) {
        Q_EMIT selectionChanged();
    }

    syncAccessibility();
    update();
}

QString QtMaterialList::itemAccessibleText(
    int index) const
{
    const QtMaterialListItem* item =
        itemAt(index);
    if (!item) {
        return {};
    }

    QStringList parts;
    const QString headline =
        item->headlineText().trimmed();
    parts << (headline.isEmpty()
                  ? tr("List item")
                  : headline);

    const QString supporting =
        item->supportingText().trimmed();
    if (!supporting.isEmpty()) {
        parts << supporting;
    }

    parts << tr("%1 of %2")
                 .arg(index + 1)
                 .arg(count());

    if (item->isSelected()) {
        parts << tr("selected");
    }
    if (!item->isEnabled()) {
        parts << tr("disabled");
    }

    return parts.join(QStringLiteral(", "));
}

QString QtMaterialList::accessibilitySummary() const
{
    QString summary =
        tr("%n item(s)", nullptr, count());

    const QList<int> selected =
        selectedIndexes();
    if (!selected.isEmpty()) {
        summary += tr(", %n selected",
                      nullptr,
                      selected.size());
    }

    if (isValidIndex(d_ptr->currentIndex)) {
        const QString headline =
            itemAt(d_ptr->currentIndex)
                ->headlineText()
                .trimmed();
        if (!headline.isEmpty()) {
            summary += tr(", current %1")
                           .arg(headline);
        }
    }

    return summary;
}

bool QtMaterialList::dividersVisible() const noexcept
{
    ensureSpecResolved();

    return d_ptr->dividersExplicit
        ? d_ptr->dividersVisible
        : d_ptr->spec.showDividers;
}

void QtMaterialList::setDividersVisible(
    bool visible)
{
    const bool oldValue =
        dividersVisible();

    d_ptr->dividersExplicit = true;
    d_ptr->dividersVisible = visible;

    if (oldValue == visible) {
        return;
    }

    synchronizeItemDividers();
    update();

    Q_EMIT dividersVisibleChanged(visible);
}

const ListSpec& QtMaterialList::resolvedSpec() const
{
    ensureSpecResolved();
    return d_ptr->spec;
}

QSize QtMaterialList::sizeHint() const
{
    ensureSpecResolved();

    const QSize contentSize =
        d_ptr->layout
            ? d_ptr->layout->sizeHint()
            : QSize();

    return contentSize.expandedTo(
        d_ptr->spec.minimumViewportSize);
}

QSize QtMaterialList::minimumSizeHint() const
{
    ensureSpecResolved();
    return d_ptr->spec.minimumViewportSize;
}

bool QtMaterialList::eventFilter(
    QObject* watched,
    QEvent* event)
{
    if (event
        && event->type() == QEvent::FocusIn) {
        auto* item =
            qobject_cast<QtMaterialListItem*>(
                watched);
        const int index = indexOf(item);
        if (index >= 0 && isItemEnabled(index)) {
            setCurrentIndex(index);
        }
    }

    return QtMaterialWidget::eventFilter(
        watched,
        event);
}

void QtMaterialList::keyPressEvent(
    QKeyEvent* event)
{
    if (!event) {
        return;
    }

    bool handled = false;
    switch (event->key()) {
    case Qt::Key_Up:
        handled = moveCurrent(-1);
        break;
    case Qt::Key_Down:
        handled = moveCurrent(1);
        break;
    case Qt::Key_Home:
        handled = moveToBoundary(true);
        break;
    case Qt::Key_End:
        handled = moveToBoundary(false);
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space:
        handled = activateIndex(
            d_ptr->currentIndex);
        break;
    default:
        break;
    }

    if (handled) {
        event->accept();
        return;
    }

    QtMaterialWidget::keyPressEvent(event);
}

void QtMaterialList::changeEvent(QEvent* event)
{
    QtMaterialWidget::changeEvent(event);

    if (event
        && event->type()
            == QEvent::LanguageChange) {
        syncAccessibility();
    }
}

void QtMaterialList::paintEvent(
    QPaintEvent*)
{
    ensureSpecResolved();

    const QRectF visualRect =
        QRectF(rect()).adjusted(
            0.5,
            0.5,
            -0.5,
            -0.5);

    if (!visualRect.isValid()) {
        return;
    }

    const qreal radius =
        qBound<qreal>(
            0.0,
            d_ptr->spec.cornerRadius,
            qMin(
                visualRect.width(),
                visualRect.height())
                / 2.0);

    QPainterPath path;
    path.addRoundedRect(
        visualRect,
        radius,
        radius);

    QPainter painter(this);
    painter.setRenderHint(
        QPainter::Antialiasing,
        true);
    painter.fillPath(
        path,
        d_ptr->spec.containerColor);

    if (hasFocus()
        && d_ptr->spec.focusRingWidth > 0) {
        QPen pen(
            d_ptr->spec.focusRingColor,
            d_ptr->spec.focusRingWidth);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(path);
    }
}

void QtMaterialList::themeChangedEvent(
    const Theme&)
{
    d_ptr->specDirty = true;
    ensureSpecResolved();
    applyResolvedSpec();
    syncAccessibility();
}

void QtMaterialList::initialiseItem(
    QtMaterialListItem* item)
{
    if (!item) {
        return;
    }

    if (item->parentWidget() != this) {
        item->setParent(this);
    }

    item->installEventFilter(this);

    connect(
        item,
        &QtMaterialListItem::clicked,
        this,
        [this, item]() {
            const int index = indexOf(item);
            if (index < 0
                || !isItemEnabled(index)) {
                return;
            }

            if (d_ptr->selectionMode
                == SelectionMode::MultiSelection) {
                const QList<int> selectionBefore =
                    selectedIndexes();
                const int oldCurrentIndex =
                    d_ptr->currentIndex;

                d_ptr->currentIndex = index;
                d_ptr->syncingSelection = true;
                item->setSelected(
                    !item->isSelected());
                d_ptr->syncingSelection = false;

                if (oldCurrentIndex != index) {
                    Q_EMIT currentIndexChanged(index);
                }
                if (selectionBefore
                    != selectedIndexes()) {
                    Q_EMIT selectionChanged();
                }
                syncAccessibility();
            } else {
                setCurrentIndex(index);
            }

            Q_EMIT itemClicked(index);
        });

    connect(
        item,
        &QtMaterialListItem::activated,
        this,
        [this, item]() {
            activateIndex(indexOf(item));
        });

    connect(
        item,
        &QtMaterialListItem::selectionChanged,
        this,
        [this, item](bool selected) {
            if (d_ptr->syncingSelection) {
                return;
            }

            const int index = indexOf(item);
            if (index < 0) {
                return;
            }

            const int oldCurrentIndex =
                d_ptr->currentIndex;

            if (d_ptr->selectionMode
                == SelectionMode::NoSelection) {
                d_ptr->syncingSelection = true;
                item->setSelected(false);
                d_ptr->syncingSelection = false;
            } else if (d_ptr->selectionMode
                       == SelectionMode::SingleSelection) {
                if (selected) {
                    d_ptr->currentIndex = index;
                    syncItemSelection();
                } else if (
                    d_ptr->currentIndex == index) {
                    d_ptr->currentIndex = -1;
                }
            }

            if (oldCurrentIndex
                != d_ptr->currentIndex) {
                Q_EMIT currentIndexChanged(
                    d_ptr->currentIndex);
            }

            Q_EMIT selectionChanged();
            syncAccessibility();
        });

    connect(
        item,
        &QObject::destroyed,
        this,
        &QtMaterialList::removeDestroyedItem);
}

void QtMaterialList::syncItemSelection()
{
    d_ptr->syncingSelection = true;

    switch (d_ptr->selectionMode) {
    case SelectionMode::NoSelection:
        for (const auto& pointer : d_ptr->items) {
            if (pointer) {
                pointer->setSelected(false);
            }
        }
        break;

    case SelectionMode::SingleSelection:
        for (int index = 0;
             index < count();
             ++index) {
            if (auto* item = itemAt(index)) {
                item->setSelected(
                    index == d_ptr->currentIndex);
            }
        }
        break;

    case SelectionMode::MultiSelection:
        break;
    }

    d_ptr->syncingSelection = false;
}

void QtMaterialList::syncAccessibility()
{
    setAccessibleName(tr("List"));

    for (int index = 0;
         index < count();
         ++index) {
        if (auto* item = itemAt(index)) {
            item->setAccessibleDescription(
                itemAccessibleText(index));
        }
    }

    const QString summary =
        accessibilitySummary();
    setAccessibleDescription(summary);
    emitAccessibilityIfChanged();
}

void QtMaterialList::emitAccessibilityIfChanged()
{
    const QString summary =
        accessibilitySummary();
    if (summary
        == d_ptr->lastAccessibilitySummary) {
        return;
    }

    d_ptr->lastAccessibilitySummary = summary;
    Q_EMIT accessibilitySummaryChanged(summary);
}

bool QtMaterialList::activateIndex(int index)
{
    if (!isValidIndex(index)
        || !isItemEnabled(index)) {
        return false;
    }

    setCurrentIndex(index);
    Q_EMIT itemActivated(index);
    return true;
}

bool QtMaterialList::moveCurrent(int delta)
{
    if (delta == 0 || isEmpty()) {
        return false;
    }

    int next = -1;
    if (d_ptr->currentIndex < 0) {
        next = delta > 0
            ? firstEnabledIndex()
            : lastEnabledIndex();
    } else {
        next = nextEnabledIndex(
            d_ptr->currentIndex,
            delta > 0 ? 1 : -1);
    }

    if (next < 0) {
        return false;
    }

    setCurrentIndex(next);
    if (auto* item = itemAt(next)) {
        item->setFocus(Qt::TabFocusReason);
    }
    return true;
}

bool QtMaterialList::moveToBoundary(bool first)
{
    const int index = first
        ? firstEnabledIndex()
        : lastEnabledIndex();
    if (index < 0) {
        return false;
    }

    setCurrentIndex(index);
    if (auto* item = itemAt(index)) {
        item->setFocus(Qt::TabFocusReason);
    }
    return true;
}

int QtMaterialList::nextEnabledIndex(
    int start,
    int delta) const
{
    if (delta == 0) {
        return -1;
    }

    for (int index = start + delta;
         index >= 0 && index < count();
         index += delta) {
        if (isItemEnabled(index)) {
            return index;
        }
    }

    return -1;
}

int QtMaterialList::firstEnabledIndex() const
{
    for (int index = 0;
         index < count();
         ++index) {
        if (isItemEnabled(index)) {
            return index;
        }
    }
    return -1;
}

int QtMaterialList::lastEnabledIndex() const
{
    for (int index = count() - 1;
         index >= 0;
         --index) {
        if (isItemEnabled(index)) {
            return index;
        }
    }
    return -1;
}

bool QtMaterialList::isValidIndex(
    int index) const noexcept
{
    return index >= 0 && index < count();
}

void QtMaterialList::ensureSpecResolved() const
{
    if (!d_ptr->specDirty) {
        return;
    }

    const DataSpecResolver resolver;
    d_ptr->spec =
        resolver.listSpec(
            theme(),
            Density::Default);
    d_ptr->specDirty = false;
}

void QtMaterialList::applyResolvedSpec()
{
    ensureSpecResolved();

    if (!d_ptr->layout) {
        return;
    }

    const QMargins margins =
        d_ptr->spec.contentMargins;

    d_ptr->layout->setContentsMargins(
        margins.left(),
        margins.top(),
        margins.right(),
        margins.bottom());
    d_ptr->layout->setSpacing(
        d_ptr->spec.itemSpacing);

    synchronizeItemDividers();
    updateGeometry();
    update();
}

void QtMaterialList::synchronizeItemDividers()
{
    const bool visible =
        dividersVisible();

    const int itemCount =
        d_ptr->items.size();

    for (int index = 0;
         index < itemCount;
         ++index) {
        QtMaterialListItem* item =
            d_ptr->items.at(index).data();

        if (!item) {
            continue;
        }

        item->setDividerVisible(
            visible
            && index + 1 < itemCount);
    }
}

void QtMaterialList::removeDestroyedItem(
    QObject* object)
{
    const int oldCount = count();
    const int oldCurrentIndex =
        d_ptr->currentIndex;
    bool removed = false;

    for (int index = count() - 1;
         index >= 0;
         --index) {
        if (d_ptr->items.at(index).isNull()
            || d_ptr->items.at(index).data()
                == object) {
            d_ptr->items.removeAt(index);
            removed = true;

            if (d_ptr->currentIndex == index) {
                d_ptr->currentIndex = -1;
            } else if (
                d_ptr->currentIndex > index) {
                --d_ptr->currentIndex;
            }
        }
    }

    if (!removed) {
        return;
    }

    synchronizeItemDividers();
    updateGeometry();
    update();

    if (oldCount != count()) {
        Q_EMIT countChanged(count());
    }
    if (oldCurrentIndex
        != d_ptr->currentIndex) {
        Q_EMIT currentIndexChanged(
            d_ptr->currentIndex);
    }

    Q_EMIT selectionChanged();
    syncAccessibility();
}

} // namespace QtMaterial
