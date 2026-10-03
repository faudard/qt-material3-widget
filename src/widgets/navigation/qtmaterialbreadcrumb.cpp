#include "qtmaterial/widgets/navigation/qtmaterialbreadcrumb.h"

#include <algorithm>

#include <QEvent>
#include <QApplication>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QHash>
#include <QLabel>
#include <QLineEdit>
#include <QKeyEvent>
#include <QLayoutItem>
#include <QList>
#include <QMenu>
#include <QMouseEvent>
#include <QMimeData>
#include <QPointer>
#include <QResizeEvent>
#include <QSet>
#include <QShortcut>
#include <QStylePainter>
#include <QStyleOptionToolButton>
#include <QToolButton>

namespace QtMaterial {

class QtMaterialBreadcrumbPrivate final
{
public:
    QStringList items;
    QHash<int, QIcon> icons;
    QHash<int, QUrl> urls;
    bool dragDropEnabled = false;
    QString location;
    bool customLocation = false;
    bool locationEditable = false;
    bool editingLocation = false;
    int currentIndex = -1;
    int maximumVisibleItems = 0;
    bool responsiveElisionEnabled = false;
    int effectiveVisibleLimit = -1;
    QHBoxLayout* layout = nullptr;
    QList<QToolButton*> buttons;
    QList<int> buttonIndexes;
    QList<QLabel*> separators;
    QLineEdit* locationEdit = nullptr;
    QPointer<QWidget> previousFocus;
};

namespace {

class ElidingBreadcrumbButton final : public QToolButton
{
public:
    explicit ElidingBreadcrumbButton(QWidget* parent) : QToolButton(parent)
    {
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    }

    QSize minimumSizeHint() const override
    {
        return QSize(24 + (icon().isNull() ? 0 : iconSize().width()), sizeHint().height());
    }

protected:
    void mousePressEvent(QMouseEvent* event) override
    {
        m_pressPosition = event->pos();
        QToolButton::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent* event) override
    {
        const QUrl url = property("breadcrumbUrl").toUrl();
        if (property("breadcrumbDragEnabled").toBool() && !url.isEmpty()
            && event->buttons().testFlag(Qt::LeftButton)
            && (event->pos() - m_pressPosition).manhattanLength() >= QApplication::startDragDistance()) {
            auto* drag = new QDrag(this);
            auto* mime = new QMimeData;
            mime->setUrls({url});
            drag->setMimeData(mime);
            setDown(false);
            drag->exec(Qt::CopyAction);
            drag->deleteLater();
            return;
        }
        QToolButton::mouseMoveEvent(event);
    }

    void paintEvent(QPaintEvent*) override
    {
        QStyleOptionToolButton option;
        initStyleOption(&option);
        const int iconWidth = icon().isNull() ? 0 : iconSize().width() + 4;
        option.text = fontMetrics().elidedText(text(), Qt::ElideMiddle, qMax(0, width() - 16 - iconWidth));
        QStylePainter painter(this);
        painter.drawComplexControl(QStyle::CC_ToolButton, option);
    }

private:
    QPoint m_pressPosition;
};

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
    if (currentIndex >= 0
        && currentIndex < itemCount) {
        selected.insert(currentIndex);
    }
    if (selected.size() < normalizedLimit) { selected.insert(0); }
    if (selected.size() < normalizedLimit) { selected.insert(itemCount - 1); }

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
    const QHash<int, QIcon>& icons,
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

    if (visibleIndexes.first() > 0) { width += overflowWidth + separatorWidth; }
    if (visibleIndexes.last() < items.size() - 1) { width += overflowWidth + separatorWidth; }

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
        if (!icons.value(index).isNull()) { width += 24; }
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
    d_ptr->locationEdit = new QLineEdit(this);
    d_ptr->locationEdit->setObjectName(QStringLiteral("QtMaterialBreadcrumbLocationEdit"));
    d_ptr->locationEdit->setAccessibleName(tr("Location"));
    d_ptr->locationEdit->setAccessibleDescription(tr("Enter submits the location; Escape cancels editing."));
    d_ptr->locationEdit->installEventFilter(this);
    d_ptr->locationEdit->hide();
    connect(d_ptr->locationEdit, &QLineEdit::returnPressed, this, [this]() { finishLocationEditing(true); });
    auto* editShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_L), this);
    editShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    connect(editShortcut, &QShortcut::activated, this, [this]() { setEditingLocation(true); });
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
    d_ptr->icons.clear();
    d_ptr->urls.clear();
    d_ptr->customLocation = false;
    if (d_ptr->editingLocation) { finishLocationEditing(false); }
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
    d_ptr->customLocation = false;
    d_ptr->currentIndex = d_ptr->items.size() - 1;
    rebuild();
    Q_EMIT currentIndexChanged(d_ptr->currentIndex);
}

void QtMaterialBreadcrumb::clear()
{
    setItems({});
}

QIcon QtMaterialBreadcrumb::itemIcon(int index) const { return d_ptr->icons.value(index); }
void QtMaterialBreadcrumb::setItemIcon(int index, const QIcon& icon)
{
    if (index < 0 || index >= d_ptr->items.size()) { return; }
    if (icon.isNull()) { d_ptr->icons.remove(index); }
    else { d_ptr->icons.insert(index, icon); }
    rebuild();
}

QUrl QtMaterialBreadcrumb::itemUrl(int index) const { return d_ptr->urls.value(index); }
void QtMaterialBreadcrumb::setItemUrl(int index, const QUrl& url)
{
    if (index < 0 || index >= d_ptr->items.size()) { return; }
    if (url.isEmpty()) { d_ptr->urls.remove(index); }
    else { d_ptr->urls.insert(index, url); }
    rebuild();
}
bool QtMaterialBreadcrumb::dragDropEnabled() const noexcept { return d_ptr->dragDropEnabled; }
void QtMaterialBreadcrumb::setDragDropEnabled(bool enabled)
{
    d_ptr->dragDropEnabled = enabled;
    setAcceptDrops(enabled);
    rebuild();
}

bool QtMaterialBreadcrumb::isLocationEditable() const noexcept { return d_ptr->locationEditable; }
void QtMaterialBreadcrumb::setLocationEditable(bool editable)
{
    d_ptr->locationEditable = editable;
    if (!editable && isEditingLocation()) { finishLocationEditing(false); }
    setFocusPolicy(editable ? Qt::StrongFocus : Qt::NoFocus);
}
bool QtMaterialBreadcrumb::isEditingLocation() const noexcept { return d_ptr->editingLocation; }
void QtMaterialBreadcrumb::setEditingLocation(bool editing)
{
    if (editing == d_ptr->editingLocation || (editing && !isLocationEditable())) { return; }
    if (!editing) { finishLocationEditing(false); return; }
    d_ptr->previousFocus = window()->focusWidget();
    d_ptr->editingLocation = true;
    d_ptr->locationEdit->setText(location());
    rebuild();
    d_ptr->locationEdit->setFocus(Qt::ShortcutFocusReason);
    d_ptr->locationEdit->selectAll();
    Q_EMIT editingLocationChanged(true);
}
QString QtMaterialBreadcrumb::location() const
{
    return d_ptr->customLocation ? d_ptr->location : d_ptr->items.mid(0, d_ptr->currentIndex + 1).join(QStringLiteral(" / "));
}
void QtMaterialBreadcrumb::setLocation(const QString& location)
{
    d_ptr->location = location;
    d_ptr->customLocation = true;
    if (isEditingLocation()) { d_ptr->locationEdit->setText(location); }
}
void QtMaterialBreadcrumb::finishLocationEditing(bool submit)
{
    if (!isEditingLocation()) { return; }
    const QString text = d_ptr->locationEdit->text();
    const QPointer<QWidget> previousFocus = d_ptr->previousFocus;
    d_ptr->editingLocation = false;
    rebuild();
    if (previousFocus && previousFocus->isVisible() && previousFocus->isEnabled()) {
        previousFocus->setFocus(Qt::OtherFocusReason);
    } else { setFocus(Qt::OtherFocusReason); }
    Q_EMIT editingLocationChanged(false);
    // Applications validate and resolve the address before updating the path.
    if (submit) { Q_EMIT locationSubmitted(text); }
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
                d_ptr->icons,
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
    int focusedIndex = -1;
    for (int i = 0; i < d_ptr->buttons.size(); ++i) {
        if (d_ptr->buttons.at(i)->hasFocus()) { focusedIndex = d_ptr->buttonIndexes.at(i); }
    }
    d_ptr->buttons.clear();
    d_ptr->buttonIndexes.clear();
    d_ptr->separators.clear();

    while (QLayoutItem* item = d_ptr->layout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            widget->hide();
            if (widget != d_ptr->locationEdit) { widget->deleteLater(); }
        }
        delete item;
    }

    if (d_ptr->editingLocation) {
        d_ptr->layout->addWidget(d_ptr->locationEdit);
        d_ptr->locationEdit->show();
        return;
    }
    d_ptr->locationEdit->hide();

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
                menu->addAction(d_ptr->icons.value(index), d_ptr->items.at(index));
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
        auto* button = new ElidingBreadcrumbButton(this);
        button->setText(text);
        button->setIcon(d_ptr->icons.value(index));
        button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        button->setToolTip(text);
        button->setProperty("breadcrumbIndex", index);
        button->setProperty("breadcrumbUrl", d_ptr->urls.value(index));
        button->setProperty("breadcrumbDragEnabled", d_ptr->dragDropEnabled);
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
    if (focusedIndex >= 0) {
        bool restored = false;
        for (int i = 0; i < d_ptr->buttons.size(); ++i) {
            if (d_ptr->buttonIndexes.at(i) == focusedIndex && d_ptr->buttons.at(i)->isEnabled()) {
                d_ptr->buttons.at(i)->setFocus(Qt::OtherFocusReason);
                restored = true;
                break;
            }
        }
        if (!restored) {
            const auto overflowButtons = findChildren<QToolButton*>();
            for (auto* button : overflowButtons) {
                if (!button->isHidden() && button->menu()) { button->setFocus(Qt::OtherFocusReason); break; }
            }
        }
    }
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
    if (event && (event->type() == QEvent::FontChange || event->type() == QEvent::StyleChange)) { rebuild(); }
}

bool QtMaterialBreadcrumb::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == d_ptr->locationEdit && event->type() == QEvent::KeyPress
        && static_cast<QKeyEvent*>(event)->key() == Qt::Key_Escape) {
        finishLocationEditing(false);
        return true;
    }
    return QWidget::eventFilter(watched, event);
}

void QtMaterialBreadcrumb::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && isLocationEditable()) {
        setEditingLocation(true);
        event->accept();
        return;
    }
    QWidget::mouseDoubleClickEvent(event);
}

void QtMaterialBreadcrumb::dragEnterEvent(QDragEnterEvent* event)
{
    if (dragDropEnabled() && !isEditingLocation() && event->mimeData()->hasUrls()
        && event->possibleActions().testFlag(Qt::CopyAction)) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    } else { event->ignore(); }
}
void QtMaterialBreadcrumb::dragMoveEvent(QDragMoveEvent* event)
{
    if (dragDropEnabled() && !isEditingLocation() && event->mimeData()->hasUrls()
        && event->possibleActions().testFlag(Qt::CopyAction)) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    } else { event->ignore(); }
}
void QtMaterialBreadcrumb::dropEvent(QDropEvent* event)
{
    if (!dragDropEnabled() || isEditingLocation() || !event->mimeData()->hasUrls()
        || !event->possibleActions().testFlag(Qt::CopyAction)) { event->ignore(); return; }
    int index = currentIndex();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QWidget* target = childAt(event->position().toPoint());
#else
    QWidget* target = childAt(event->pos());
#endif
    while (target && target != this) {
        if (target->property("breadcrumbIndex").isValid()) {
            index = target->property("breadcrumbIndex").toInt();
            break;
        }
        target = target->parentWidget();
    }
    event->setDropAction(Qt::CopyAction);
    event->accept();
    Q_EMIT urlsDropped(index, event->mimeData()->urls());
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
