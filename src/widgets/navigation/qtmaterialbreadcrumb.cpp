#include "qtmaterial/widgets/navigation/qtmaterialbreadcrumb.h"

#include <algorithm>

#include <QEvent>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QList>
#include <QMenu>
#include <QResizeEvent>
#include <QSet>
#include <QToolButton>

namespace QtMaterial {

class QtMaterialBreadcrumbPrivate final
{
public:
    QStringList items;
    int currentIndex = -1;
    int maximumVisibleItems = 0;
    bool responsiveElisionEnabled = false;
    int effectiveVisibleLimit = -1;
    QHBoxLayout* layout = nullptr;
    QList<QToolButton*> buttons;
    QList<int> buttonIndexes;
    QList<QLabel*> separators;
};

namespace {

QList<int> breadcrumbVisibleIndexes(
    int itemCount,
    int currentIndex,
    int limit)
{
    QList<int> visibleIndexes;

    if (itemCount <= 0) {
        return visibleIndexes;
    }

    const int normalizedLimit =
        qBound(1, limit, itemCount);

    if (itemCount <= normalizedLimit) {
        for (int index = 0; index < itemCount; ++index) {
            visibleIndexes.push_back(index);
        }
        return visibleIndexes;
    }

    if (normalizedLimit == 1) {
        visibleIndexes.push_back(
            currentIndex >= 0
                ? currentIndex
                : itemCount - 1);
        return visibleIndexes;
    }

    QSet<int> selected;
    selected.insert(0);
    selected.insert(itemCount - 1);

    if (currentIndex >= 0
        && currentIndex < itemCount) {
        selected.insert(currentIndex);
    }

    for (int index = itemCount - 2;
         selected.size() < normalizedLimit
             && index > 0;
         --index) {
        selected.insert(index);
    }

    for (int index = 1;
         selected.size() < normalizedLimit
             && index < itemCount - 1;
         ++index) {
        selected.insert(index);
    }

    visibleIndexes = selected.values();
    std::sort(
        visibleIndexes.begin(),
        visibleIndexes.end());
    return visibleIndexes;
}

int estimatedBreadcrumbWidth(
    const QStringList& items,
    const QList<int>& visibleIndexes,
    const QFontMetrics& metrics)
{
    if (visibleIndexes.isEmpty()) {
        return 0;
    }

    constexpr int kButtonChrome = 28;
    constexpr int kSeparatorChrome = 12;
    constexpr int kOverflowChrome = 28;

    const int separatorWidth =
        metrics.horizontalAdvance(
            QStringLiteral("›"))
        + kSeparatorChrome;
    const int overflowWidth =
        metrics.horizontalAdvance(
            QStringLiteral("…"))
        + kOverflowChrome;

    int width = 0;
    int previousIndex = -1;

    for (int position = 0;
         position < visibleIndexes.size();
         ++position) {
        const int index =
            visibleIndexes.at(position);

        if (position > 0) {
            width += separatorWidth;
            if (index - previousIndex > 1) {
                width +=
                    overflowWidth
                    + separatorWidth;
            }
        }

        width +=
            metrics.horizontalAdvance(
                items.value(index))
            + kButtonChrome;
        previousIndex = index;
    }

    return width;
}

} // namespace

QtMaterialBreadcrumb::QtMaterialBreadcrumb(QWidget* parent)
    : QWidget(parent)
    , d_ptr(std::make_unique<QtMaterialBreadcrumbPrivate>())
{
    setObjectName(QStringLiteral("QtMaterialBreadcrumb"));
    setAccessibleName(tr("Breadcrumb"));
    d_ptr->layout = new QHBoxLayout(this);
    d_ptr->layout->setContentsMargins(0, 0, 0, 0);
    d_ptr->layout->setSpacing(4);
}

QtMaterialBreadcrumb::~QtMaterialBreadcrumb() = default;

QStringList QtMaterialBreadcrumb::items() const
{
    return d_ptr->items;
}

void QtMaterialBreadcrumb::setItems(const QStringList& items)
{
    if (d_ptr->items == items) {
        return;
    }

    d_ptr->items = items;
    const int nextIndex = d_ptr->items.isEmpty() ? -1 : d_ptr->items.size() - 1;
    const bool indexChanged = d_ptr->currentIndex != nextIndex;
    d_ptr->currentIndex = nextIndex;
    rebuild();
    if (indexChanged) {
        Q_EMIT currentIndexChanged(d_ptr->currentIndex);
    }
}

void QtMaterialBreadcrumb::addItem(const QString& text)
{
    d_ptr->items.push_back(text);
    d_ptr->currentIndex = d_ptr->items.size() - 1;
    rebuild();
    Q_EMIT currentIndexChanged(d_ptr->currentIndex);
}

void QtMaterialBreadcrumb::clear()
{
    setItems({});
}

int QtMaterialBreadcrumb::currentIndex() const noexcept
{
    return d_ptr->currentIndex;
}

void QtMaterialBreadcrumb::setCurrentIndex(int index)
{
    if (d_ptr->items.isEmpty()) {
        index = -1;
    } else {
        index = qBound(0, index, d_ptr->items.size() - 1);
    }

    if (d_ptr->currentIndex == index) {
        return;
    }

    d_ptr->currentIndex = index;
    if (d_ptr->responsiveElisionEnabled
        || (d_ptr->maximumVisibleItems > 0
            && d_ptr->items.size()
                > d_ptr->maximumVisibleItems)) {
        rebuild();
    } else {
        refreshCurrentSegment();
    }
    Q_EMIT currentIndexChanged(d_ptr->currentIndex);
}

int QtMaterialBreadcrumb::maximumVisibleItems() const noexcept
{
    return d_ptr->maximumVisibleItems;
}

void QtMaterialBreadcrumb::setMaximumVisibleItems(int count)
{
    const int normalized = qMax(0, count);
    if (d_ptr->maximumVisibleItems == normalized) {
        return;
    }

    d_ptr->maximumVisibleItems = normalized;
    rebuild();
    Q_EMIT maximumVisibleItemsChanged(normalized);
}

bool QtMaterialBreadcrumb::responsiveElisionEnabled() const noexcept
{
    return d_ptr->responsiveElisionEnabled;
}

void QtMaterialBreadcrumb::setResponsiveElisionEnabled(
    bool enabled)
{
    if (d_ptr->responsiveElisionEnabled == enabled) {
        return;
    }

    d_ptr->responsiveElisionEnabled = enabled;
    d_ptr->layout->setSizeConstraint(
        enabled
            ? QLayout::SetNoConstraint
            : QLayout::SetDefaultConstraint);
    rebuild();
    updateGeometry();
    Q_EMIT responsiveElisionEnabledChanged(enabled);
}

int QtMaterialBreadcrumb::effectiveVisibleLimit() const
{
    const int itemCount = d_ptr->items.size();
    if (itemCount <= 0) {
        return 0;
    }

    int limit =
        d_ptr->maximumVisibleItems > 0
            ? qMin(
                  d_ptr->maximumVisibleItems,
                  itemCount)
            : itemCount;

    if (!d_ptr->responsiveElisionEnabled) {
        return limit;
    }

    const int availableWidth =
        qMax(0, contentsRect().width());
    if (availableWidth <= 0) {
        return limit;
    }

    const QFontMetrics metrics(font());

    while (limit > 1) {
        const QList<int> visibleIndexes =
            breadcrumbVisibleIndexes(
                itemCount,
                d_ptr->currentIndex,
                limit);
        if (estimatedBreadcrumbWidth(
                d_ptr->items,
                visibleIndexes,
                metrics)
            <= availableWidth) {
            break;
        }
        --limit;
    }

    return limit;
}

void QtMaterialBreadcrumb::rebuild()
{
    d_ptr->buttons.clear();
    d_ptr->buttonIndexes.clear();
    d_ptr->separators.clear();

    while (QLayoutItem* item = d_ptr->layout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }

    const int itemCount = d_ptr->items.size();
    const int limit = effectiveVisibleLimit();
    d_ptr->effectiveVisibleLimit = limit;

    const QList<int> visibleIndexes =
        breadcrumbVisibleIndexes(
            itemCount,
            d_ptr->currentIndex,
            qMax(1, limit));

    const auto addSeparator = [this]() {
        auto* separator = new QLabel(this);
        separator->setAccessibleName(
            tr("Breadcrumb separator"));
        separator->setFocusPolicy(Qt::NoFocus);
        d_ptr->separators.push_back(separator);
        d_ptr->layout->addWidget(separator);
    };

    const auto addOverflow = [this](
        int firstHidden,
        int lastHidden) {
        auto* overflow = new QToolButton(this);
        overflow->setText(QStringLiteral("…"));
        overflow->setAutoRaise(true);
        overflow->setPopupMode(
            QToolButton::InstantPopup);
        overflow->setAccessibleName(
            tr("More breadcrumb items"));

        auto* menu = new QMenu(overflow);
        for (int index = firstHidden;
             index <= lastHidden;
             ++index) {
            QAction* action =
                menu->addAction(d_ptr->items.at(index));
            action->setData(index);
            connect(
                action,
                &QAction::triggered,
                this,
                [this, index]() {
                    const QString text =
                        d_ptr->items.value(index);
                    setCurrentIndex(index);
                    Q_EMIT activated(index, text);
                });
        }
        overflow->setMenu(menu);
        d_ptr->layout->addWidget(overflow);
    };

    int previousIndex = -1;

    if (!visibleIndexes.isEmpty()
        && visibleIndexes.first() > 0) {
        addOverflow(
            0,
            visibleIndexes.first() - 1);
        addSeparator();
    }

    for (int visiblePosition = 0;
         visiblePosition < visibleIndexes.size();
         ++visiblePosition) {
        const int index =
            visibleIndexes.at(visiblePosition);

        if (visiblePosition > 0) {
            addSeparator();
            if (index - previousIndex > 1) {
                addOverflow(
                    previousIndex + 1,
                    index - 1);
                addSeparator();
            }
        }

        const QString text = d_ptr->items.at(index);
        auto* button = new QToolButton(this);
        button->setText(text);
        button->setAutoRaise(true);
        button->setAccessibleName(text);
        connect(
            button,
            &QToolButton::clicked,
            this,
            [this, index, text]() {
                setCurrentIndex(index);
                Q_EMIT activated(index, text);
            });
        d_ptr->buttons.push_back(button);
        d_ptr->buttonIndexes.push_back(index);
        d_ptr->layout->addWidget(button);
        previousIndex = index;
    }

    if (!visibleIndexes.isEmpty()
        && visibleIndexes.last() < itemCount - 1) {
        addSeparator();
        addOverflow(
            visibleIndexes.last() + 1,
            itemCount - 1);
    }

    refreshDirection();
    refreshCurrentSegment();
    d_ptr->layout->addStretch(1);
}

void QtMaterialBreadcrumb::refreshCurrentSegment()
{
    for (int buttonPosition = 0;
         buttonPosition < d_ptr->buttons.size();
         ++buttonPosition) {
        QToolButton* button =
            d_ptr->buttons.at(buttonPosition);
        const int itemIndex =
            d_ptr->buttonIndexes.value(
                buttonPosition,
                -1);
        const bool current =
            itemIndex == d_ptr->currentIndex;
        button->setEnabled(!current);
        button->setAccessibleDescription(
            current
                ? tr("%1 of %2, current location")
                      .arg(itemIndex + 1)
                      .arg(d_ptr->items.size())
                : tr("%1 of %2")
                      .arg(itemIndex + 1)
                      .arg(d_ptr->items.size()));
    }

    setAccessibleDescription(
        d_ptr->items.isEmpty()
            ? tr("Empty breadcrumb")
            : tr("Path: %1").arg(
                  d_ptr->items.join(QStringLiteral(" / "))));
}

void QtMaterialBreadcrumb::refreshDirection()
{
    const QString separatorText =
        layoutDirection() == Qt::RightToLeft
            ? QStringLiteral("‹")
            : QStringLiteral("›");
    for (QLabel* separator : d_ptr->separators) {
        separator->setText(separatorText);
    }
}

void QtMaterialBreadcrumb::changeEvent(QEvent* event)
{
    QWidget::changeEvent(event);
    if (event
        && event->type() == QEvent::LayoutDirectionChange) {
        refreshDirection();
    }
}

void QtMaterialBreadcrumb::resizeEvent(
    QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    if (!d_ptr->responsiveElisionEnabled) {
        return;
    }

    const int nextLimit =
        effectiveVisibleLimit();
    if (nextLimit != d_ptr->effectiveVisibleLimit) {
        rebuild();
    }
}

} // namespace QtMaterial
