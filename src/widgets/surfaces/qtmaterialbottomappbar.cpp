#include "qtmaterial/widgets/surfaces/qtmaterialbottomappbar.h"

#include <QAbstractButton>
#include <QEvent>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QStyle>
#include <QToolButton>

namespace {
constexpr int kDefaultHeight = 80;
constexpr int kMinimumWidth = 240;
constexpr int kPreferredWidth = 360;
constexpr int kActionSlot = 48;
constexpr int kHorizontalPadding = 16;
constexpr int kFabSlot = 64;
constexpr int kBetweenPadding = 12;

QString fallbackActionName(int index)
{
    return QStringLiteral("Action %1").arg(index + 1);
}
}

QtMaterialBottomAppBar::QtMaterialBottomAppBar(QWidget* parent)
    : QtMaterialSurface(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setObjectName(QStringLiteral("qtmaterial_bottomAppBar"));
    syncButtons();
    syncAccessibility();
}

QtMaterialBottomAppBar::QtMaterialBottomAppBar(const QString& title, QWidget* parent)
    : QtMaterialBottomAppBar(parent)
{
    setTitle(title);
}

QtMaterialBottomAppBar::~QtMaterialBottomAppBar() = default;

QString QtMaterialBottomAppBar::title() const
{
    return m_title;
}

void QtMaterialBottomAppBar::setTitle(const QString& title)
{
    if (m_title == title) {
        return;
    }
    const QString previousSummary = accessibilitySummary();
    m_title = title;
    invalidateLayoutCache();
    syncAccessibility();
    emit titleChanged(m_title);
    emitAccessibilityIfChanged(previousSummary);
    updateGeometry();
    update();
}

bool QtMaterialBottomAppBar::elevated() const noexcept
{
    return m_elevated;
}

void QtMaterialBottomAppBar::setElevated(bool value)
{
    if (m_elevated == value) {
        return;
    }
    m_elevated = value;
    emit elevatedChanged(value);
    update();
}

bool QtMaterialBottomAppBar::fabAttached() const noexcept
{
    return m_fabAttached;
}

void QtMaterialBottomAppBar::setFabAttached(bool value)
{
    if (m_fabAttached == value) {
        return;
    }
    const QString previousSummary = accessibilitySummary();
    m_fabAttached = value;
    invalidateLayoutCache();
    syncAccessibility();
    emit fabAttachedChanged(value);
    emitAccessibilityIfChanged(previousSummary);
    updateGeometry();
    update();
}

QIcon QtMaterialBottomAppBar::navigationIcon() const
{
    return m_navigationIcon;
}

void QtMaterialBottomAppBar::setNavigationIcon(const QIcon& icon)
{
    const QString previousSummary = accessibilitySummary();
    m_navigationIcon = icon;
    syncButtons();
    invalidateLayoutCache();
    syncAccessibility();
    emitAccessibilityIfChanged(previousSummary);
    update();
}

void QtMaterialBottomAppBar::clearNavigationIcon()
{
    setNavigationIcon(QIcon());
}

QString QtMaterialBottomAppBar::navigationAccessibleName() const
{
    return m_navigationAccessibleName;
}

void QtMaterialBottomAppBar::setNavigationAccessibleName(const QString& name)
{
    if (m_navigationAccessibleName == name) {
        return;
    }
    const QString previousSummary = accessibilitySummary();
    m_navigationAccessibleName = name;
    syncButtons();
    syncAccessibility();
    emit navigationAccessibleNameChanged(m_navigationAccessibleName);
    emitAccessibilityIfChanged(previousSummary);
}

int QtMaterialBottomAppBar::actionCount() const noexcept
{
    return m_actionButtons.size();
}

int QtMaterialBottomAppBar::addActionButton(const QIcon& icon, const QString& toolTip)
{
    const QString previousSummary = accessibilitySummary();
    const int index = m_actionButtons.size();
    auto* button = new QToolButton(this);
    button->setObjectName(QStringLiteral("qtmaterial_bottomAppBar_action_%1").arg(index));
    button->setAutoRaise(true);
    button->setIcon(icon);
    button->setToolTip(toolTip);
    button->setFocusPolicy(Qt::StrongFocus);
    button->setAccessibleName(toolTip.isEmpty() ? fallbackActionName(index) : toolTip);
    button->setAccessibleDescription(QStringLiteral("Bottom app bar action"));
    connect(button, &QToolButton::clicked, this, [this, index]() { emit actionTriggered(index); });
    m_actionButtons.push_back({button, index, toolTip});
    invalidateLayoutCache();
    syncButtons();
    syncAccessibility();
    emitAccessibilityIfChanged(previousSummary);
    updateGeometry();
    update();
    return index;
}

void QtMaterialBottomAppBar::clearActionButtons()
{
    const QString previousSummary = accessibilitySummary();
    for (auto& entry : m_actionButtons) {
        if (entry.button) {
            entry.button->deleteLater();
        }
    }
    m_actionButtons.clear();
    invalidateLayoutCache();
    syncAccessibility();
    emitAccessibilityIfChanged(previousSummary);
    updateGeometry();
    update();
}

QString QtMaterialBottomAppBar::actionAccessibleName(int index) const
{
    if (index < 0 || index >= m_actionButtons.size()) {
        return QString();
    }
    const auto& entry = m_actionButtons.at(index);
    if (!entry.accessibleName.isEmpty()) {
        return entry.accessibleName;
    }
    if (entry.button && !entry.button->toolTip().isEmpty()) {
        return entry.button->toolTip();
    }
    return fallbackActionName(index);
}

void QtMaterialBottomAppBar::setActionAccessibleName(int index, const QString& name)
{
    if (index < 0 || index >= m_actionButtons.size()) {
        return;
    }
    if (m_actionButtons[index].accessibleName == name) {
        return;
    }
    const QString previousSummary = accessibilitySummary();
    m_actionButtons[index].accessibleName = name;
    if (m_actionButtons[index].button) {
        m_actionButtons[index].button->setAccessibleName(actionAccessibleName(index));
        if (m_actionButtons[index].button->toolTip().isEmpty()) {
            m_actionButtons[index].button->setToolTip(actionAccessibleName(index));
        }
    }
    syncAccessibility();
    emitAccessibilityIfChanged(previousSummary);
}

QString QtMaterialBottomAppBar::actionAccessibleText(int index) const
{
    const QString name = actionAccessibleName(index);
    if (name.isEmpty()) {
        return QString();
    }
    return QStringLiteral("%1, bottom app bar action").arg(name);
}

void QtMaterialBottomAppBar::setFabButton(QAbstractButton* button)
{
    if (m_fabButton == button) {
        return;
    }
    const QString previousSummary = accessibilitySummary();
    if (m_fabButton) {
        m_fabButton->setParent(nullptr);
    }
    m_fabButton = button;
    if (m_fabButton) {
        m_fabButton->setParent(this);
        m_fabButton->setFocusPolicy(Qt::StrongFocus);
        if (m_fabButton->accessibleName().isEmpty()) {
            m_fabButton->setAccessibleName(QStringLiteral("Primary action"));
        }
        m_fabButton->setAccessibleDescription(QStringLiteral("Bottom app bar floating action button"));
        m_fabButton->raise();
    }
    invalidateLayoutCache();
    syncAccessibility();
    emitAccessibilityIfChanged(previousSummary);
    updateGeometry();
    update();
}

QAbstractButton* QtMaterialBottomAppBar::fabButton() const noexcept
{
    return m_fabButton;
}

QString QtMaterialBottomAppBar::fabAccessibleText() const
{
    if (!m_fabButton) {
        return QString();
    }
    const QString name = m_fabButton->accessibleName().isEmpty()
        ? m_fabButton->text()
        : m_fabButton->accessibleName();
    return name.isEmpty() ? QStringLiteral("Primary action") : name;
}

QString QtMaterialBottomAppBar::accessibilitySummary() const
{
    QStringList parts;
    parts << (m_title.isEmpty() ? QStringLiteral("Bottom app bar") : m_title);
    if (!m_navigationIcon.isNull()) {
        parts << effectiveNavigationAccessibleName();
    }
    if (!m_actionButtons.isEmpty()) {
        parts << tr("%n action(s)", nullptr, m_actionButtons.size());
    }
    if (m_fabButton) {
        parts << fabAccessibleText();
    }
    return parts.join(QStringLiteral(", "));
}

QSize QtMaterialBottomAppBar::sizeHint() const
{
    return QSize(kPreferredWidth, kDefaultHeight + (m_fabAttached ? 16 : 0));
}

QSize QtMaterialBottomAppBar::minimumSizeHint() const
{
    return QSize(kMinimumWidth, kDefaultHeight);
}

void QtMaterialBottomAppBar::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    ensureLayoutResolved();

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    const QColor base = palette().color(QPalette::Window);
    const QColor outline = palette().color(QPalette::Mid);
    const QColor text = palette().color(QPalette::WindowText);

    p.setPen(Qt::NoPen);
    p.setBrush(base);
    p.drawRect(m_visualRect);

    if (m_elevated) {
        p.setPen(outline);
        p.drawLine(m_visualRect.topLeft(), m_visualRect.topRight());
    }

    p.setPen(text);
    QFont titleFont = font();
    titleFont.setBold(true);
    p.setFont(titleFont);
    const QString elided = QFontMetrics(titleFont).elidedText(m_title, Qt::ElideRight, m_titleRect.width());
    const int horizontalAlignment = layoutDirection() == Qt::RightToLeft ? Qt::AlignRight : Qt::AlignLeft;
    p.drawText(m_titleRect, Qt::AlignVCenter | horizontalAlignment, elided);

    if (hasFocus()) {
        QStyleOptionFocusRect option;
        option.initFrom(this);
        option.rect = rect().adjusted(2, 2, -2, -2);
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &option, &p, this);
    }
}

void QtMaterialBottomAppBar::resizeEvent(QResizeEvent* event)
{
    QtMaterialSurface::resizeEvent(event);
    invalidateLayoutCache();
}

void QtMaterialBottomAppBar::changeEvent(QEvent* event)
{
    QtMaterialSurface::changeEvent(event);
    switch (event->type()) {
    case QEvent::StyleChange:
    case QEvent::FontChange:
    case QEvent::LayoutDirectionChange:
    case QEvent::EnabledChange:
        invalidateLayoutCache();
        syncButtons();
        syncAccessibility();
        break;
    default:
        break;
    }
}

void QtMaterialBottomAppBar::keyPressEvent(QKeyEvent* event)
{
    if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter || event->key() == Qt::Key_Space || event->key() == Qt::Key_Select)
        && m_navigationButton && m_navigationButton->isVisible()) {
        emit navigationTriggered();
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_Escape) {
        clearFocus();
        event->accept();
        return;
    }
    QtMaterialSurface::keyPressEvent(event);
}

void QtMaterialBottomAppBar::themeChangedEvent(const QtMaterial::Theme& theme)
{
    QtMaterialSurface::themeChangedEvent(theme);
    Q_UNUSED(theme)
    syncButtons();
    update();
}

void QtMaterialBottomAppBar::stateChangedEvent()
{
    QtMaterialSurface::stateChangedEvent();
    update();
}

void QtMaterialBottomAppBar::ensureLayoutResolved() const
{
    if (!m_layoutDirty) {
        return;
    }

    m_visualRect = rect().adjusted(0, m_fabAttached ? 8 : 0, 0, 0);
    if (m_visualRect.height() <= 0) {
        m_visualRect = QRect(0, 0, width(), sizeHint().height());
    }
    const QRect content = availableContentRect();
    int left = content.left();
    int right = content.right();
    const bool rtl = layoutDirection() == Qt::RightToLeft;

    m_navRect = QRect();
    if (m_navigationButton && m_navigationButton->isVisible()) {
        if (rtl) {
            m_navRect = QRect(right - kActionSlot + 1, content.center().y() - kActionSlot / 2, kActionSlot, kActionSlot);
            right = m_navRect.left() - kBetweenPadding;
        } else {
            m_navRect = QRect(left, content.center().y() - kActionSlot / 2, kActionSlot, kActionSlot);
            left = m_navRect.right() + kBetweenPadding;
        }
        m_navigationButton->setGeometry(m_navRect);
    }

    m_actionRects.clear();
    if (rtl) {
        int currentLeft = left;
        for (int i = 0; i < m_actionButtons.size(); ++i) {
            QRect slot(currentLeft, content.center().y() - kActionSlot / 2, kActionSlot, kActionSlot);
            m_actionRects.push_back(slot);
            currentLeft = slot.right() + kBetweenPadding;
            if (m_actionButtons[i].button) {
                m_actionButtons[i].button->setGeometry(slot);
            }
        }
        left = currentLeft;
    } else {
        int currentRight = right;
        for (int i = m_actionButtons.size() - 1; i >= 0; --i) {
            QRect slot(currentRight - kActionSlot + 1, content.center().y() - kActionSlot / 2, kActionSlot, kActionSlot);
            m_actionRects.prepend(slot);
            currentRight = slot.left() - kBetweenPadding;
            if (m_actionButtons[i].button) {
                m_actionButtons[i].button->setGeometry(slot);
            }
        }
        right = currentRight;
    }

    m_fabRect = QRect();
    if (m_fabAttached && m_fabButton) {
        m_fabRect = QRect(content.center().x() - kFabSlot / 2, m_visualRect.top() - 20, kFabSlot, kFabSlot);
        m_fabButton->setGeometry(m_fabRect);
        m_fabButton->raise();
        const int gap = (kFabSlot / 2) + kBetweenPadding;
        if (left < m_fabRect.center().x() && m_fabRect.center().x() < right) {
            if (layoutDirection() == Qt::RightToLeft) {
                left = std::max(left, m_fabRect.right() + gap);
            } else {
                right = std::min(right, m_fabRect.left() - gap);
            }
        }
    }

    m_titleRect = QRect(left, content.top(), std::max(0, right - left), content.height());
    m_layoutDirty = false;
}

void QtMaterialBottomAppBar::invalidateLayoutCache()
{
    m_layoutDirty = true;
    updateGeometry();
    update();
}

void QtMaterialBottomAppBar::syncButtons()
{
    if (!m_navigationButton) {
        m_navigationButton = new QToolButton(this);
        m_navigationButton->setObjectName(QStringLiteral("qtmaterial_bottomAppBar_navigationButton"));
        m_navigationButton->setAutoRaise(true);
        m_navigationButton->setFocusPolicy(Qt::StrongFocus);
        connect(m_navigationButton, &QToolButton::clicked, this, &QtMaterialBottomAppBar::navigationTriggered);
    }
    m_navigationButton->setVisible(!m_navigationIcon.isNull());
    m_navigationButton->setIcon(m_navigationIcon);
    m_navigationButton->setAccessibleName(effectiveNavigationAccessibleName());
    m_navigationButton->setAccessibleDescription(QStringLiteral("Bottom app bar navigation"));
    if (m_navigationButton->toolTip().isEmpty()) {
        m_navigationButton->setToolTip(effectiveNavigationAccessibleName());
    }

    auto syncButton = [this](QToolButton* button) {
        if (!button) {
            return;
        }
        button->setEnabled(isEnabled());
        button->setIconSize(QSize(24, 24));
        button->setStyleSheet(QStringLiteral("QToolButton { border: none; background: transparent; }"));
    };

    syncButton(m_navigationButton);
    for (int i = 0; i < m_actionButtons.size(); ++i) {
        auto& entry = m_actionButtons[i];
        if (!entry.button) {
            continue;
        }
        entry.button->setVisible(true);
        entry.button->setAccessibleName(actionAccessibleName(i));
        entry.button->setAccessibleDescription(QStringLiteral("Bottom app bar action"));
        if (entry.button->toolTip().isEmpty()) {
            entry.button->setToolTip(actionAccessibleName(i));
        }
        entry.button->raise();
        syncButton(entry.button);
    }
    if (m_fabButton) {
        m_fabButton->raise();
    }
}

void QtMaterialBottomAppBar::syncAccessibility()
{
    const QString name = m_title.isEmpty() ? QStringLiteral("Bottom app bar") : m_title;
    setAccessibleName(name);
    setAccessibleDescription(accessibilitySummary());
}

void QtMaterialBottomAppBar::emitAccessibilityIfChanged(const QString& previousSummary)
{
    const QString current = accessibilitySummary();
    if (current != previousSummary && current != m_lastAccessibilitySummary) {
        m_lastAccessibilitySummary = current;
        emit accessibilitySummaryChanged(current);
    }
}

QString QtMaterialBottomAppBar::effectiveNavigationAccessibleName() const
{
    if (!m_navigationAccessibleName.isEmpty()) {
        return m_navigationAccessibleName;
    }
    return QStringLiteral("Navigate");
}

QRect QtMaterialBottomAppBar::availableContentRect() const
{
    return m_visualRect.adjusted(kHorizontalPadding, 8, -kHorizontalPadding, -8);
}
