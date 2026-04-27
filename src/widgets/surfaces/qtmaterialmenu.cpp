#include "qtmaterial/widgets/surfaces/qtmaterialmenu.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {

QtMaterialMenu::QtMaterialMenu(QWidget* parent)
    : QtMaterialControl(parent)
{
    setWindowFlag(Qt::Popup, true);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
}

QtMaterialMenu::~QtMaterialMenu() = default;

int QtMaterialMenu::addItem(const QString& text, const QIcon& icon, const QString& shortcut)
{
    m_items.push_back(MenuItem{text, icon, shortcut, false, true});
    updateGeometry();
    update();
    return m_items.size() - 1;
}

void QtMaterialMenu::addSeparator()
{
    m_items.push_back(MenuItem{QString(), QIcon(), QString(), true, false});
    updateGeometry();
    update();
}

void QtMaterialMenu::clear()
{
    m_items.clear();
    setCurrentIndex(-1);
    updateGeometry();
    update();
}

int QtMaterialMenu::count() const noexcept { return m_items.size(); }

QString QtMaterialMenu::itemText(int index) const
{
    return index >= 0 && index < m_items.size() ? m_items.at(index).text : QString();
}

void QtMaterialMenu::setItemEnabled(int index, bool enabled)
{
    if (index < 0 || index >= m_items.size() || m_items[index].enabled == enabled) {
        return;
    }
    m_items[index].enabled = enabled;
    update();
}

bool QtMaterialMenu::isItemEnabled(int index) const
{
    return index >= 0 && index < m_items.size() && m_items.at(index).enabled;
}

int QtMaterialMenu::currentIndex() const noexcept { return m_currentIndex; }

void QtMaterialMenu::setCurrentIndex(int index)
{
    if (index < -1 || index >= m_items.size() || m_currentIndex == index) {
        return;
    }
    if (index >= 0 && (m_items.at(index).separator || !m_items.at(index).enabled)) {
        return;
    }
    m_currentIndex = index;
    update();
    emit currentIndexChanged(index);
}

void QtMaterialMenu::popup(const QPoint& globalPos)
{
    resize(sizeHint());
    move(globalPos);
    show();
    raise();
    setFocus(Qt::PopupFocusReason);
}

void QtMaterialMenu::themeChangedEvent(const Theme& theme)
{
    QtMaterialControl::themeChangedEvent(theme);
    invalidateResolvedSpec();
}

void QtMaterialMenu::invalidateResolvedSpec()
{
    m_specDirty = true;
}

void QtMaterialMenu::ensureSpecResolved() const
{
    if (!m_specDirty) {
        return;
    }
    SpecFactory factory;
    m_spec = factory.menuSpec(theme(), density());
    m_specDirty = false;
}

QRect QtMaterialMenu::itemRect(int index) const
{
    ensureSpecResolved();
    return QRect(0, index * m_spec.minItemSize.height(), width(), m_spec.minItemSize.height());
}

int QtMaterialMenu::indexAt(const QPoint& pos) const
{
    for (int i = 0; i < m_items.size(); ++i) {
        if (itemRect(i).contains(pos)) {
            return i;
        }
    }
    return -1;
}

void QtMaterialMenu::triggerIndex(int index)
{
    if (index < 0 || index >= m_items.size() || m_items.at(index).separator || !m_items.at(index).enabled) {
        return;
    }
    emit itemTriggered(index, m_items.at(index).text);
    hide();
}

QSize QtMaterialMenu::sizeHint() const
{
    ensureSpecResolved();
    int width = m_spec.minItemSize.width();
    QFont labelFont = font();
    if (theme().typography().contains(m_spec.labelTypeRole)) {
        labelFont = theme().typography().style(m_spec.labelTypeRole).font;
    }
    const QFontMetrics fm(labelFont);
    for (const MenuItem& item : m_items) {
        if (item.separator) {
            continue;
        }
        int itemWidth = m_spec.itemPadding.left() + m_spec.itemPadding.right() + fm.horizontalAdvance(item.text);
        if (!item.icon.isNull()) {
            itemWidth += m_spec.iconSize + m_spec.iconSpacing;
        }
        if (!item.shortcut.isEmpty()) {
            itemWidth += 32 + fm.horizontalAdvance(item.shortcut);
        }
        width = qMax(width, itemWidth);
    }
    const int height = qMin(m_spec.maxPopupSize.height(), m_items.size() * m_spec.minItemSize.height());
    return QSize(qMin(width, m_spec.maxPopupSize.width()), height);
}

QSize QtMaterialMenu::minimumSizeHint() const
{
    ensureSpecResolved();
    return m_spec.minItemSize;
}

void QtMaterialMenu::mouseMoveEvent(QMouseEvent* event)
{
    setCurrentIndex(indexAt(event->pos()));
    QtMaterialControl::mouseMoveEvent(event);
}

void QtMaterialMenu::mouseReleaseEvent(QMouseEvent* event)
{
    triggerIndex(indexAt(event->pos()));
    QtMaterialControl::mouseReleaseEvent(event);
}

void QtMaterialMenu::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        hide();
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_Down || event->key() == Qt::Key_Up) {
        if (!m_items.isEmpty()) {
            const int delta = event->key() == Qt::Key_Down ? 1 : -1;
            int candidate = m_currentIndex;
            for (int step = 0; step < m_items.size(); ++step) {
                candidate = (candidate + delta + m_items.size()) % m_items.size();
                if (!m_items.at(candidate).separator && m_items.at(candidate).enabled) {
                    setCurrentIndex(candidate);
                    break;
                }
            }
        }
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter || event->key() == Qt::Key_Space) {
        triggerIndex(m_currentIndex);
        event->accept();
        return;
    }
    QtMaterialControl::keyPressEvent(event);
}

void QtMaterialMenu::paintEvent(QPaintEvent*)
{
    ensureSpecResolved();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const qreal radius = theme().shapes().contains(m_spec.shapeRole)
        ? theme().shapes().radius(m_spec.shapeRole)
        : m_spec.cornerRadius;
    QPainterPath containerPath;
    containerPath.addRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
    painter.fillPath(containerPath, m_spec.containerColor);

    QFont labelFont = font();
    if (theme().typography().contains(m_spec.labelTypeRole)) {
        labelFont = theme().typography().style(m_spec.labelTypeRole).font;
    }
    painter.setFont(labelFont);

    for (int i = 0; i < m_items.size(); ++i) {
        const QRect r = itemRect(i);
        const MenuItem& item = m_items.at(i);
        if (item.separator) {
            painter.setPen(QPen(m_spec.dividerColor, 1));
            painter.drawLine(r.left() + 8, r.center().y(), r.right() - 8, r.center().y());
            continue;
        }
        if (i == m_currentIndex) {
            QPainterPath layer;
            layer.addRect(r);
            QtMaterialStateLayerPainter::paintPath(&painter, layer, m_spec.stateLayerColor, theme().stateLayer().hoverOpacity);
        }
        const QColor textColor = item.enabled ? m_spec.itemLabelColor : m_spec.disabledItemLabelColor;
        painter.setPen(textColor);
        int x = m_spec.itemPadding.left();
        if (!item.icon.isNull()) {
            const QRect iconRect(x, r.center().y() - m_spec.iconSize / 2, m_spec.iconSize, m_spec.iconSize);
            item.icon.paint(&painter, iconRect, Qt::AlignCenter, item.enabled ? QIcon::Normal : QIcon::Disabled);
            x += m_spec.iconSize + m_spec.iconSpacing;
        }
        painter.drawText(QRect(x, r.top(), r.width() - x - m_spec.itemPadding.right(), r.height()), Qt::AlignVCenter | Qt::AlignLeft, item.text);
        if (!item.shortcut.isEmpty()) {
            painter.setPen(item.enabled ? m_spec.shortcutColor : m_spec.disabledItemLabelColor);
            painter.drawText(r.adjusted(0, 0, -m_spec.itemPadding.right(), 0), Qt::AlignVCenter | Qt::AlignRight, item.shortcut);
        }
    }

    if (hasFocus() && m_currentIndex >= 0) {
        QPainterPath focusPath;
        focusPath.addRect(itemRect(m_currentIndex).adjusted(2, 2, -2, -2));
        QtMaterialFocusIndicator::paintPathFocusRing(&painter, focusPath, m_spec.focusRingColor, 2.0);
    }
}

} // namespace QtMaterial
