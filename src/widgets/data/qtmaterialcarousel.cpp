#include "qtmaterial/widgets/data/qtmaterialcarousel.h"

#include <QApplication>
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOptionFocusRect>

#include <algorithm>
#include <memory>

namespace QtMaterial {

struct QtMaterialCarouselPrivate
{
    QVector<QtMaterialCarousel::Item> m_items;
    int m_currentIndex = -1;
    bool m_wrapAround = true;
    int m_visibleItemCount = 3;
    QString m_accessibilitySummary;
};


namespace {
constexpr int kDefaultCardWidth = 176;
constexpr int kDefaultCardHeight = 128;
constexpr int kGap = 12;
constexpr int kPadding = 16;
constexpr int kRadius = 18;

QString fallbackCarouselName()
{
    return QStringLiteral("Carousel");
}
} // namespace

QtMaterialCarousel::QtMaterialCarousel(QWidget* parent)
    : QWidget(parent)
{
    d_ptr = std::make_unique<QtMaterialCarouselPrivate>();

    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover, true);
    setAccessibleName(fallbackCarouselName());
    updateAccessibilitySummary();
}

QtMaterialCarousel::~QtMaterialCarousel() = default;

int QtMaterialCarousel::addItem(const QString& title, const QString& supportingText, const QIcon& icon)
{
    insertItem(d_ptr->m_items.size(), title, supportingText, icon);
    return d_ptr->m_items.size() - 1;
}

void QtMaterialCarousel::insertItem(int index, const QString& title, const QString& supportingText, const QIcon& icon)
{
    index = qBound(0, index, d_ptr->m_items.size());
    d_ptr->m_items.insert(index, Item{title, supportingText, icon, true});

    if (d_ptr->m_currentIndex < 0) {
        d_ptr->m_currentIndex = index;
        emitCurrentIndexChanged(d_ptr->m_currentIndex);
    } else if (index <= d_ptr->m_currentIndex) {
        ++d_ptr->m_currentIndex;
        emitCurrentIndexChanged(d_ptr->m_currentIndex);
    }

    emit itemCountChanged(d_ptr->m_items.size());
    updateAccessibilitySummary();
    updateGeometry();
    update();
}

void QtMaterialCarousel::removeItem(int index)
{
    if (!isValidIndex(index)) {
        return;
    }

    d_ptr->m_items.removeAt(index);

    if (d_ptr->m_items.isEmpty()) {
        if (d_ptr->m_currentIndex != -1) {
            d_ptr->m_currentIndex = -1;
            emitCurrentIndexChanged(d_ptr->m_currentIndex);
        }
    } else if (index < d_ptr->m_currentIndex) {
        --d_ptr->m_currentIndex;
        emitCurrentIndexChanged(d_ptr->m_currentIndex);
    } else if (index == d_ptr->m_currentIndex) {
        d_ptr->m_currentIndex = qMin(index, d_ptr->m_items.size() - 1);
        if (!d_ptr->m_items.at(d_ptr->m_currentIndex).enabled) {
            const int enabled = nextEnabledIndex(d_ptr->m_currentIndex, 1);
            d_ptr->m_currentIndex = enabled >= 0 ? enabled : d_ptr->m_currentIndex;
        }
        emitCurrentIndexChanged(d_ptr->m_currentIndex);
    }

    emit itemCountChanged(d_ptr->m_items.size());
    updateAccessibilitySummary();
    updateGeometry();
    update();
}

void QtMaterialCarousel::clear()
{
    if (d_ptr->m_items.isEmpty()) {
        return;
    }

    d_ptr->m_items.clear();
    d_ptr->m_currentIndex = -1;
    emit itemCountChanged(0);
    emitCurrentIndexChanged(-1);
    updateAccessibilitySummary();
    updateGeometry();
    update();
}

int QtMaterialCarousel::count() const noexcept
{
    return d_ptr->m_items.size();
}

bool QtMaterialCarousel::isEmpty() const noexcept
{
    return d_ptr->m_items.isEmpty();
}

QString QtMaterialCarousel::itemTitle(int index) const
{
    return isValidIndex(index) ? d_ptr->m_items.at(index).title : QString();
}

void QtMaterialCarousel::setItemTitle(int index, const QString& title)
{
    if (!isValidIndex(index) || d_ptr->m_items[index].title == title) {
        return;
    }

    d_ptr->m_items[index].title = title;
    updateAccessibilitySummary();
    update();
}

QString QtMaterialCarousel::itemSupportingText(int index) const
{
    return isValidIndex(index) ? d_ptr->m_items.at(index).supportingText : QString();
}

void QtMaterialCarousel::setItemSupportingText(int index, const QString& text)
{
    if (!isValidIndex(index) || d_ptr->m_items[index].supportingText == text) {
        return;
    }

    d_ptr->m_items[index].supportingText = text;
    updateAccessibilitySummary();
    update();
}

QIcon QtMaterialCarousel::itemIcon(int index) const
{
    return isValidIndex(index) ? d_ptr->m_items.at(index).icon : QIcon();
}

void QtMaterialCarousel::setItemIcon(int index, const QIcon& icon)
{
    if (!isValidIndex(index)) {
        return;
    }

    d_ptr->m_items[index].icon = icon;
    update();
}

bool QtMaterialCarousel::isItemEnabled(int index) const
{
    return isValidIndex(index) && d_ptr->m_items.at(index).enabled;
}

void QtMaterialCarousel::setItemEnabled(int index, bool enabled)
{
    if (!isValidIndex(index) || d_ptr->m_items[index].enabled == enabled) {
        return;
    }

    d_ptr->m_items[index].enabled = enabled;
    if (!enabled && index == d_ptr->m_currentIndex) {
        const int replacement = nextEnabledIndex(index, 1);
        if (replacement >= 0) {
            d_ptr->m_currentIndex = replacement;
            emitCurrentIndexChanged(d_ptr->m_currentIndex);
        }
    }

    updateAccessibilitySummary();
    update();
}

int QtMaterialCarousel::currentIndex() const noexcept
{
    return d_ptr->m_currentIndex;
}

void QtMaterialCarousel::setCurrentIndex(int index)
{
    if (!isValidIndex(index) || !d_ptr->m_items.at(index).enabled || index == d_ptr->m_currentIndex) {
        return;
    }

    d_ptr->m_currentIndex = index;
    emitCurrentIndexChanged(d_ptr->m_currentIndex);
    updateAccessibilitySummary();
    update();
}

bool QtMaterialCarousel::wrapAround() const noexcept
{
    return d_ptr->m_wrapAround;
}

void QtMaterialCarousel::setWrapAround(bool wrap)
{
    if (d_ptr->m_wrapAround == wrap) {
        return;
    }

    d_ptr->m_wrapAround = wrap;
    emit wrapAroundChanged(d_ptr->m_wrapAround);
}

int QtMaterialCarousel::visibleItemCount() const noexcept
{
    return d_ptr->m_visibleItemCount;
}

void QtMaterialCarousel::setVisibleItemCount(int count)
{
    count = qBound(1, count, 9);
    if (d_ptr->m_visibleItemCount == count) {
        return;
    }

    d_ptr->m_visibleItemCount = count;
    emit visibleItemCountChanged(d_ptr->m_visibleItemCount);
    updateGeometry();
    update();
}

QString QtMaterialCarousel::itemAccessibleText(int index) const
{
    if (!isValidIndex(index)) {
        return QString();
    }

    const auto& item = d_ptr->m_items.at(index);
    QStringList parts;
    parts << item.title;
    if (!item.supportingText.isEmpty()) {
        parts << item.supportingText;
    }
    parts << QStringLiteral("%1 of %2").arg(index + 1).arg(d_ptr->m_items.size());
    if (!item.enabled) {
        parts << QStringLiteral("disabled");
    }
    if (index == d_ptr->m_currentIndex) {
        parts << QStringLiteral("current");
    }
    return parts.join(QStringLiteral(", "));
}

QString QtMaterialCarousel::currentItemAccessibleText() const
{
    return itemAccessibleText(d_ptr->m_currentIndex);
}

QString QtMaterialCarousel::accessibilitySummary() const
{
    return d_ptr->m_accessibilitySummary;
}

QSize QtMaterialCarousel::sizeHint() const
{
    const int cards = qMax(1, d_ptr->m_visibleItemCount);
    return QSize(kPadding * 2 + cards * kDefaultCardWidth + (cards - 1) * kGap,
                 kPadding * 2 + kDefaultCardHeight);
}

QSize QtMaterialCarousel::minimumSizeHint() const
{
    return QSize(kPadding * 2 + kDefaultCardWidth, kPadding * 2 + kDefaultCardHeight);
}

void QtMaterialCarousel::next()
{
    const int nextIndex = nextEnabledIndex(d_ptr->m_currentIndex, 1);
    if (nextIndex >= 0) {
        setCurrentIndex(nextIndex);
    }
}

void QtMaterialCarousel::previous()
{
    const int previousIndex = nextEnabledIndex(d_ptr->m_currentIndex, -1);
    if (previousIndex >= 0) {
        setCurrentIndex(previousIndex);
    }
}

void QtMaterialCarousel::activateCurrentItem()
{
    if (isValidIndex(d_ptr->m_currentIndex) && d_ptr->m_items.at(d_ptr->m_currentIndex).enabled) {
        emit itemActivated(d_ptr->m_currentIndex);
    }
}

void QtMaterialCarousel::keyPressEvent(QKeyEvent* event)
{
    const bool rtl = layoutDirection() == Qt::RightToLeft;
    switch (event->key()) {
    case Qt::Key_Right:
        rtl ? previous() : next();
        event->accept();
        return;
    case Qt::Key_Left:
        rtl ? next() : previous();
        event->accept();
        return;
    case Qt::Key_Home:
        for (int i = 0; i < d_ptr->m_items.size(); ++i) {
            if (d_ptr->m_items.at(i).enabled) {
                setCurrentIndex(i);
                break;
            }
        }
        event->accept();
        return;
    case Qt::Key_End:
        for (int i = d_ptr->m_items.size() - 1; i >= 0; --i) {
            if (d_ptr->m_items.at(i).enabled) {
                setCurrentIndex(i);
                break;
            }
        }
        event->accept();
        return;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space:
        activateCurrentItem();
        event->accept();
        return;
    default:
        break;
    }

    QWidget::keyPressEvent(event);
}

void QtMaterialCarousel::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton) {
        QWidget::mouseReleaseEvent(event);
        return;
    }

    for (int i = 0; i < d_ptr->m_items.size(); ++i) {
        if (itemRect(i).contains(event->pos()) && d_ptr->m_items.at(i).enabled) {
            setCurrentIndex(i);
            emit itemClicked(i);
            emit itemActivated(i);
            event->accept();
            return;
        }
    }

    QWidget::mouseReleaseEvent(event);
}

void QtMaterialCarousel::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QColor base = palette().color(QPalette::Base);
    const QColor text = palette().color(QPalette::Text);
    const QColor muted = palette().color(QPalette::Disabled, QPalette::Text);
    const QColor highlight = palette().color(QPalette::Highlight);

    for (int i = 0; i < d_ptr->m_items.size(); ++i) {
        const QRect rect = itemRect(i);
        if (rect.isNull()) {
            continue;
        }

        const bool current = i == d_ptr->m_currentIndex;
        const bool enabled = d_ptr->m_items.at(i).enabled;
        const QColor fill = current ? highlight.lighter(185) : base;

        painter.setPen(current ? QPen(highlight, 2) : QPen(palette().color(QPalette::Mid), 1));
        painter.setBrush(fill);
        painter.drawRoundedRect(rect.adjusted(1, 1, -1, -1), kRadius, kRadius);

        QRect content = rect.adjusted(14, 12, -14, -12);
        if (!d_ptr->m_items.at(i).icon.isNull()) {
            const QSize iconSize(28, 28);
            const QRect iconRect(content.topLeft(), iconSize);
            d_ptr->m_items.at(i).icon.paint(&painter, iconRect, Qt::AlignCenter,
                                      enabled ? QIcon::Normal : QIcon::Disabled);
            content.setTop(iconRect.bottom() + 10);
        }

        painter.setPen(enabled ? text : muted);
        QFont titleFont = font();
        titleFont.setBold(current);
        painter.setFont(titleFont);
        const QFontMetrics titleMetrics(titleFont);
        painter.drawText(content,
                         Qt::AlignLeft | Qt::AlignTop,
                         titleMetrics.elidedText(d_ptr->m_items.at(i).title, Qt::ElideRight, content.width()));

        if (!d_ptr->m_items.at(i).supportingText.isEmpty()) {
            QFont supportFont = font();
            supportFont.setPointSize(qMax(1, supportFont.pointSize() - 1));
            painter.setFont(supportFont);
            painter.setPen(enabled ? palette().color(QPalette::PlaceholderText) : muted);
            const QRect supportRect = content.adjusted(0, titleMetrics.height() + 6, 0, 0);
            const QFontMetrics supportMetrics(supportFont);
            painter.drawText(supportRect,
                             Qt::AlignLeft | Qt::AlignTop,
                             supportMetrics.elidedText(d_ptr->m_items.at(i).supportingText,
                                                       Qt::ElideRight,
                                                       supportRect.width()));
        }
    }

    if (hasFocus()) {
        QStyleOptionFocusRect option;
        option.initFrom(this);
        option.rect = rect().adjusted(3, 3, -3, -3);
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &option, &painter, this);
    }
}

void QtMaterialCarousel::changeEvent(QEvent* event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::LayoutDirectionChange || event->type() == QEvent::PaletteChange ||
        event->type() == QEvent::FontChange) {
        update();
    }
}

bool QtMaterialCarousel::isValidIndex(int index) const noexcept
{
    return index >= 0 && index < d_ptr->m_items.size();
}

int QtMaterialCarousel::firstVisibleIndex() const noexcept
{
    if (d_ptr->m_items.isEmpty()) {
        return 0;
    }

    const int half = d_ptr->m_visibleItemCount / 2;
    return qBound(0, d_ptr->m_currentIndex - half, qMax(0, d_ptr->m_items.size() - d_ptr->m_visibleItemCount));
}

int QtMaterialCarousel::nextEnabledIndex(int start, int delta) const noexcept
{
    if (d_ptr->m_items.isEmpty() || delta == 0) {
        return -1;
    }

    int index = isValidIndex(start) ? start : (delta > 0 ? -1 : d_ptr->m_items.size());
    for (int step = 0; step < d_ptr->m_items.size(); ++step) {
        index += delta;
        if (d_ptr->m_wrapAround) {
            if (index >= d_ptr->m_items.size()) {
                index = 0;
            } else if (index < 0) {
                index = d_ptr->m_items.size() - 1;
            }
        }

        if (!isValidIndex(index)) {
            return -1;
        }
        if (d_ptr->m_items.at(index).enabled) {
            return index;
        }
    }
    return -1;
}

QRect QtMaterialCarousel::itemRect(int index) const
{
    if (!isValidIndex(index)) {
        return QRect();
    }

    const int first = firstVisibleIndex();
    const int last = qMin(d_ptr->m_items.size(), first + d_ptr->m_visibleItemCount);
    if (index < first || index >= last) {
        return QRect();
    }

    const int visibleOffset = index - first;
    const int cards = last - first;
    const int availableWidth = width() - kPadding * 2 - qMax(0, cards - 1) * kGap;
    const int cardWidth = qMax(72, availableWidth / qMax(1, cards));
    const int cardHeight = qMax(72, height() - kPadding * 2);

    const int visualOffset = layoutDirection() == Qt::RightToLeft ? cards - 1 - visibleOffset : visibleOffset;
    const int x = kPadding + visualOffset * (cardWidth + kGap);
    return QRect(x, kPadding, cardWidth, cardHeight);
}

void QtMaterialCarousel::updateAccessibilitySummary()
{
    QString summary;
    if (d_ptr->m_items.isEmpty()) {
        summary = QStringLiteral("Carousel, empty");
    } else {
        summary = QStringLiteral("Carousel, %1 items, current %2")
                      .arg(d_ptr->m_items.size())
                      .arg(currentItemAccessibleText());
    }

    if (d_ptr->m_accessibilitySummary == summary) {
        setAccessibleDescription(summary);
        return;
    }

    d_ptr->m_accessibilitySummary = summary;
    setAccessibleDescription(d_ptr->m_accessibilitySummary);
    emit accessibilitySummaryChanged(d_ptr->m_accessibilitySummary);
}

void QtMaterialCarousel::emitCurrentIndexChanged(int index)
{
    emit currentIndexChanged(index);
}

} // namespace QtMaterial
