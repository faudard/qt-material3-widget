#include "qtmaterial/widgets/navigation/qtmaterialfloatingtoolbar.h"

#include <QBoxLayout>
#include <QEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QPalette>

#include "../resolution/qtmaterialmissingmaterial3specresolution_p.h"
#include "qtmaterial/widgets/buttons/qtmaterialiconbutton.h"

namespace QtMaterial {

QtMaterialFloatingToolbar::QtMaterialFloatingToolbar(QWidget* parent)
    : QtMaterialWidget(parent)
    , m_layout(new QBoxLayout(QBoxLayout::LeftToRight, this))
{
    setMaterialComponent(QStringLiteral("FloatingToolbar"));
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_TranslucentBackground, true);
    m_layout->setContentsMargins(6, 6, 6, 6);
    m_layout->setSpacing(4);
    setAccessibleName(tr("Floating toolbar"));
}

QtMaterialFloatingToolbar::~QtMaterialFloatingToolbar()
{
    // QWidget destroys child items after derived members have been torn down.
    // Disconnect callbacks now so QObject::destroyed cannot access m_items
    // once its storage has already been released.
    for (QWidget* item : m_items) {
        if (!item) {
            continue;
        }
        item->removeEventFilter(this);
        QObject::disconnect(item, nullptr, this, nullptr);
    }
    m_items.clear();
}

Qt::Orientation QtMaterialFloatingToolbar::orientation() const noexcept
{
    return m_orientation;
}

void QtMaterialFloatingToolbar::setOrientation(Qt::Orientation orientation)
{
    if (m_orientation == orientation) {
        return;
    }

    m_orientation = orientation;
    m_layout->setDirection(
        orientation == Qt::Horizontal
            ? QBoxLayout::LeftToRight
            : QBoxLayout::TopToBottom);
    updateGeometry();
    Q_EMIT orientationChanged(orientation);
}

bool QtMaterialFloatingToolbar::isExpanded() const noexcept
{
    return m_expanded;
}

void QtMaterialFloatingToolbar::setExpanded(bool expanded)
{
    if (m_expanded == expanded) {
        return;
    }
    m_expanded = expanded;
    syncVisibility();
    Q_EMIT expandedChanged(expanded);
}

int QtMaterialFloatingToolbar::spacing() const noexcept
{
    return m_layout->spacing();
}

void QtMaterialFloatingToolbar::setSpacing(int spacing)
{
    spacing = qMax(0, spacing);
    if (m_layout->spacing() == spacing) {
        return;
    }
    m_layout->setSpacing(spacing);
    updateGeometry();
    Q_EMIT spacingChanged(spacing);
}

int QtMaterialFloatingToolbar::count() const noexcept
{
    return m_items.size();
}

QWidget* QtMaterialFloatingToolbar::itemAt(int index) const
{
    return index >= 0 && index < m_items.size()
        ? m_items.at(index)
        : nullptr;
}

QtMaterialIconButton* QtMaterialFloatingToolbar::addAction(
    const QIcon& icon,
    const QString& accessibleName)
{
    auto* button = new QtMaterialIconButton(icon, this);
    button->setAccessibleName(accessibleName);
    button->setRequiresAccessibleName(true);
    addWidget(button);

    connect(button, &QAbstractButton::clicked, this, [this, button]() {
        const int index = m_items.indexOf(button);
        if (index >= 0) {
            Q_EMIT actionTriggered(index);
        }
    });

    return button;
}

void QtMaterialFloatingToolbar::addWidget(QWidget* widget)
{
    if (!widget || m_items.contains(widget)) {
        return;
    }

    widget->setParent(this);
    widget->installEventFilter(this);
    m_items.append(widget);
    m_layout->addWidget(widget);

    connect(widget, &QObject::destroyed, this, [this, widget]() {
        const int index = m_items.indexOf(widget);
        if (index >= 0) {
            m_items.remove(index);
            syncVisibility();
        }
    });

    syncVisibility();
}

void QtMaterialFloatingToolbar::removeWidget(QWidget* widget)
{
    const int index = m_items.indexOf(widget);
    if (index < 0) {
        return;
    }

    m_items.remove(index);
    m_layout->removeWidget(widget);
    widget->removeEventFilter(this);
    widget->setParent(nullptr);
    syncVisibility();
}

void QtMaterialFloatingToolbar::clear()
{
    while (!m_items.isEmpty()) {
        QWidget* widget = m_items.takeLast();
        if (!widget) {
            continue;
        }
        widget->removeEventFilter(this);
        m_layout->removeWidget(widget);
        widget->setParent(nullptr);
    }
    syncVisibility();
}

void QtMaterialFloatingToolbar::paintEvent(QPaintEvent* event)
{
    QtMaterialWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const auto resolved =
        MissingMaterial3SpecResolution::floatingToolbarSpec(
            theme(),
            palette());

    painter.setPen(Qt::NoPen);
    painter.setBrush(resolved.containerColor);

    const QRectF bounds = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
    const qreal radius = qMin<qreal>(28.0, bounds.height() / 2.0);
    painter.drawRoundedRect(bounds, radius, radius);
}

bool QtMaterialFloatingToolbar::eventFilter(QObject* watched, QEvent* event)
{
    if (!event || event->type() != QEvent::KeyPress) {
        return QtMaterialWidget::eventFilter(watched, event);
    }

    const int current = itemIndex(watched);
    if (current < 0) {
        return QtMaterialWidget::eventFilter(watched, event);
    }

    auto* keyEvent = static_cast<QKeyEvent*>(event);
    int step = 0;
    if (m_orientation == Qt::Horizontal) {
        if (keyEvent->key() == Qt::Key_Left) {
            step = layoutDirection() == Qt::RightToLeft ? 1 : -1;
        } else if (keyEvent->key() == Qt::Key_Right) {
            step = layoutDirection() == Qt::RightToLeft ? -1 : 1;
        }
    } else {
        if (keyEvent->key() == Qt::Key_Up) {
            step = -1;
        } else if (keyEvent->key() == Qt::Key_Down) {
            step = 1;
        }
    }

    if (keyEvent->key() == Qt::Key_Home) {
        focusIndex(nextFocusable(-1, 1));
        keyEvent->accept();
        return true;
    }
    if (keyEvent->key() == Qt::Key_End) {
        focusIndex(nextFocusable(m_items.size(), -1));
        keyEvent->accept();
        return true;
    }
    if (step == 0) {
        return QtMaterialWidget::eventFilter(watched, event);
    }

    focusIndex(nextFocusable(current, step));
    keyEvent->accept();
    return true;
}

void QtMaterialFloatingToolbar::themeChangedEvent(const Theme& theme)
{
    QtMaterialWidget::themeChangedEvent(theme);
    update();
}

int QtMaterialFloatingToolbar::itemIndex(const QObject* object) const noexcept
{
    for (int index = 0; index < m_items.size(); ++index) {
        if (m_items.at(index) == object) {
            return index;
        }
    }
    return -1;
}

int QtMaterialFloatingToolbar::nextFocusable(int start, int step) const noexcept
{
    if (m_items.isEmpty() || step == 0) {
        return -1;
    }

    int index = start;
    for (int scanned = 0; scanned < m_items.size(); ++scanned) {
        index += step;
        if (index < 0) {
            index = m_items.size() - 1;
        } else if (index >= m_items.size()) {
            index = 0;
        }
        QWidget* widget = m_items.at(index);
        if (widget && widget->isEnabled() && !widget->isHidden()
            && widget->focusPolicy() != Qt::NoFocus) {
            return index;
        }
    }
    return -1;
}

void QtMaterialFloatingToolbar::syncVisibility()
{
    for (int index = 0; index < m_items.size(); ++index) {
        QWidget* item = m_items.at(index);
        if (item) {
            item->setVisible(m_expanded || index == 0);
        }
    }

    setAccessibleDescription(
        tr("%1 toolbar, %2 actions")
            .arg(m_expanded ? tr("Expanded") : tr("Collapsed"))
            .arg(m_items.size()));
    updateGeometry();
    update();
}

void QtMaterialFloatingToolbar::focusIndex(int index)
{
    QWidget* widget = itemAt(index);
    if (widget) {
        widget->setFocus(Qt::TabFocusReason);
    }
}

} // namespace QtMaterial
