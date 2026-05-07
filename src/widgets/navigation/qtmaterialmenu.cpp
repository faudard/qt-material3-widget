#include "qtmaterial/widgets/navigation/qtmaterialmenu.h"

#include <QAccessible>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <QPaintEvent>

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
} // namespace

QtMaterialMenu::QtMaterialMenu(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setAccessibleName(QStringLiteral("Menu"));
    updateAccessibility();
}

QtMaterialMenu::~QtMaterialMenu() = default;

int QtMaterialMenu::addItem(const QString& text, const QIcon& icon)
{
    Item item;
    item.text = text;
    item.icon = icon;
    const int index = m_items.size();
    m_items.append(item);
    ensureCurrentIndex();
    updateGeometry();
    updateAccessibility();
    update();
    return index;
}

int QtMaterialMenu::addSeparator()
{
    Item item;
    item.role = ItemRole::Separator;
    item.enabled = false;
    const int index = m_items.size();
    m_items.append(item);
    updateGeometry();
    updateAccessibility();
    update();
    return index;
}

void QtMaterialMenu::clear()
{
    if (m_items.isEmpty()) {
        return;
    }

    m_items.clear();
    m_pressedIndex = -1;
    setCurrentIndex(-1);
    updateGeometry();
    updateAccessibility();
    update();
}

int QtMaterialMenu::count() const noexcept
{
    return m_items.size();
}

bool QtMaterialMenu::isEmpty() const noexcept
{
    return m_items.isEmpty();
}

QString QtMaterialMenu::itemText(int index) const
{
    return isValidIndex(index) ? m_items.at(index).text : QString();
}

void QtMaterialMenu::setItemText(int index, const QString& text)
{
    if (!isValidIndex(index) || m_items[index].text == text) {
        return;
    }

    m_items[index].text = text;
    updateGeometry();
    updateAccessibility();
    update();
}

QIcon QtMaterialMenu::itemIcon(int index) const
{
    return isValidIndex(index) ? m_items.at(index).icon : QIcon();
}

void QtMaterialMenu::setItemIcon(int index, const QIcon& icon)
{
    if (!isValidIndex(index)) {
        return;
    }

    m_items[index].icon = icon;
    updateGeometry();
    updateAccessibility();
    update();
}

bool QtMaterialMenu::isSeparator(int index) const
{
    return isValidIndex(index) && m_items.at(index).role == ItemRole::Separator;
}

bool QtMaterialMenu::isItemEnabled(int index) const
{
    return isActivatableIndex(index);
}

void QtMaterialMenu::setItemEnabled(int index, bool enabled)
{
    if (!isValidIndex(index) || isSeparator(index) || m_items[index].enabled == enabled) {
        return;
    }

    m_items[index].enabled = enabled;
    if (!isActivatableIndex(m_currentIndex)) {
        setCurrentIndex(nextActivatableIndex(index, enabled ? 0 : 1));
    }
    updateAccessibility();
    update();
}

bool QtMaterialMenu::isItemCheckable(int index) const
{
    return isValidIndex(index) && m_items.at(index).checkable;
}

void QtMaterialMenu::setItemCheckable(int index, bool checkable)
{
    if (!isValidIndex(index) || isSeparator(index) || m_items[index].checkable == checkable) {
        return;
    }

    m_items[index].checkable = checkable;
    if (!checkable) {
        m_items[index].checked = false;
    }
    updateAccessibility();
    update();
}

bool QtMaterialMenu::isItemChecked(int index) const
{
    return isValidIndex(index) && m_items.at(index).checked;
}

void QtMaterialMenu::setItemChecked(int index, bool checked)
{
    if (!isValidIndex(index) || isSeparator(index) || !m_items[index].checkable || m_items[index].checked == checked) {
        return;
    }

    m_items[index].checked = checked;
    updateAccessibility();
    update();
}

int QtMaterialMenu::currentIndex() const noexcept
{
    return m_currentIndex;
}

void QtMaterialMenu::setCurrentIndex(int index)
{
    if (index != -1 && !isActivatableIndex(index)) {
        index = nextActivatableIndex(index, 1);
    }

    if (m_currentIndex == index) {
        return;
    }

    m_currentIndex = index;
    Q_EMIT currentIndexChanged(m_currentIndex);
    updateAccessibility();
    update();
}

int QtMaterialMenu::itemAt(const QPoint& position) const
{
    for (int i = 0; i < m_items.size(); ++i) {
        if (itemRect(i).contains(position)) {
            return i;
        }
    }
    return -1;
}

QRect QtMaterialMenu::itemRect(int index) const
{
    if (!isValidIndex(index)) {
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
    if (!isValidIndex(index)) {
        return {};
    }

    const Item& item = m_items.at(index);
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
    if (index == m_currentIndex) {
        parts << QStringLiteral("focused");
    }
    return parts.join(QStringLiteral(", "));
}

QString QtMaterialMenu::accessibilitySummary() const
{
    const int total = m_items.size();
    int activatable = 0;
    for (int i = 0; i < m_items.size(); ++i) {
        if (isActivatableIndex(i)) {
            ++activatable;
        }
    }

    QString summary = tr("Menu, %n item(s)", nullptr, activatable);
    if (m_currentIndex >= 0) {
        summary += QStringLiteral(", ") + itemAccessibleText(m_currentIndex);
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

    for (const Item& item : m_items) {
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

    for (int i = 0; i < m_items.size(); ++i) {
        const Item& item = m_items.at(i);
        const QRect row = itemRect(i);

        if (item.role == ItemRole::Separator) {
            const int cy = row.center().y();
            painter.setPen(withAlpha(textColor, 40));
            painter.drawLine(row.left() + kHorizontalPadding, cy, row.right() - kHorizontalPadding, cy);
            continue;
        }

        if (i == m_currentIndex || i == m_pressedIndex) {
            const int alpha = (i == m_pressedIndex) ? 38 : 22;
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
    if (isActivatableIndex(index)) {
        setCurrentIndex(index);
    }
    QWidget::mouseMoveEvent(event);
}

void QtMaterialMenu::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        const int index = itemAt(event->pos());
        if (isActivatableIndex(index)) {
            m_pressedIndex = index;
            setCurrentIndex(index);
            update();
        }
    }
    QWidget::mousePressEvent(event);
}

void QtMaterialMenu::mouseReleaseEvent(QMouseEvent* event)
{
    const int pressed = m_pressedIndex;
    m_pressedIndex = -1;
    if (event->button() == Qt::LeftButton && pressed >= 0 && pressed == itemAt(event->pos())) {
        activateIndex(pressed);
    }
    update();
    QWidget::mouseReleaseEvent(event);
}

void QtMaterialMenu::leaveEvent(QEvent* event)
{
    m_pressedIndex = -1;
    update();
    QWidget::leaveEvent(event);
}

void QtMaterialMenu::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Down:
    case Qt::Key_Right:
        setCurrentIndex(nextActivatableIndex(m_currentIndex, 1));
        event->accept();
        return;
    case Qt::Key_Up:
    case Qt::Key_Left:
        setCurrentIndex(nextActivatableIndex(m_currentIndex, -1));
        event->accept();
        return;
    case Qt::Key_Home:
        setCurrentIndex(firstActivatableIndex());
        event->accept();
        return;
    case Qt::Key_End:
        setCurrentIndex(lastActivatableIndex());
        event->accept();
        return;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space:
        activateIndex(m_currentIndex);
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
    ensureCurrentIndex();
    updateAccessibility();
    update();
    QWidget::focusInEvent(event);
}

void QtMaterialMenu::focusOutEvent(QFocusEvent* event)
{
    updateAccessibility();
    update();
    QWidget::focusOutEvent(event);
}

bool QtMaterialMenu::isValidIndex(int index) const noexcept
{
    return index >= 0 && index < m_items.size();
}

bool QtMaterialMenu::isActivatableIndex(int index) const
{
    return isValidIndex(index) && m_items.at(index).role == ItemRole::Action && m_items.at(index).enabled;
}

int QtMaterialMenu::firstActivatableIndex() const
{
    for (int i = 0; i < m_items.size(); ++i) {
        if (isActivatableIndex(i)) {
            return i;
        }
    }
    return -1;
}

int QtMaterialMenu::lastActivatableIndex() const
{
    for (int i = m_items.size() - 1; i >= 0; --i) {
        if (isActivatableIndex(i)) {
            return i;
        }
    }
    return -1;
}

int QtMaterialMenu::nextActivatableIndex(int start, int step) const
{
    if (m_items.isEmpty()) {
        return -1;
    }

    if (step == 0) {
        return isActivatableIndex(start) ? start : firstActivatableIndex();
    }

    int index = start;
    if (!isValidIndex(index)) {
        index = step > 0 ? -1 : m_items.size();
    }

    for (int scanned = 0; scanned < m_items.size(); ++scanned) {
        index += step;
        if (index < 0) {
            index = m_items.size() - 1;
        } else if (index >= m_items.size()) {
            index = 0;
        }

        if (isActivatableIndex(index)) {
            return index;
        }
    }

    return -1;
}

void QtMaterialMenu::activateIndex(int index)
{
    if (!isActivatableIndex(index)) {
        return;
    }

    if (m_items[index].checkable) {
        m_items[index].checked = !m_items[index].checked;
        updateAccessibility();
        update();
    }

    Q_EMIT activated(index);
}

void QtMaterialMenu::ensureCurrentIndex()
{
    if (!isActivatableIndex(m_currentIndex)) {
        setCurrentIndex(firstActivatableIndex());
    }
}

void QtMaterialMenu::updateAccessibility()
{
    if (accessibleName().isEmpty()) {
        setAccessibleName(QStringLiteral("Menu"));
    }
    setAccessibleDescription(accessibilitySummary());
    emitAccessibilitySummaryIfChanged();

    if (QAccessible::isActive()) {
        QAccessibleEvent event(this, QAccessible::DescriptionChanged);
        QAccessible::updateAccessibility(&event);
    }
}

void QtMaterialMenu::emitAccessibilitySummaryIfChanged()
{
    const QString summary = accessibilitySummary();
    if (m_lastAccessibilitySummary == summary) {
        return;
    }

    m_lastAccessibilitySummary = summary;
    Q_EMIT accessibilitySummaryChanged(summary);
}
