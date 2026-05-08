#include "qtmaterial/widgets/navigation/qtmaterialmenu.h"

#include <QAccessible>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <QPaintEvent>
#include <memory>

struct QtMaterialMenuPrivate
{
    QVector<QtMaterialMenu::Item> m_items;
    int m_currentIndex = -1;
    int m_pressedIndex = -1;
    QString m_lastAccessibilitySummary;
};


namespace {
constexpr int kHorizontalPadding = 12;
constexpr int kVerticalPadding = 8;
constexpr int kItemHeight = 48;
constexpr int kSeparatorHeight = 8;
constexpr int kMinWidth = 112;
constexpr int kIconSize = 24;
constexpr int kIconGap = 12;
constexpr int kCheckColumnWidth = 24;
constexpr int kCornerRadius = 4;

QColor withAlpha(QColor color, int alpha)
{
    color.setAlpha(alpha);
    return color;
}

// Tranche 28 PIMPL-local menu helpers
bool isValidIndex(const QtMaterialMenuPrivate* d, int index) noexcept
{
    return d != nullptr && index >= 0 && index < d->m_items.size();
}

bool isActivatableIndex(const QtMaterialMenuPrivate* d, int index)
{
    return isValidIndex(d, index)
           && d->m_items.at(index).role == QtMaterialMenu::ItemRole::Action
           && d->m_items.at(index).enabled;
}

int firstActivatableIndex(const QtMaterialMenuPrivate* d)
{
    if (d == nullptr) {
        return -1;
    }
    for (int i = 0; i < d->m_items.size(); ++i) {
        if (isActivatableIndex(d, i)) {
            return i;
        }
    }
    return -1;
}

int lastActivatableIndex(const QtMaterialMenuPrivate* d)
{
    if (d == nullptr) {
        return -1;
    }
    for (int i = d->m_items.size() - 1; i >= 0; --i) {
        if (isActivatableIndex(d, i)) {
            return i;
        }
    }
    return -1;
}

int nextActivatableIndex(const QtMaterialMenuPrivate* d, int start, int step)
{
    if (d == nullptr || d->m_items.isEmpty()) {
        return -1;
    }
    if (step == 0) {
        return isActivatableIndex(d, start) ? start : firstActivatableIndex(d);
    }

    int index = start;
    if (!isValidIndex(d, index)) {
        index = step > 0 ? -1 : d->m_items.size();
    }

    for (int scanned = 0; scanned < d->m_items.size(); ++scanned) {
        index += step;
        if (index < 0) {
            index = d->m_items.size() - 1;
        } else if (index >= d->m_items.size()) {
            index = 0;
        }
        if (isActivatableIndex(d, index)) {
            return index;
        }
    }
    return -1;
}

void emitAccessibilitySummaryIfChanged(QtMaterialMenu* self, QtMaterialMenuPrivate* d)
{
    if (self == nullptr || d == nullptr) {
        return;
    }
    const QString summary = self->accessibilitySummary();
    if (d->m_lastAccessibilitySummary == summary) {
        return;
    }
    d->m_lastAccessibilitySummary = summary;
    Q_EMIT self->accessibilitySummaryChanged(summary);
}

void updateAccessibility(QtMaterialMenu* self, QtMaterialMenuPrivate* d)
{
    if (self == nullptr || d == nullptr) {
        return;
    }
    if (self->accessibleName().isEmpty()) {
        self->setAccessibleName(QStringLiteral("Menu"));
    }
    self->setAccessibleDescription(self->accessibilitySummary());
    emitAccessibilitySummaryIfChanged(self, d);

    if (QAccessible::isActive()) {
        QAccessibleEvent event(self, QAccessible::DescriptionChanged);
        QAccessible::updateAccessibility(&event);
    }
}

void ensureCurrentIndex(QtMaterialMenu* self, QtMaterialMenuPrivate* d)
{
    if (self == nullptr || d == nullptr) {
        return;
    }
    if (!isActivatableIndex(d, d->m_currentIndex)) {
        self->setCurrentIndex(firstActivatableIndex(d));
    }
}

void activateIndex(QtMaterialMenu* self, QtMaterialMenuPrivate* d, int index)
{
    if (self == nullptr || d == nullptr || !isActivatableIndex(d, index)) {
        return;
    }
    if (d->m_items[index].checkable) {
        d->m_items[index].checked = !d->m_items[index].checked;
        updateAccessibility(self, d);
        self->update();
    }
    Q_EMIT self->activated(index);
}

} // namespace

QtMaterialMenu::QtMaterialMenu(QWidget* parent)
    : QWidget(parent)
{
    d_ptr = std::make_unique<QtMaterialMenuPrivate>();

    setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setAccessibleName(QStringLiteral("Menu"));
    updateAccessibility(this, d_ptr.get());
}

QtMaterialMenu::~QtMaterialMenu() = default;

int QtMaterialMenu::addItem(const QString& text, const QIcon& icon)
{
    Item item;
    item.text = text;
    item.icon = icon;
    const int index = d_ptr->m_items.size();
    d_ptr->m_items.append(item);
    ensureCurrentIndex(this, d_ptr.get());
    updateGeometry();
    updateAccessibility(this, d_ptr.get());
    update();
    return index;
}

int QtMaterialMenu::addSeparator()
{
    Item item;
    item.role = ItemRole::Separator;
    item.enabled = false;
    const int index = d_ptr->m_items.size();
    d_ptr->m_items.append(item);
    updateGeometry();
    updateAccessibility(this, d_ptr.get());
    update();
    return index;
}

void QtMaterialMenu::clear()
{
    if (d_ptr->m_items.isEmpty()) {
        return;
    }

    d_ptr->m_items.clear();
    d_ptr->m_pressedIndex = -1;
    setCurrentIndex(-1);
    updateGeometry();
    updateAccessibility(this, d_ptr.get());
    update();
}

int QtMaterialMenu::count() const noexcept
{
    return d_ptr->m_items.size();
}

bool QtMaterialMenu::isEmpty() const noexcept
{
    return d_ptr->m_items.isEmpty();
}

QString QtMaterialMenu::itemText(int index) const
{
    return isValidIndex(d_ptr.get(), index) ? d_ptr->m_items.at(index).text : QString();
}

void QtMaterialMenu::setItemText(int index, const QString& text)
{
    if (!isValidIndex(d_ptr.get(), index) || d_ptr->m_items[index].text == text) {
        return;
    }

    d_ptr->m_items[index].text = text;
    updateGeometry();
    updateAccessibility(this, d_ptr.get());
    update();
}

QIcon QtMaterialMenu::itemIcon(int index) const
{
    return isValidIndex(d_ptr.get(), index) ? d_ptr->m_items.at(index).icon : QIcon();
}

void QtMaterialMenu::setItemIcon(int index, const QIcon& icon)
{
    if (!isValidIndex(d_ptr.get(), index)) {
        return;
    }

    d_ptr->m_items[index].icon = icon;
    updateGeometry();
    updateAccessibility(this, d_ptr.get());
    update();
}

bool QtMaterialMenu::isSeparator(int index) const
{
    return isValidIndex(d_ptr.get(), index) && d_ptr->m_items.at(index).role == ItemRole::Separator;
}

bool QtMaterialMenu::isItemEnabled(int index) const
{
    return isActivatableIndex(d_ptr.get(), index);
}

void QtMaterialMenu::setItemEnabled(int index, bool enabled)
{
    if (!isValidIndex(d_ptr.get(), index) || isSeparator(index) || d_ptr->m_items[index].enabled == enabled) {
        return;
    }

    d_ptr->m_items[index].enabled = enabled;
    if (!isActivatableIndex(d_ptr.get(), d_ptr->m_currentIndex)) {
        setCurrentIndex(nextActivatableIndex(d_ptr.get(), index, enabled ? 0 : 1));
    }
    updateAccessibility(this, d_ptr.get());
    update();
}

bool QtMaterialMenu::isItemCheckable(int index) const
{
    return isValidIndex(d_ptr.get(), index) && d_ptr->m_items.at(index).checkable;
}

void QtMaterialMenu::setItemCheckable(int index, bool checkable)
{
    if (!isValidIndex(d_ptr.get(), index) || isSeparator(index) || d_ptr->m_items[index].checkable == checkable) {
        return;
    }

    d_ptr->m_items[index].checkable = checkable;
    if (!checkable) {
        d_ptr->m_items[index].checked = false;
    }
    updateAccessibility(this, d_ptr.get());
    update();
}

bool QtMaterialMenu::isItemChecked(int index) const
{
    return isValidIndex(d_ptr.get(), index) && d_ptr->m_items.at(index).checked;
}

void QtMaterialMenu::setItemChecked(int index, bool checked)
{
    if (!isValidIndex(d_ptr.get(), index) || isSeparator(index) || !d_ptr->m_items[index].checkable || d_ptr->m_items[index].checked == checked) {
        return;
    }

    d_ptr->m_items[index].checked = checked;
    updateAccessibility(this, d_ptr.get());
    update();
}

int QtMaterialMenu::currentIndex() const noexcept
{
    return d_ptr->m_currentIndex;
}

void QtMaterialMenu::setCurrentIndex(int index)
{
    if (index != -1 && !isActivatableIndex(d_ptr.get(), index)) {
        index = nextActivatableIndex(d_ptr.get(), index, 1);
    }

    if (d_ptr->m_currentIndex == index) {
        return;
    }

    d_ptr->m_currentIndex = index;
    Q_EMIT currentIndexChanged(d_ptr->m_currentIndex);
    updateAccessibility(this, d_ptr.get());
    update();
}

int QtMaterialMenu::itemAt(const QPoint& position) const
{
    for (int i = 0; i < d_ptr->m_items.size(); ++i) {
        if (itemRect(i).contains(position)) {
            return i;
        }
    }
    return -1;
}

QRect QtMaterialMenu::itemRect(int index) const
{
    if (!isValidIndex(d_ptr.get(), index)) {
        return {};
    }

    int y = kVerticalPadding;
    for (int i = 0; i < index; ++i) {
        y += isSeparator(i) ? kSeparatorHeight : kItemHeight;
    }

    const int height = isSeparator(index) ? kSeparatorHeight : kItemHeight;
    return QRect(kHorizontalPadding / 2, y, width() - kHorizontalPadding, height);
}

QString QtMaterialMenu::itemAccessibleText(int index) const
{
    if (!isValidIndex(d_ptr.get(), index)) {
        return {};
    }

    const Item& item = d_ptr->m_items.at(index);
    if (item.role == ItemRole::Separator) {
        return QStringLiteral("Separator");
    }

    QStringList parts;
    parts << item.text;
    if (!item.enabled) {
        parts << QStringLiteral("disabled");
    }
    if (item.checkable) {
        parts << (item.checked ? QStringLiteral("checked") : QStringLiteral("not checked"));
    }
    if (index == d_ptr->m_currentIndex) {
        parts << QStringLiteral("focused");
    }
    return parts.join(QStringLiteral(", "));
}

QString QtMaterialMenu::accessibilitySummary() const
{
    const int total = d_ptr->m_items.size();
    int activatable = 0;
    for (int i = 0; i < d_ptr->m_items.size(); ++i) {
        if (isActivatableIndex(d_ptr.get(), i)) {
            ++activatable;
        }
    }

    QString summary = tr("Menu, %n item(s)", nullptr, activatable);
    if (d_ptr->m_currentIndex >= 0) {
        summary += QStringLiteral(", ") + itemAccessibleText(d_ptr->m_currentIndex);
    } else if (total == 0) {
        summary += QStringLiteral(", empty");
    }
    return summary;
}

QSize QtMaterialMenu::sizeHint() const
{
    QFontMetrics fm(font());
    int textWidth = 0;
    bool hasIcon = false;
    bool hasCheckable = false;
    int height = kVerticalPadding * 2;

    for (const Item& item : d_ptr->m_items) {
        if (item.role == ItemRole::Separator) {
            height += kSeparatorHeight;
            continue;
        }
        textWidth = qMax(textWidth, fm.horizontalAdvance(item.text));
        hasIcon = hasIcon || !item.icon.isNull();
        hasCheckable = hasCheckable || item.checkable;
        height += kItemHeight;
    }

    int width = kHorizontalPadding * 2 + textWidth;
    if (hasIcon) {
        width += kIconSize + kIconGap;
    }
    if (hasCheckable) {
        width += kCheckColumnWidth;
    }

    return QSize(qMax(kMinWidth, width), qMax(kItemHeight + kVerticalPadding * 2, height));
}

QSize QtMaterialMenu::minimumSizeHint() const
{
    return QSize(kMinWidth, kItemHeight + kVerticalPadding * 2);
}

void QtMaterialMenu::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPainterPath background;
    background.addRoundedRect(rect().adjusted(0, 0, -1, -1), kCornerRadius, kCornerRadius);
    painter.fillPath(background, palette().window());

    const bool rtl = layoutDirection() == Qt::RightToLeft;
    const QColor textColor = palette().color(QPalette::WindowText);
    const QColor disabledColor = palette().color(QPalette::Disabled, QPalette::WindowText);
    const QColor stateColor = palette().color(QPalette::Highlight);

    for (int i = 0; i < d_ptr->m_items.size(); ++i) {
        const Item& item = d_ptr->m_items.at(i);
        const QRect row = itemRect(i);

        if (item.role == ItemRole::Separator) {
            const int cy = row.center().y();
            painter.setPen(withAlpha(textColor, 40));
            painter.drawLine(row.left() + kHorizontalPadding, cy, row.right() - kHorizontalPadding, cy);
            continue;
        }

        if (i == d_ptr->m_currentIndex || i == d_ptr->m_pressedIndex) {
            const int alpha = (i == d_ptr->m_pressedIndex) ? 38 : 22;
            painter.fillRect(row, withAlpha(stateColor, alpha));
        }

        int left = row.left() + kHorizontalPadding;
        int right = row.right() - kHorizontalPadding;

        const QColor foreground = item.enabled ? textColor : disabledColor;
        painter.setPen(foreground);

        if (item.checkable) {
            const QRect checkRect(rtl ? right - kCheckColumnWidth : left, row.top(), kCheckColumnWidth, row.height());
            if (item.checked) {
                painter.drawText(checkRect, Qt::AlignCenter, QStringLiteral("✓"));
            }
            if (rtl) {
                right -= kCheckColumnWidth;
            } else {
                left += kCheckColumnWidth;
            }
        }

        if (!item.icon.isNull()) {
            const QRect iconRect(rtl ? right - kIconSize : left,
                                 row.center().y() - kIconSize / 2,
                                 kIconSize,
                                 kIconSize);
            item.icon.paint(&painter, iconRect, Qt::AlignCenter, item.enabled ? QIcon::Normal : QIcon::Disabled);
            if (rtl) {
                right -= kIconSize + kIconGap;
            } else {
                left += kIconSize + kIconGap;
            }
        }

        const QRect textRect(left, row.top(), qMax(0, right - left), row.height());
        painter.drawText(textRect, Qt::AlignVCenter | (rtl ? Qt::AlignRight : Qt::AlignLeft), item.text);
    }

    if (hasFocus()) {
        QPen pen(palette().color(QPalette::Highlight));
        pen.setWidth(2);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(rect().adjusted(1, 1, -2, -2), kCornerRadius, kCornerRadius);
    }
}

void QtMaterialMenu::mouseMoveEvent(QMouseEvent* event)
{
    const int index = itemAt(event->pos());
    if (isActivatableIndex(d_ptr.get(), index)) {
        setCurrentIndex(index);
    }
    QWidget::mouseMoveEvent(event);
}

void QtMaterialMenu::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        const int index = itemAt(event->pos());
        if (isActivatableIndex(d_ptr.get(), index)) {
            d_ptr->m_pressedIndex = index;
            setCurrentIndex(index);
            update();
        }
    }
    QWidget::mousePressEvent(event);
}

void QtMaterialMenu::mouseReleaseEvent(QMouseEvent* event)
{
    const int pressed = d_ptr->m_pressedIndex;
    d_ptr->m_pressedIndex = -1;
    if (event->button() == Qt::LeftButton && pressed >= 0 && pressed == itemAt(event->pos())) {
        activateIndex(this, d_ptr.get(), pressed);
    }
    update();
    QWidget::mouseReleaseEvent(event);
}

void QtMaterialMenu::leaveEvent(QEvent* event)
{
    d_ptr->m_pressedIndex = -1;
    update();
    QWidget::leaveEvent(event);
}

void QtMaterialMenu::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Down:
    case Qt::Key_Right:
        setCurrentIndex(nextActivatableIndex(d_ptr.get(), d_ptr->m_currentIndex, 1));
        event->accept();
        return;
    case Qt::Key_Up:
    case Qt::Key_Left:
        setCurrentIndex(nextActivatableIndex(d_ptr.get(), d_ptr->m_currentIndex, -1));
        event->accept();
        return;
    case Qt::Key_Home:
        setCurrentIndex(firstActivatableIndex(d_ptr.get()));
        event->accept();
        return;
    case Qt::Key_End:
        setCurrentIndex(lastActivatableIndex(d_ptr.get()));
        event->accept();
        return;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space:
        activateIndex(this, d_ptr.get(), d_ptr->m_currentIndex);
        event->accept();
        return;
    case Qt::Key_Escape:
        hide();
        Q_EMIT dismissed();
        event->accept();
        return;
    default:
        break;
    }

    QWidget::keyPressEvent(event);
}

void QtMaterialMenu::focusInEvent(QFocusEvent* event)
{
    ensureCurrentIndex(this, d_ptr.get());
    updateAccessibility(this, d_ptr.get());
    update();
    QWidget::focusInEvent(event);
}

void QtMaterialMenu::focusOutEvent(QFocusEvent* event)
{
    updateAccessibility(this, d_ptr.get());
    update();
    QWidget::focusOutEvent(event);
}
