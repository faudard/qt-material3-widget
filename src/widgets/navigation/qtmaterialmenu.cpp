#include "qtmaterial/widgets/navigation/qtmaterialmenu.h"

#include <QChar>
#include <QAccessible>
#include <QAccessibleEvent>
#include <QEvent>
#include <QFocusEvent>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QShowEvent>
#include <QStringList>

#include "qtmaterial/core/qtmaterialthemecontextbinding.h"
#include "qtmaterial/effects/qtmaterialelevationrenderer.h"
#include "qtmaterial/specs/qtmaterialmenuspecresolver.h"

using QtMaterial::MenuSpec;
using QtMaterial::MenuSpecResolver;
using QtMaterial::QtMaterialThemeContextBinding;
using QtMaterial::Theme;
using QtMaterial::ThemeContext;

class QtMaterialMenuPrivate
{
public:
    QVector<QtMaterialMenu::Item> items;
    MenuSpec spec;
    QtMaterialThemeContextBinding* themeBinding = nullptr;
    int currentIndex = -1;
    int pressedIndex = -1;
    bool hasExplicitSpec = false;
    QString lastAccessibilitySummary;
};

namespace {

bool validIndex(
    const QtMaterialMenuPrivate* d,
    int index) noexcept
{
    return d
        && index >= 0
        && index < d->items.size();
}

bool activatable(
    const QtMaterialMenuPrivate* d,
    int index)
{
    return validIndex(d, index)
        && d->items.at(index).role
            == QtMaterialMenu::ItemRole::Action
        && d->items.at(index).enabled;
}

int firstActivatable(const QtMaterialMenuPrivate* d)
{
    if (!d) {
        return -1;
    }
    for (int index = 0; index < d->items.size(); ++index) {
        if (activatable(d, index)) {
            return index;
        }
    }
    return -1;
}

int lastActivatable(const QtMaterialMenuPrivate* d)
{
    if (!d) {
        return -1;
    }
    for (int index = d->items.size() - 1; index >= 0; --index) {
        if (activatable(d, index)) {
            return index;
        }
    }
    return -1;
}

int nextActivatable(
    const QtMaterialMenuPrivate* d,
    int start,
    int step)
{
    if (!d || d->items.isEmpty()) {
        return -1;
    }
    if (step == 0) {
        return activatable(d, start)
            ? start
            : firstActivatable(d);
    }

    int index = start;
    if (!validIndex(d, index)) {
        index = step > 0 ? -1 : d->items.size();
    }

    for (int scanned = 0;
         scanned < d->items.size();
         ++scanned) {
        index += step;
        if (index < 0) {
            index = d->items.size() - 1;
        } else if (index >= d->items.size()) {
            index = 0;
        }
        if (activatable(d, index)) {
            return index;
        }
    }
    return -1;
}

QColor withOpacity(
    QColor color,
    qreal opacity)
{
    color.setAlphaF(qBound(0.0, opacity, 1.0));
    return color;
}

int typeAheadMatch(
    const QtMaterialMenuPrivate* d,
    int current,
    const QString& prefix)
{
    if (!d || prefix.isEmpty() || d->items.isEmpty()) {
        return -1;
    }

    int index = validIndex(d, current) ? current : -1;
    for (int scanned = 0;
         scanned < d->items.size();
         ++scanned) {
        index = (index + 1) % d->items.size();
        if (!activatable(d, index)) {
            continue;
        }
        if (d->items.at(index).text.startsWith(
                prefix,
                Qt::CaseInsensitive)) {
            return index;
        }
    }
    return -1;
}

} // namespace

QtMaterialMenu::QtMaterialMenu(QWidget* parent)
    : QWidget(parent)
    , d_ptr(std::make_unique<QtMaterialMenuPrivate>())
{
    d_ptr->themeBinding =
        new QtMaterialThemeContextBinding(this, this);
    connect(
        d_ptr->themeBinding,
        &QtMaterialThemeContextBinding::
            effectiveThemeContextChanged,
        this,
        &QtMaterialMenu::effectiveThemeContextChanged);
    connect(
        d_ptr->themeBinding,
        &QtMaterialThemeContextBinding::themeChanged,
        this,
        [this](const Theme&) {
            if (!d_ptr->hasExplicitSpec) {
                resolveThemeSpec();
            }
        });

    setAttribute(Qt::WA_Hover, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setAccessibleName(tr("Menu"));

    resolveThemeSpec();
    updateAccessibility();
}

QtMaterialMenu::~QtMaterialMenu() = default;

void QtMaterialMenu::setThemeContext(ThemeContext* context)
{
    d_ptr->themeBinding->setThemeContext(context);
}

ThemeContext* QtMaterialMenu::themeContext() const noexcept
{
    return d_ptr->themeBinding->themeContext();
}

ThemeContext*
QtMaterialMenu::effectiveThemeContext() const noexcept
{
    return d_ptr->themeBinding->effectiveThemeContext();
}

MenuSpec QtMaterialMenu::spec() const
{
    return d_ptr->spec;
}

const MenuSpec& QtMaterialMenu::resolvedSpec() const noexcept
{
    return d_ptr->spec;
}

void QtMaterialMenu::setSpec(const MenuSpec& spec)
{
    d_ptr->hasExplicitSpec = true;
    d_ptr->spec = spec;
    applySpec();
}

bool QtMaterialMenu::hasExplicitSpec() const noexcept
{
    return d_ptr->hasExplicitSpec;
}

void QtMaterialMenu::resetSpec()
{
    d_ptr->hasExplicitSpec = false;
    resolveThemeSpec();
}

int QtMaterialMenu::addItem(
    const QString& text,
    const QIcon& icon)
{
    Item item;
    item.text = text;
    item.icon = icon;

    const int index = d_ptr->items.size();
    d_ptr->items.append(item);
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

    const int index = d_ptr->items.size();
    d_ptr->items.append(item);
    updateGeometry();
    updateAccessibility();
    update();
    return index;
}

void QtMaterialMenu::clear()
{
    if (d_ptr->items.isEmpty()) {
        return;
    }

    d_ptr->items.clear();
    d_ptr->pressedIndex = -1;
    setCurrentIndex(-1);
    updateGeometry();
    updateAccessibility();
    update();
}

int QtMaterialMenu::count() const noexcept
{
    return d_ptr->items.size();
}

bool QtMaterialMenu::isEmpty() const noexcept
{
    return d_ptr->items.isEmpty();
}

QString QtMaterialMenu::itemText(int index) const
{
    return validIndex(d_ptr.get(), index)
        ? d_ptr->items.at(index).text
        : QString();
}

void QtMaterialMenu::setItemText(
    int index,
    const QString& text)
{
    if (!validIndex(d_ptr.get(), index)
        || d_ptr->items[index].text == text) {
        return;
    }

    d_ptr->items[index].text = text;
    updateGeometry();
    updateAccessibility();
    update();
}

QString QtMaterialMenu::itemShortcutText(int index) const
{
    return validIndex(d_ptr.get(), index)
        ? d_ptr->items.at(index).shortcutText
        : QString();
}

void QtMaterialMenu::setItemShortcutText(
    int index,
    const QString& shortcutText)
{
    if (!validIndex(d_ptr.get(), index)
        || d_ptr->items[index].role == ItemRole::Separator
        || d_ptr->items[index].shortcutText == shortcutText) {
        return;
    }

    d_ptr->items[index].shortcutText = shortcutText;
    updateGeometry();
    updateAccessibility();
    update();
}

QIcon QtMaterialMenu::itemIcon(int index) const
{
    return validIndex(d_ptr.get(), index)
        ? d_ptr->items.at(index).icon
        : QIcon();
}

void QtMaterialMenu::setItemIcon(
    int index,
    const QIcon& icon)
{
    if (!validIndex(d_ptr.get(), index)) {
        return;
    }

    d_ptr->items[index].icon = icon;
    updateGeometry();
    updateAccessibility();
    update();
}

bool QtMaterialMenu::isSeparator(int index) const
{
    return validIndex(d_ptr.get(), index)
        && d_ptr->items.at(index).role
            == ItemRole::Separator;
}

bool QtMaterialMenu::isItemEnabled(int index) const
{
    return activatable(d_ptr.get(), index);
}

void QtMaterialMenu::setItemEnabled(
    int index,
    bool enabled)
{
    if (!validIndex(d_ptr.get(), index)
        || isSeparator(index)
        || d_ptr->items[index].enabled == enabled) {
        return;
    }

    d_ptr->items[index].enabled = enabled;
    if (!activatable(
            d_ptr.get(),
            d_ptr->currentIndex)) {
        setCurrentIndex(
            nextActivatable(
                d_ptr.get(),
                index,
                enabled ? 0 : 1));
    }
    updateAccessibility();
    update();
}

bool QtMaterialMenu::isItemCheckable(int index) const
{
    return validIndex(d_ptr.get(), index)
        && d_ptr->items.at(index).checkable;
}

void QtMaterialMenu::setItemCheckable(
    int index,
    bool checkable)
{
    if (!validIndex(d_ptr.get(), index)
        || isSeparator(index)
        || d_ptr->items[index].checkable == checkable) {
        return;
    }

    d_ptr->items[index].checkable = checkable;
    if (!checkable) {
        d_ptr->items[index].checked = false;
    }
    updateAccessibility();
    update();
}

bool QtMaterialMenu::isItemChecked(int index) const
{
    return validIndex(d_ptr.get(), index)
        && d_ptr->items.at(index).checked;
}

void QtMaterialMenu::setItemChecked(
    int index,
    bool checked)
{
    if (!validIndex(d_ptr.get(), index)
        || isSeparator(index)
        || !d_ptr->items[index].checkable
        || d_ptr->items[index].checked == checked) {
        return;
    }

    d_ptr->items[index].checked = checked;
    updateAccessibility();
    update();
}

int QtMaterialMenu::currentIndex() const noexcept
{
    return d_ptr->currentIndex;
}

void QtMaterialMenu::setCurrentIndex(int index)
{
    if (index != -1
        && !activatable(d_ptr.get(), index)) {
        index = nextActivatable(
            d_ptr.get(),
            index,
            1);
    }
    if (d_ptr->currentIndex == index) {
        return;
    }

    d_ptr->currentIndex = index;
    Q_EMIT currentIndexChanged(index);
    updateAccessibility();
    update();
}

int QtMaterialMenu::itemAt(
    const QPoint& position) const
{
    for (int index = 0;
         index < d_ptr->items.size();
         ++index) {
        if (itemRect(index).contains(position)) {
            return index;
        }
    }
    return -1;
}

QRect QtMaterialMenu::itemRect(int index) const
{
    if (!validIndex(d_ptr.get(), index)) {
        return {};
    }

    int y = d_ptr->spec.verticalPadding;
    for (int preceding = 0;
         preceding < index;
         ++preceding) {
        y += isSeparator(preceding)
            ? d_ptr->spec.separatorHeight
            : d_ptr->spec.minItemSize.height();
    }

    const int height = isSeparator(index)
        ? d_ptr->spec.separatorHeight
        : d_ptr->spec.minItemSize.height();
    return QRect(
        0,
        y,
        width(),
        height);
}

QString QtMaterialMenu::itemAccessibleText(int index) const
{
    if (!validIndex(d_ptr.get(), index)) {
        return {};
    }

    const Item& item = d_ptr->items.at(index);
    if (item.role == ItemRole::Separator) {
        return tr("Separator");
    }

    QStringList parts;
    parts << item.text;
    if (!item.shortcutText.isEmpty()) {
        parts << tr("shortcut %1").arg(item.shortcutText);
    }
    if (!item.enabled) {
        parts << tr("disabled");
    }
    if (item.checkable) {
        parts << (item.checked
            ? tr("checked")
            : tr("not checked"));
    }
    if (index == d_ptr->currentIndex) {
        parts << tr("focused");
    }
    return parts.join(QStringLiteral(", "));
}

QString QtMaterialMenu::accessibilitySummary() const
{
    int activatableCount = 0;
    for (int index = 0;
         index < d_ptr->items.size();
         ++index) {
        if (activatable(d_ptr.get(), index)) {
            ++activatableCount;
        }
    }

    QString summary =
        tr("Menu, %n item(s)", nullptr, activatableCount);
    if (d_ptr->currentIndex >= 0) {
        summary += QStringLiteral(", ")
            + itemAccessibleText(d_ptr->currentIndex);
    } else if (d_ptr->items.isEmpty()) {
        summary += QStringLiteral(", ") + tr("empty");
    }
    return summary;
}

QSize QtMaterialMenu::sizeHint() const
{
    const QFontMetrics labelMetrics(d_ptr->spec.labelFont);
    const QFontMetrics shortcutMetrics(
        d_ptr->spec.shortcutFont);

    int labelWidth = 0;
    int shortcutWidth = 0;
    bool hasIcon = false;
    bool hasCheckable = false;
    int height = d_ptr->spec.verticalPadding * 2;

    for (const Item& item : d_ptr->items) {
        if (item.role == ItemRole::Separator) {
            height += d_ptr->spec.separatorHeight;
            continue;
        }

        labelWidth = qMax(
            labelWidth,
            labelMetrics.horizontalAdvance(item.text));
        shortcutWidth = qMax(
            shortcutWidth,
            shortcutMetrics.horizontalAdvance(
                item.shortcutText));
        hasIcon = hasIcon || !item.icon.isNull();
        hasCheckable =
            hasCheckable || item.checkable;
        height += d_ptr->spec.minItemSize.height();
    }

    int width =
        d_ptr->spec.itemPadding.left()
        + d_ptr->spec.itemPadding.right()
        + labelWidth;
    if (hasCheckable) {
        width += d_ptr->spec.checkColumnWidth;
    }
    if (hasIcon) {
        width += d_ptr->spec.iconSize
            + d_ptr->spec.iconSpacing;
    }
    if (shortcutWidth > 0) {
        width += d_ptr->spec.shortcutSpacing
            + shortcutWidth;
    }

    return QSize(
        qBound(
            d_ptr->spec.minItemSize.width(),
            width,
            d_ptr->spec.maxPopupSize.width()),
        qMin(
            qMax(
                d_ptr->spec.minItemSize.height()
                    + d_ptr->spec.verticalPadding * 2,
                height),
            d_ptr->spec.maxPopupSize.height()));
}

QSize QtMaterialMenu::minimumSizeHint() const
{
    return QSize(
        d_ptr->spec.minItemSize.width(),
        d_ptr->spec.minItemSize.height()
            + d_ptr->spec.verticalPadding * 2);
}

void QtMaterialMenu::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRectF containerRect =
        QRectF(rect()).adjusted(1, 1, -1, -1);
    QPainterPath containerPath;
    containerPath.addRoundedRect(
        containerRect,
        d_ptr->spec.cornerRadius,
        d_ptr->spec.cornerRadius);

    QtMaterial::QtMaterialElevationRenderer::
        paintPathElevation(
            &painter,
            containerPath,
            d_ptr->spec.shadowColor,
            d_ptr->spec.elevationStyle);
    painter.fillPath(
        containerPath,
        d_ptr->spec.containerColor);

    const bool rtl =
        layoutDirection() == Qt::RightToLeft;

    for (int index = 0;
         index < d_ptr->items.size();
         ++index) {
        const Item& item = d_ptr->items.at(index);
        const QRect row = itemRect(index);

        if (item.role == ItemRole::Separator) {
            const int y = row.center().y();
            painter.setPen(d_ptr->spec.dividerColor);
            painter.drawLine(
                row.left() + d_ptr->spec.dividerInset,
                y,
                row.right() - d_ptr->spec.dividerInset,
                y);
            continue;
        }

        qreal stateOpacity = 0.0;
        if (index == d_ptr->pressedIndex) {
            stateOpacity =
                d_ptr->spec.pressStateLayerOpacity;
        } else if (index == d_ptr->currentIndex) {
            stateOpacity = hasFocus()
                ? d_ptr->spec.focusStateLayerOpacity
                : d_ptr->spec.hoverStateLayerOpacity;
        }
        if (stateOpacity > 0.0) {
            painter.fillRect(
                row,
                withOpacity(
                    d_ptr->spec.stateLayerColor,
                    stateOpacity));
        }

        int left =
            row.left()
            + d_ptr->spec.itemPadding.left();
        int right =
            row.right()
            - d_ptr->spec.itemPadding.right();

        const QColor labelColor = item.enabled
            ? d_ptr->spec.itemLabelColor
            : d_ptr->spec.disabledItemLabelColor;
        const QColor iconColor = item.enabled
            ? d_ptr->spec.itemIconColor
            : d_ptr->spec.disabledItemIconColor;
        const QColor shortcutColor = item.enabled
            ? d_ptr->spec.shortcutColor
            : d_ptr->spec.disabledShortcutColor;

        if (item.checkable) {
            const QRect checkRect(
                rtl
                    ? right - d_ptr->spec.checkColumnWidth
                    : left,
                row.top(),
                d_ptr->spec.checkColumnWidth,
                row.height());
            if (item.checked) {
                painter.setFont(d_ptr->spec.labelFont);
                painter.setPen(
                    d_ptr->spec.checkedIndicatorColor);
                painter.drawText(
                    checkRect,
                    Qt::AlignCenter,
                    QString(QChar(0x2713)));
            }
            if (rtl) {
                right -= d_ptr->spec.checkColumnWidth;
            } else {
                left += d_ptr->spec.checkColumnWidth;
            }
        }

        if (!item.icon.isNull()) {
            const QRect iconRect(
                rtl ? right - d_ptr->spec.iconSize : left,
                row.center().y()
                    - d_ptr->spec.iconSize / 2,
                d_ptr->spec.iconSize,
                d_ptr->spec.iconSize);
            QIcon icon = item.icon;
            QPixmap pixmap = icon.pixmap(
                QSize(
                    d_ptr->spec.iconSize,
                    d_ptr->spec.iconSize),
                item.enabled
                    ? QIcon::Normal
                    : QIcon::Disabled);
            if (!pixmap.isNull()) {
                QPainter iconPainter(&pixmap);
                iconPainter.setCompositionMode(
                    QPainter::CompositionMode_SourceIn);
                iconPainter.fillRect(
                    pixmap.rect(),
                    iconColor);
                iconPainter.end();
                painter.drawPixmap(iconRect, pixmap);
            }
            if (rtl) {
                right -= d_ptr->spec.iconSize
                    + d_ptr->spec.iconSpacing;
            } else {
                left += d_ptr->spec.iconSize
                    + d_ptr->spec.iconSpacing;
            }
        }

        if (!item.shortcutText.isEmpty()) {
            painter.setFont(d_ptr->spec.shortcutFont);
            painter.setPen(shortcutColor);
            const QFontMetrics shortcutMetrics(
                d_ptr->spec.shortcutFont);
            const int shortcutWidth =
                shortcutMetrics.horizontalAdvance(
                    item.shortcutText);
            const QRect shortcutRect(
                rtl ? left : right - shortcutWidth,
                row.top(),
                shortcutWidth,
                row.height());
            painter.drawText(
                shortcutRect,
                Qt::AlignVCenter
                    | (rtl ? Qt::AlignLeft : Qt::AlignRight),
                item.shortcutText);
            if (rtl) {
                left += shortcutWidth
                    + d_ptr->spec.shortcutSpacing;
            } else {
                right -= shortcutWidth
                    + d_ptr->spec.shortcutSpacing;
            }
        }

        painter.setFont(d_ptr->spec.labelFont);
        painter.setPen(labelColor);
        const QRect labelRect(
            left,
            row.top(),
            qMax(0, right - left),
            row.height());
        painter.drawText(
            labelRect,
            Qt::AlignVCenter
                | (rtl ? Qt::AlignRight : Qt::AlignLeft),
            item.text);
    }

    if (hasFocus()) {
        painter.setBrush(Qt::NoBrush);
        painter.setPen(
            QPen(
                d_ptr->spec.focusRingColor,
                d_ptr->spec.focusRingWidth));
        painter.drawRoundedRect(
            containerRect.adjusted(
                d_ptr->spec.focusRingWidth,
                d_ptr->spec.focusRingWidth,
                -d_ptr->spec.focusRingWidth,
                -d_ptr->spec.focusRingWidth),
            qMax<qreal>(
                0.0,
                d_ptr->spec.cornerRadius
                    - d_ptr->spec.focusRingWidth),
            qMax<qreal>(
                0.0,
                d_ptr->spec.cornerRadius
                    - d_ptr->spec.focusRingWidth));
    }
}

void QtMaterialMenu::mouseMoveEvent(QMouseEvent* event)
{
    const int index = itemAt(event->pos());
    if (activatable(d_ptr.get(), index)) {
        setCurrentIndex(index);
    }
    QWidget::mouseMoveEvent(event);
}

void QtMaterialMenu::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        const int index = itemAt(event->pos());
        if (activatable(d_ptr.get(), index)) {
            d_ptr->pressedIndex = index;
            setCurrentIndex(index);
            update();
        }
    }
    QWidget::mousePressEvent(event);
}

void QtMaterialMenu::mouseReleaseEvent(QMouseEvent* event)
{
    const int pressed = d_ptr->pressedIndex;
    d_ptr->pressedIndex = -1;

    if (event->button() == Qt::LeftButton
        && pressed >= 0
        && pressed == itemAt(event->pos())) {
        activateIndex(pressed);
    }

    update();
    QWidget::mouseReleaseEvent(event);
}

void QtMaterialMenu::leaveEvent(QEvent* event)
{
    d_ptr->pressedIndex = -1;
    update();
    QWidget::leaveEvent(event);
}

void QtMaterialMenu::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Down:
    case Qt::Key_Right:
        setCurrentIndex(
            nextActivatable(
                d_ptr.get(),
                d_ptr->currentIndex,
                1));
        event->accept();
        return;
    case Qt::Key_Up:
    case Qt::Key_Left:
        setCurrentIndex(
            nextActivatable(
                d_ptr.get(),
                d_ptr->currentIndex,
                -1));
        event->accept();
        return;
    case Qt::Key_Home:
        setCurrentIndex(firstActivatable(d_ptr.get()));
        event->accept();
        return;
    case Qt::Key_End:
        setCurrentIndex(lastActivatable(d_ptr.get()));
        event->accept();
        return;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space:
        activateIndex(d_ptr->currentIndex);
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

    const QString text = event->text().trimmed();
    if (!text.isEmpty()
        && !event->modifiers().testFlag(Qt::ControlModifier)
        && !event->modifiers().testFlag(Qt::AltModifier)
        && !event->modifiers().testFlag(Qt::MetaModifier)) {
        const int match = typeAheadMatch(
            d_ptr.get(),
            d_ptr->currentIndex,
            text);
        if (match >= 0) {
            setCurrentIndex(match);
            event->accept();
            return;
        }
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
    d_ptr->pressedIndex = -1;
    updateAccessibility();
    update();
    QWidget::focusOutEvent(event);
}

void QtMaterialMenu::showEvent(QShowEvent* event)
{
    ensureCurrentIndex();
    updateAccessibility();
    QWidget::showEvent(event);
}

void QtMaterialMenu::changeEvent(QEvent* event)
{
    QWidget::changeEvent(event);
    if (event && event->type() == QEvent::LanguageChange) {
        setAccessibleName(tr("Menu"));
        updateAccessibility();
    }
}

void QtMaterialMenu::resolveThemeSpec()
{
    if (d_ptr->hasExplicitSpec) {
        return;
    }

    d_ptr->spec =
        MenuSpecResolver().menuSpec(
            d_ptr->themeBinding->theme());
    applySpec();
}

void QtMaterialMenu::applySpec()
{
    setFont(d_ptr->spec.labelFont);
    setMaximumSize(d_ptr->spec.maxPopupSize);
    updateGeometry();
    update();
}

void QtMaterialMenu::ensureCurrentIndex()
{
    if (!activatable(
            d_ptr.get(),
            d_ptr->currentIndex)) {
        setCurrentIndex(firstActivatable(d_ptr.get()));
    }
}

void QtMaterialMenu::activateIndex(int index)
{
    if (!activatable(d_ptr.get(), index)) {
        return;
    }

    if (d_ptr->items[index].checkable) {
        d_ptr->items[index].checked =
            !d_ptr->items[index].checked;
        updateAccessibility();
        update();
    }

    Q_EMIT activated(index);
}

void QtMaterialMenu::updateAccessibility()
{
    if (accessibleName().isEmpty()) {
        setAccessibleName(tr("Menu"));
    }

    const QString summary = accessibilitySummary();
    setAccessibleDescription(summary);

    if (d_ptr->lastAccessibilitySummary != summary) {
        d_ptr->lastAccessibilitySummary = summary;
        Q_EMIT accessibilitySummaryChanged(summary);
    }

    if (QAccessible::isActive()) {
        QAccessibleEvent accessibilityEvent(
            this,
            QAccessible::DescriptionChanged);
        QAccessible::updateAccessibility(
            &accessibilityEvent);
    }
}
