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
#include <qstyleoption.h>

#include <memory>
#include <QPainterPath>
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

struct QtMaterialBottomAppBarPrivate
{
    struct ActionButtonEntry
    {
        QPointer<QToolButton> button;
        int index = -1;
        QString accessibleName;
    };

    QString m_title;
    QIcon m_navigationIcon;
    QString m_navigationAccessibleName;
    bool m_elevated = false;
    bool m_fabAttached = false;

    mutable bool m_layoutDirty = true;
    mutable QRect m_visualRect;
    mutable QRect m_navRect;
    mutable QRect m_titleRect;
    mutable QRect m_fabRect;
    mutable QVector<QRect> m_actionRects;
    mutable QPainterPath m_containerPath;

    QPointer<QToolButton> m_navigationButton;
    QVector<ActionButtonEntry> m_actionButtons;
    QPointer<QAbstractButton> m_fabButton;
    QString m_lastAccessibilitySummary;
};

QtMaterialBottomAppBar::QtMaterialBottomAppBar(QWidget* parent)
    : QtMaterialSurface(parent)
    , d(std::make_unique<QtMaterialBottomAppBarPrivate>())
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
    return d->m_title;
}

void QtMaterialBottomAppBar::setTitle(const QString& title)
{
    if (d->m_title == title) {
        return;
    }
    const QString previousSummary = accessibilitySummary();
    d->m_title = title;
    invalidateLayoutCache();
    syncAccessibility();
    emit titleChanged(d->m_title);
    emitAccessibilityIfChanged(previousSummary);
    updateGeometry();
    update();
}

bool QtMaterialBottomAppBar::elevated() const noexcept
{
    return d->m_elevated;
}

void QtMaterialBottomAppBar::setElevated(bool value)
{
    if (d->m_elevated == value) {
        return;
    }
    d->m_elevated = value;
    emit elevatedChanged(value);
    update();
}

bool QtMaterialBottomAppBar::fabAttached() const noexcept
{
    return d->m_fabAttached;
}

void QtMaterialBottomAppBar::setFabAttached(bool value)
{
    if (d->m_fabAttached == value) {
        return;
    }
    const QString previousSummary = accessibilitySummary();
    d->m_fabAttached = value;
    invalidateLayoutCache();
    syncAccessibility();
    emit fabAttachedChanged(value);
    emitAccessibilityIfChanged(previousSummary);
    updateGeometry();
    update();
}

QIcon QtMaterialBottomAppBar::navigationIcon() const
{
    return d->m_navigationIcon;
}

void QtMaterialBottomAppBar::setNavigationIcon(const QIcon& icon)
{
    const QString previousSummary = accessibilitySummary();
    d->m_navigationIcon = icon;
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
    return d->m_navigationAccessibleName;
}

void QtMaterialBottomAppBar::setNavigationAccessibleName(const QString& name)
{
    if (d->m_navigationAccessibleName == name) {
        return;
    }
    const QString previousSummary = accessibilitySummary();
    d->m_navigationAccessibleName = name;
    syncButtons();
    syncAccessibility();
    emit navigationAccessibleNameChanged(d->m_navigationAccessibleName);
    emitAccessibilityIfChanged(previousSummary);
}

int QtMaterialBottomAppBar::actionCount() const noexcept
{
    return d->m_actionButtons.size();
}

int QtMaterialBottomAppBar::addActionButton(const QIcon& icon, const QString& toolTip)
{
    const QString previousSummary = accessibilitySummary();
    const int index = d->m_actionButtons.size();
    auto* button = new QToolButton(this);
    button->setObjectName(QStringLiteral("qtmaterial_bottomAppBar_action_%1").arg(index));
    button->setAutoRaise(true);
    button->setIcon(icon);
    button->setToolTip(toolTip);
    button->setFocusPolicy(Qt::StrongFocus);
    button->setAccessibleName(toolTip.isEmpty() ? fallbackActionName(index) : toolTip);
    button->setAccessibleDescription(QStringLiteral("Bottom app bar action"));
    connect(button, &QToolButton::clicked, this, [this, index]() { emit actionTriggered(index); });
    d->m_actionButtons.push_back({button, index, toolTip});
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
    for (auto& entry : d->m_actionButtons) {
        if (entry.button) {
            entry.button->deleteLater();
        }
    }
    d->m_actionButtons.clear();
    invalidateLayoutCache();
    syncAccessibility();
    emitAccessibilityIfChanged(previousSummary);
    updateGeometry();
    update();
}

QString QtMaterialBottomAppBar::actionAccessibleName(int index) const
{
    if (index < 0 || index >= d->m_actionButtons.size()) {
        return QString();
    }
    const auto& entry = d->m_actionButtons.at(index);
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
    if (index < 0 || index >= d->m_actionButtons.size()) {
        return;
    }
    if (d->m_actionButtons[index].accessibleName == name) {
        return;
    }
    const QString previousSummary = accessibilitySummary();
    d->m_actionButtons[index].accessibleName = name;
    if (d->m_actionButtons[index].button) {
        d->m_actionButtons[index].button->setAccessibleName(actionAccessibleName(index));
        if (d->m_actionButtons[index].button->toolTip().isEmpty()) {
            d->m_actionButtons[index].button->setToolTip(actionAccessibleName(index));
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
    if (d->m_fabButton == button) {
        return;
    }
    const QString previousSummary = accessibilitySummary();
    if (d->m_fabButton) {
        d->m_fabButton->setParent(nullptr);
    }
    d->m_fabButton = button;
    if (d->m_fabButton) {
        d->m_fabButton->setParent(this);
        d->m_fabButton->setFocusPolicy(Qt::StrongFocus);
        if (d->m_fabButton->accessibleName().isEmpty()) {
            d->m_fabButton->setAccessibleName(QStringLiteral("Primary action"));
        }
        d->m_fabButton->setAccessibleDescription(QStringLiteral("Bottom app bar floating action button"));
        d->m_fabButton->raise();
    }
    invalidateLayoutCache();
    syncAccessibility();
    emitAccessibilityIfChanged(previousSummary);
    updateGeometry();
    update();
}

QAbstractButton* QtMaterialBottomAppBar::fabButton() const noexcept
{
    return d->m_fabButton;
}

QString QtMaterialBottomAppBar::fabAccessibleText() const
{
    if (!d->m_fabButton) {
        return QString();
    }
    const QString name = d->m_fabButton->accessibleName().isEmpty()
        ? d->m_fabButton->text()
        : d->m_fabButton->accessibleName();
    return name.isEmpty() ? QStringLiteral("Primary action") : name;
}

QString QtMaterialBottomAppBar::accessibilitySummary() const
{
    QStringList parts;
    parts << (d->m_title.isEmpty() ? QStringLiteral("Bottom app bar") : d->m_title);
    if (!d->m_navigationIcon.isNull()) {
        parts << effectiveNavigationAccessibleName();
    }
    if (!d->m_actionButtons.isEmpty()) {
        parts << tr("%n action(s)", nullptr, d->m_actionButtons.size());
    }
    if (d->m_fabButton) {
        parts << fabAccessibleText();
    }
    return parts.join(QStringLiteral(", "));
}

QSize QtMaterialBottomAppBar::sizeHint() const
{
    return QSize(kPreferredWidth, kDefaultHeight + (d->m_fabAttached ? 16 : 0));
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
    p.drawRect(d->m_visualRect);

    if (d->m_elevated) {
        p.setPen(outline);
        p.drawLine(d->m_visualRect.topLeft(), d->m_visualRect.topRight());
    }

    p.setPen(text);
    QFont titleFont = font();
    titleFont.setBold(true);
    p.setFont(titleFont);
    const QString elided = QFontMetrics(titleFont).elidedText(d->m_title, Qt::ElideRight, d->m_titleRect.width());
    const int horizontalAlignment = layoutDirection() == Qt::RightToLeft ? Qt::AlignRight : Qt::AlignLeft;
    p.drawText(d->m_titleRect, Qt::AlignVCenter | horizontalAlignment, elided);

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
        && d->m_navigationButton && d->m_navigationButton->isVisible()) {
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
    if (!d->m_layoutDirty) {
        return;
    }

    d->m_visualRect = rect().adjusted(0, d->m_fabAttached ? 8 : 0, 0, 0);
    if (d->m_visualRect.height() <= 0) {
        d->m_visualRect = QRect(0, 0, width(), sizeHint().height());
    }
    const QRect content = availableContentRect();
    int left = content.left();
    int right = content.right();
    const bool rtl = layoutDirection() == Qt::RightToLeft;

    d->m_navRect = QRect();
    if (d->m_navigationButton && d->m_navigationButton->isVisible()) {
        if (rtl) {
            d->m_navRect = QRect(right - kActionSlot + 1, content.center().y() - kActionSlot / 2, kActionSlot, kActionSlot);
            right = d->m_navRect.left() - kBetweenPadding;
        } else {
            d->m_navRect = QRect(left, content.center().y() - kActionSlot / 2, kActionSlot, kActionSlot);
            left = d->m_navRect.right() + kBetweenPadding;
        }
        d->m_navigationButton->setGeometry(d->m_navRect);
    }

    d->m_actionRects.clear();
    if (rtl) {
        int currentLeft = left;
        for (int i = 0; i < d->m_actionButtons.size(); ++i) {
            QRect slot(currentLeft, content.center().y() - kActionSlot / 2, kActionSlot, kActionSlot);
            d->m_actionRects.push_back(slot);
            currentLeft = slot.right() + kBetweenPadding;
            if (d->m_actionButtons[i].button) {
                d->m_actionButtons[i].button->setGeometry(slot);
            }
        }
        left = currentLeft;
    } else {
        int currentRight = right;
        for (int i = d->m_actionButtons.size() - 1; i >= 0; --i) {
            QRect slot(currentRight - kActionSlot + 1, content.center().y() - kActionSlot / 2, kActionSlot, kActionSlot);
            d->m_actionRects.prepend(slot);
            currentRight = slot.left() - kBetweenPadding;
            if (d->m_actionButtons[i].button) {
                d->m_actionButtons[i].button->setGeometry(slot);
            }
        }
        right = currentRight;
    }

    d->m_fabRect = QRect();
    if (d->m_fabAttached && d->m_fabButton) {
        d->m_fabRect = QRect(content.center().x() - kFabSlot / 2, d->m_visualRect.top() - 20, kFabSlot, kFabSlot);
        d->m_fabButton->setGeometry(d->m_fabRect);
        d->m_fabButton->raise();
        const int gap = (kFabSlot / 2) + kBetweenPadding;
        if (left < d->m_fabRect.center().x() && d->m_fabRect.center().x() < right) {
            if (layoutDirection() == Qt::RightToLeft) {
                left = std::max(left, d->m_fabRect.right() + gap);
            } else {
                right = std::min(right, d->m_fabRect.left() - gap);
            }
        }
    }

    d->m_titleRect = QRect(left, content.top(), std::max(0, right - left), content.height());
    d->m_layoutDirty = false;
}

void QtMaterialBottomAppBar::invalidateLayoutCache()
{
    d->m_layoutDirty = true;
    updateGeometry();
    update();
}

void QtMaterialBottomAppBar::syncButtons()
{
    if (!d->m_navigationButton) {
        d->m_navigationButton = new QToolButton(this);
        d->m_navigationButton->setObjectName(QStringLiteral("qtmaterial_bottomAppBar_navigationButton"));
        d->m_navigationButton->setAutoRaise(true);
        d->m_navigationButton->setFocusPolicy(Qt::StrongFocus);
        connect(d->m_navigationButton, &QToolButton::clicked, this, &QtMaterialBottomAppBar::navigationTriggered);
    }
    d->m_navigationButton->setVisible(!d->m_navigationIcon.isNull());
    d->m_navigationButton->setIcon(d->m_navigationIcon);
    d->m_navigationButton->setAccessibleName(effectiveNavigationAccessibleName());
    d->m_navigationButton->setAccessibleDescription(QStringLiteral("Bottom app bar navigation"));
    if (d->m_navigationButton->toolTip().isEmpty()) {
        d->m_navigationButton->setToolTip(effectiveNavigationAccessibleName());
    }

    auto syncButton = [this](QToolButton* button) {
        if (!button) {
            return;
        }
        button->setEnabled(isEnabled());
        button->setIconSize(QSize(24, 24));
        button->setStyleSheet(QStringLiteral("QToolButton { border: none; background: transparent; }"));
    };

    syncButton(d->m_navigationButton);
    for (int i = 0; i < d->m_actionButtons.size(); ++i) {
        auto& entry = d->m_actionButtons[i];
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
    if (d->m_fabButton) {
        d->m_fabButton->raise();
    }
}

void QtMaterialBottomAppBar::syncAccessibility()
{
    const QString name = d->m_title.isEmpty() ? QStringLiteral("Bottom app bar") : d->m_title;
    setAccessibleName(name);
    setAccessibleDescription(accessibilitySummary());
}

void QtMaterialBottomAppBar::emitAccessibilityIfChanged(const QString& previousSummary)
{
    const QString current = accessibilitySummary();
    if (current != previousSummary && current != d->m_lastAccessibilitySummary) {
        d->m_lastAccessibilitySummary = current;
        emit accessibilitySummaryChanged(current);
    }
}

QString QtMaterialBottomAppBar::effectiveNavigationAccessibleName() const
{
    if (!d->m_navigationAccessibleName.isEmpty()) {
        return d->m_navigationAccessibleName;
    }
    return QStringLiteral("Navigate");
}

QRect QtMaterialBottomAppBar::availableContentRect() const
{
    return d->m_visualRect.adjusted(kHorizontalPadding, 8, -kHorizontalPadding, -8);
}
