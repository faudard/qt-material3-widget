#include "qtmaterial/widgets/surfaces/qtmaterialtopappbar.h"

#include <QApplication>
#include <QEvent>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QToolButton>

#include <memory>
#include <QPainterPath>
#include "qtmaterial/effects/qtmaterialelevationrenderer.h"
#include "qtmaterial/specs/qtmaterialappbarspecresolver.h"
#include "qtmaterial/specs/qtmaterialappbarspec.h"
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

struct QtMaterialTopAppBarPrivate
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
    bool m_centeredTitle = false;
    bool m_elevated = false;

    mutable bool m_layoutDirty = true;
    mutable QtMaterial::AppBarSpec m_spec;
    mutable bool m_specDirty = true;
    mutable QRect m_visualRect;
    mutable QRect m_navRect;
    mutable QRect m_titleRect;
    mutable QVector<QRect> m_actionRects;
    mutable QPainterPath m_containerPath;

    QPointer<QToolButton> m_navigationButton;
    QVector<ActionButtonEntry> m_actionButtons;
    QString m_lastAccessibilitySummary;
};

QtMaterialTopAppBar::QtMaterialTopAppBar(QWidget* parent)
    : QtMaterialSurface(parent)
    , d(std::make_unique<QtMaterialTopAppBarPrivate>())
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
    return d->m_title;
}

void QtMaterialTopAppBar::setTitle(const QString& title)
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

bool QtMaterialTopAppBar::centeredTitle() const noexcept
{
    return d->m_centeredTitle;
}

void QtMaterialTopAppBar::setCenteredTitle(bool value)
{
    if (d->m_centeredTitle == value) {
        return;
    }

    d->m_centeredTitle = value;
    invalidateLayoutCache();
    emit centeredTitleChanged(value);
    update();
}

bool QtMaterialTopAppBar::elevated() const noexcept
{
    return d->m_elevated;
}

void QtMaterialTopAppBar::setElevated(bool value)
{
    if (d->m_elevated == value) {
        return;
    }

    d->m_elevated = value;
    emit elevatedChanged(value);
    update();
}

QIcon QtMaterialTopAppBar::navigationIcon() const
{
    return d->m_navigationIcon;
}

void QtMaterialTopAppBar::setNavigationIcon(const QIcon& icon)
{
    const QString previousSummary = accessibilitySummary();
    d->m_navigationIcon = icon;
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
    return d->m_navigationAccessibleName;
}

void QtMaterialTopAppBar::setNavigationAccessibleName(const QString& name)
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

int QtMaterialTopAppBar::actionCount() const noexcept
{
    return d->m_actionButtons.size();
}

int QtMaterialTopAppBar::addActionButton(const QIcon& icon, const QString& toolTip)
{
    const QString previousSummary = accessibilitySummary();
    const int index = d->m_actionButtons.size();
    auto* button = new QToolButton(this);
    button->setObjectName(QStringLiteral("qtmaterial_topAppBar_action_%1").arg(index));
    button->setAutoRaise(true);
    button->setIcon(icon);
    button->setToolTip(toolTip);
    button->setFocusPolicy(Qt::StrongFocus);
    button->setAccessibleName(toolTip.isEmpty() ? fallbackActionName(index) : toolTip);
    button->setAccessibleDescription(QStringLiteral("Top app bar action"));
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

void QtMaterialTopAppBar::clearActionButtons()
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

QString QtMaterialTopAppBar::actionAccessibleName(int index) const
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

void QtMaterialTopAppBar::setActionAccessibleName(int index, const QString& name)
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
    parts << (d->m_title.isEmpty() ? QStringLiteral("Top app bar") : d->m_title);
    if (!d->m_navigationIcon.isNull()) {
        parts << effectiveNavigationAccessibleName();
    }
    if (!d->m_actionButtons.isEmpty()) {
        parts << tr("%n action(s)", nullptr, d->m_actionButtons.size());
    }
    return parts.join(QStringLiteral(", "));
}

QSize QtMaterialTopAppBar::sizeHint() const
{
    ensureSpecResolved();
    const int extraHeight =
        0;
    return QSize(
        d->m_spec.preferredWidth,
        d->m_spec.preferredHeight + extraHeight);
}

QSize QtMaterialTopAppBar::minimumSizeHint() const
{
    ensureSpecResolved();
    return QSize(
        d->m_spec.minimumWidth,
        d->m_spec.preferredHeight);
}

void QtMaterialTopAppBar::paintEvent(
    QPaintEvent* event)
{
    Q_UNUSED(event)

    ensureSpecResolved();
    ensureLayoutResolved();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPainterPath containerPath;
    containerPath.addRect(QRectF(d->m_visualRect));

    if (d->m_elevated
        && d->m_spec.hasResolvedElevatedElevationStyle) {
        QtMaterialElevationRenderer::paintPathElevation(
            &painter,
            containerPath,
            d->m_spec.shadowColor,
            d->m_spec.elevatedElevationStyle);
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(d->m_spec.containerColor);
    painter.drawPath(containerPath);

    painter.setPen(d->m_spec.titleColor);
    const QFont titleFont =
        d->m_spec.hasResolvedTitleFont
        ? d->m_spec.titleFont
        : font();
    painter.setFont(titleFont);

    const QString elided =
        QFontMetrics(titleFont).elidedText(
            d->m_title,
            Qt::ElideRight,
            d->m_titleRect.width());
    const int horizontalAlignment =
        d->m_centeredTitle
        ? Qt::AlignHCenter
        : (
            layoutDirection() == Qt::RightToLeft
            ? Qt::AlignRight
            : Qt::AlignLeft);

    painter.drawText(
        d->m_titleRect,
        Qt::AlignVCenter | horizontalAlignment,
        elided);

    if (hasFocus()) {
        QPen focusPen(
            d->m_spec.focusRingColor,
            d->m_spec.focusRingWidth);
        focusPen.setStyle(Qt::DashLine);
        painter.setPen(focusPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(
            QRectF(rect()).adjusted(2, 2, -2, -2));
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
        && d->m_navigationButton && d->m_navigationButton->isVisible()) {
        emit navigationTriggered();
        event->accept();
        return;
    }

    QtMaterialSurface::keyPressEvent(event);
}

void QtMaterialTopAppBar::themeChangedEvent(
    const QtMaterial::Theme& changedTheme)
{
    QtMaterialSurface::themeChangedEvent(changedTheme);
    d->m_specDirty = true;
    d->m_layoutDirty = true;
    ensureSpecResolved();
    syncButtons();
    updateGeometry();
    update();
}

void QtMaterialTopAppBar::stateChangedEvent()
{
    QtMaterialSurface::stateChangedEvent();
    update();
}

void QtMaterialTopAppBar::ensureLayoutResolved() const
{
    ensureSpecResolved();
    if (!d->m_layoutDirty) {
        return;
    }

    d->m_visualRect = rect();
    QRect content = d->m_visualRect.adjusted(d->m_spec.contentPadding.left(), 0, -d->m_spec.contentPadding.right(), 0);
    const int top = d->m_visualRect.center().y() - d->m_spec.actionSlot / 2;
    const bool rtl = layoutDirection() == Qt::RightToLeft;

    d->m_navRect = QRect();
    if (d->m_navigationButton && !d->m_navigationIcon.isNull()) {
        if (rtl) {
            d->m_navRect = QRect(content.right() - d->m_spec.actionSlot + 1, top, d->m_spec.actionSlot, d->m_spec.actionSlot);
            content.setRight(d->m_navRect.left() - d->m_spec.betweenSpacing);
        } else {
            d->m_navRect = QRect(content.left(), top, d->m_spec.actionSlot, d->m_spec.actionSlot);
            content.setLeft(d->m_navRect.right() + d->m_spec.betweenSpacing);
        }
    }

    d->m_actionRects.clear();
    if (rtl) {
        int left = content.left();
        for (int i = 0; i < d->m_actionButtons.size(); ++i) {
            QRect r(left, top, d->m_spec.actionSlot, d->m_spec.actionSlot);
            d->m_actionRects.push_back(r);
            left = r.right() + d->m_spec.betweenSpacing;
        }
        content.setLeft(left);
    } else {
        int right = content.right();
        for (int i = d->m_actionButtons.size() - 1; i >= 0; --i) {
            QRect r(right - d->m_spec.actionSlot + 1, top, d->m_spec.actionSlot, d->m_spec.actionSlot);
            d->m_actionRects.prepend(r);
            right = r.left() - d->m_spec.betweenSpacing;
        }
        content.setRight(right);
    }

    d->m_titleRect = QRect(content.left(), d->m_visualRect.top(), std::max(0, content.width()), d->m_visualRect.height());

    if (d->m_navigationButton) {
        d->m_navigationButton->setGeometry(d->m_navRect);
    }
    for (int i = 0; i < d->m_actionButtons.size() && i < d->m_actionRects.size(); ++i) {
        if (d->m_actionButtons[i].button) {
            d->m_actionButtons[i].button->setGeometry(d->m_actionRects[i]);
        }
    }

    d->m_layoutDirty = false;
}

void QtMaterialTopAppBar::invalidateLayoutCache()
{
    d->m_layoutDirty = true;
    updateGeometry();
    update();
}

void QtMaterialTopAppBar::syncButtons()
{
    ensureSpecResolved();
    if (!d->m_navigationIcon.isNull()) {
        if (!d->m_navigationButton) {
            auto* button = new QToolButton(this);
            button->setObjectName(QStringLiteral("qtmaterial_topAppBar_navigationButton"));
            button->setAutoRaise(true);
            button->setFocusPolicy(Qt::StrongFocus);
            connect(button, &QToolButton::clicked, this, &QtMaterialTopAppBar::navigationTriggered);
            d->m_navigationButton = button;
        }
        d->m_navigationButton->setIcon(d->m_navigationIcon);
        d->m_navigationButton->setAccessibleName(effectiveNavigationAccessibleName());
        d->m_navigationButton->setAccessibleDescription(QStringLiteral("Top app bar navigation"));
        if (d->m_navigationButton->toolTip().isEmpty()) {
            d->m_navigationButton->setToolTip(effectiveNavigationAccessibleName());
        }
        d->m_navigationButton->show();
    } else if (d->m_navigationButton) {
        d->m_navigationButton->hide();
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
    const QString name = d->m_title.isEmpty() ? QStringLiteral("Top app bar") : d->m_title;
    setAccessibleName(name);
    setAccessibleDescription(accessibilitySummary());
}

void QtMaterialTopAppBar::emitAccessibilityIfChanged(const QString& previousSummary)
{
    const QString current = accessibilitySummary();
    if (current != previousSummary && current != d->m_lastAccessibilitySummary) {
        d->m_lastAccessibilitySummary = current;
        emit accessibilitySummaryChanged(current);
    }
}

QString QtMaterialTopAppBar::effectiveNavigationAccessibleName() const
{
    if (!d->m_navigationAccessibleName.isEmpty()) {
        return d->m_navigationAccessibleName;
    }
    return QStringLiteral("Navigate");
}

QRect QtMaterialTopAppBar::availableContentRect() const
{
    ensureLayoutResolved();
    return d->m_titleRect;
}

void QtMaterialTopAppBar::ensureSpecResolved() const
{
    if (!d->m_specDirty) {
        return;
    }

    const QtMaterial::AppBarSpecResolver resolver;
    d->m_spec = resolver.topAppBarSpec(theme());
    d->m_specDirty = false;
}

const QtMaterial::AppBarSpec&
QtMaterialTopAppBar::resolvedSpec() const
{
    ensureSpecResolved();
    return d->m_spec;
}
