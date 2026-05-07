#include "qtmaterial/widgets/surfaces/qtmaterialtopappbar.h"

#include <QApplication>
#include <QEvent>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QStyle>
#include <QToolButton>

namespace {
constexpr int kDefaultHeight = 64;
constexpr int kMinimumWidth = 200;
constexpr int kPreferredWidth = 360;
constexpr int kActionSlot = 48;
constexpr int kHorizontalPadding = 16;
constexpr int kBetweenPadding = 12;

QString fallbackActionName(int index)
{
    return QStringLiteral("Action %1").arg(index + 1);
}
}

QtMaterialTopAppBar::QtMaterialTopAppBar(QWidget* parent)
    : QtMaterialSurface(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setObjectName(QStringLiteral("qtmaterial_topAppBar"));
    syncButtons();
    syncAccessibility();
}

QtMaterialTopAppBar::QtMaterialTopAppBar(const QString& title, QWidget* parent)
    : QtMaterialTopAppBar(parent)
{
    setTitle(title);
}

QtMaterialTopAppBar::~QtMaterialTopAppBar() = default;

QString QtMaterialTopAppBar::title() const
{
    return m_title;
}

void QtMaterialTopAppBar::setTitle(const QString& title)
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

bool QtMaterialTopAppBar::centeredTitle() const noexcept
{
    return m_centeredTitle;
}

void QtMaterialTopAppBar::setCenteredTitle(bool value)
{
    if (m_centeredTitle == value) {
        return;
    }

    m_centeredTitle = value;
    invalidateLayoutCache();
    emit centeredTitleChanged(value);
    update();
}

bool QtMaterialTopAppBar::elevated() const noexcept
{
    return m_elevated;
}

void QtMaterialTopAppBar::setElevated(bool value)
{
    if (m_elevated == value) {
        return;
    }

    m_elevated = value;
    emit elevatedChanged(value);
    update();
}

QIcon QtMaterialTopAppBar::navigationIcon() const
{
    return m_navigationIcon;
}

void QtMaterialTopAppBar::setNavigationIcon(const QIcon& icon)
{
    const QString previousSummary = accessibilitySummary();
    m_navigationIcon = icon;
    syncButtons();
    invalidateLayoutCache();
    syncAccessibility();
    emitAccessibilityIfChanged(previousSummary);
    update();
}

void QtMaterialTopAppBar::clearNavigationIcon()
{
    setNavigationIcon(QIcon());
}

QString QtMaterialTopAppBar::navigationAccessibleName() const
{
    return m_navigationAccessibleName;
}

void QtMaterialTopAppBar::setNavigationAccessibleName(const QString& name)
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

int QtMaterialTopAppBar::actionCount() const noexcept
{
    return m_actionButtons.size();
}

int QtMaterialTopAppBar::addActionButton(const QIcon& icon, const QString& toolTip)
{
    const QString previousSummary = accessibilitySummary();
    const int index = m_actionButtons.size();
    auto* button = new QToolButton(this);
    button->setObjectName(QStringLiteral("qtmaterial_topAppBar_action_%1").arg(index));
    button->setAutoRaise(true);
    button->setIcon(icon);
    button->setToolTip(toolTip);
    button->setFocusPolicy(Qt::StrongFocus);
    button->setAccessibleName(toolTip.isEmpty() ? fallbackActionName(index) : toolTip);
    button->setAccessibleDescription(QStringLiteral("Top app bar action"));
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

void QtMaterialTopAppBar::clearActionButtons()
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

QString QtMaterialTopAppBar::actionAccessibleName(int index) const
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

void QtMaterialTopAppBar::setActionAccessibleName(int index, const QString& name)
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

QString QtMaterialTopAppBar::actionAccessibleText(int index) const
{
    const QString name = actionAccessibleName(index);
    if (name.isEmpty()) {
        return QString();
    }
    return QStringLiteral("%1, top app bar action").arg(name);
}

QString QtMaterialTopAppBar::accessibilitySummary() const
{
    QStringList parts;
    parts << (m_title.isEmpty() ? QStringLiteral("Top app bar") : m_title);
    if (!m_navigationIcon.isNull()) {
        parts << effectiveNavigationAccessibleName();
    }
    if (!m_actionButtons.isEmpty()) {
        parts << tr("%n action(s)", nullptr, m_actionButtons.size());
    }
    return parts.join(QStringLiteral(", "));
}

QSize QtMaterialTopAppBar::sizeHint() const
{
    return QSize(kPreferredWidth, kDefaultHeight);
}

QSize QtMaterialTopAppBar::minimumSizeHint() const
{
    return QSize(kMinimumWidth, kDefaultHeight);
}

void QtMaterialTopAppBar::paintEvent(QPaintEvent* event)
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
        p.drawLine(m_visualRect.bottomLeft(), m_visualRect.bottomRight());
    }

    p.setPen(text);
    QFont titleFont = font();
    titleFont.setBold(true);
    p.setFont(titleFont);
    const QString elided = QFontMetrics(titleFont).elidedText(m_title, Qt::ElideRight, m_titleRect.width());
    const int horizontalAlignment = m_centeredTitle ? Qt::AlignHCenter : (layoutDirection() == Qt::RightToLeft ? Qt::AlignRight : Qt::AlignLeft);
    p.drawText(m_titleRect, Qt::AlignVCenter | horizontalAlignment, elided);

    if (hasFocus()) {
        QStyleOptionFocusRect option;
        option.initFrom(this);
        option.rect = rect().adjusted(2, 2, -2, -2);
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &option, &p, this);
    }
}

void QtMaterialTopAppBar::resizeEvent(QResizeEvent* event)
{
    QtMaterialSurface::resizeEvent(event);
    invalidateLayoutCache();
}

void QtMaterialTopAppBar::changeEvent(QEvent* event)
{
    QtMaterialSurface::changeEvent(event);
    switch (event->type()) {
    case QEvent::EnabledChange:
    case QEvent::StyleChange:
    case QEvent::FontChange:
    case QEvent::LayoutDirectionChange:
        invalidateLayoutCache();
        syncButtons();
        syncAccessibility();
        break;
    default:
        break;
    }
}

void QtMaterialTopAppBar::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        clearFocus();
        event->accept();
        return;
    }

    if ((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return || event->key() == Qt::Key_Space || event->key() == Qt::Key_Select)
        && m_navigationButton && m_navigationButton->isVisible()) {
        emit navigationTriggered();
        event->accept();
        return;
    }

    QtMaterialSurface::keyPressEvent(event);
}

void QtMaterialTopAppBar::themeChangedEvent(const QtMaterial::Theme& theme)
{
    QtMaterialSurface::themeChangedEvent(theme);
    Q_UNUSED(theme)
    syncButtons();
    update();
}

void QtMaterialTopAppBar::stateChangedEvent()
{
    QtMaterialSurface::stateChangedEvent();
    update();
}

void QtMaterialTopAppBar::ensureLayoutResolved() const
{
    if (!m_layoutDirty) {
        return;
    }

    m_visualRect = rect();
    QRect content = m_visualRect.adjusted(kHorizontalPadding, 0, -kHorizontalPadding, 0);
    const int top = m_visualRect.center().y() - kActionSlot / 2;
    const bool rtl = layoutDirection() == Qt::RightToLeft;

    m_navRect = QRect();
    if (m_navigationButton && !m_navigationIcon.isNull()) {
        if (rtl) {
            m_navRect = QRect(content.right() - kActionSlot + 1, top, kActionSlot, kActionSlot);
            content.setRight(m_navRect.left() - kBetweenPadding);
        } else {
            m_navRect = QRect(content.left(), top, kActionSlot, kActionSlot);
            content.setLeft(m_navRect.right() + kBetweenPadding);
        }
    }

    m_actionRects.clear();
    if (rtl) {
        int left = content.left();
        for (int i = 0; i < m_actionButtons.size(); ++i) {
            QRect r(left, top, kActionSlot, kActionSlot);
            m_actionRects.push_back(r);
            left = r.right() + kBetweenPadding;
        }
        content.setLeft(left);
    } else {
        int right = content.right();
        for (int i = m_actionButtons.size() - 1; i >= 0; --i) {
            QRect r(right - kActionSlot + 1, top, kActionSlot, kActionSlot);
            m_actionRects.prepend(r);
            right = r.left() - kBetweenPadding;
        }
        content.setRight(right);
    }

    m_titleRect = QRect(content.left(), m_visualRect.top(), std::max(0, content.width()), m_visualRect.height());

    if (m_navigationButton) {
        m_navigationButton->setGeometry(m_navRect);
    }
    for (int i = 0; i < m_actionButtons.size() && i < m_actionRects.size(); ++i) {
        if (m_actionButtons[i].button) {
            m_actionButtons[i].button->setGeometry(m_actionRects[i]);
        }
    }

    m_layoutDirty = false;
}

void QtMaterialTopAppBar::invalidateLayoutCache()
{
    m_layoutDirty = true;
    updateGeometry();
    update();
}

void QtMaterialTopAppBar::syncButtons()
{
    if (!m_navigationIcon.isNull()) {
        if (!m_navigationButton) {
            auto* button = new QToolButton(this);
            button->setObjectName(QStringLiteral("qtmaterial_topAppBar_navigationButton"));
            button->setAutoRaise(true);
            button->setFocusPolicy(Qt::StrongFocus);
            connect(button, &QToolButton::clicked, this, &QtMaterialTopAppBar::navigationTriggered);
            m_navigationButton = button;
        }
        m_navigationButton->setIcon(m_navigationIcon);
        m_navigationButton->setAccessibleName(effectiveNavigationAccessibleName());
        m_navigationButton->setAccessibleDescription(QStringLiteral("Top app bar navigation"));
        if (m_navigationButton->toolTip().isEmpty()) {
            m_navigationButton->setToolTip(effectiveNavigationAccessibleName());
        }
        m_navigationButton->show();
    } else if (m_navigationButton) {
        m_navigationButton->hide();
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
        entry.button->setAccessibleName(actionAccessibleName(i));
        entry.button->setAccessibleDescription(QStringLiteral("Top app bar action"));
        if (entry.button->toolTip().isEmpty()) {
            entry.button->setToolTip(actionAccessibleName(i));
        }
        syncButton(entry.button);
    }
}

void QtMaterialTopAppBar::syncAccessibility()
{
    const QString name = m_title.isEmpty() ? QStringLiteral("Top app bar") : m_title;
    setAccessibleName(name);
    setAccessibleDescription(accessibilitySummary());
}

void QtMaterialTopAppBar::emitAccessibilityIfChanged(const QString& previousSummary)
{
    const QString current = accessibilitySummary();
    if (current != previousSummary && current != m_lastAccessibilitySummary) {
        m_lastAccessibilitySummary = current;
        emit accessibilitySummaryChanged(current);
    }
}

QString QtMaterialTopAppBar::effectiveNavigationAccessibleName() const
{
    if (!m_navigationAccessibleName.isEmpty()) {
        return m_navigationAccessibleName;
    }
    return QStringLiteral("Navigate");
}

QRect QtMaterialTopAppBar::availableContentRect() const
{
    ensureLayoutResolved();
    return m_titleRect;
}
