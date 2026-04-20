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

#include "qtmaterial/effects/qtmaterialfocusindicator.h"
// #include "qtmaterial/specs/qtmaterialbottomappbarspec.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
// #include "qtmaterial/widgets/surfaces/private/qtmaterialsurfacerenderhelper_p.h"
#include "private/qtmaterialsurfacerenderhelper_p.h"

namespace {
constexpr int kDefaultHeight = 80;
constexpr int kActionSlot = 48;
constexpr int kHorizontalPadding = 16;
constexpr int kFabSlot = 64;
constexpr int kBetweenPadding = 12;
}

QtMaterialBottomAppBar::QtMaterialBottomAppBar(QWidget* parent)
    : QtMaterialSurface(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
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
    m_title = title;
    invalidateLayoutCache();
    syncAccessibility();
    emit titleChanged(m_title);
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
    invalidateSpecCache();
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
    m_fabAttached = value;
    invalidateLayoutCache();
    emit fabAttachedChanged(value);
    updateGeometry();
    update();
}

QIcon QtMaterialBottomAppBar::navigationIcon() const
{
    return m_navigationIcon;
}

void QtMaterialBottomAppBar::setNavigationIcon(const QIcon& icon)
{
    m_navigationIcon = icon;
    syncButtons();
    invalidateLayoutCache();
    update();
}

void QtMaterialBottomAppBar::clearNavigationIcon()
{
    setNavigationIcon(QIcon());
}

int QtMaterialBottomAppBar::actionCount() const noexcept
{
    return m_actionButtons.size();
}

int QtMaterialBottomAppBar::addActionButton(const QIcon& icon, const QString& toolTip)
{
    const int index = m_actionButtons.size();
    auto* button = new QToolButton(this);
    button->setAutoRaise(true);
    button->setIcon(icon);
    button->setToolTip(toolTip);
    button->setFocusPolicy(Qt::StrongFocus);
    connect(button, &QToolButton::clicked, this, [this, index]() { emit actionTriggered(index); });
    m_actionButtons.push_back({button, index});
    invalidateLayoutCache();
    syncButtons();
    updateGeometry();
    update();
    return index;
}

void QtMaterialBottomAppBar::clearActionButtons()
{
    for (auto &entry : m_actionButtons) {
        if (entry.button) {
            entry.button->deleteLater();
        }
    }
    m_actionButtons.clear();
    invalidateLayoutCache();
    updateGeometry();
    update();
}

void QtMaterialBottomAppBar::setFabButton(QAbstractButton* button)
{
    if (m_fabButton == button) {
        return;
    }
    if (m_fabButton) {
        m_fabButton->setParent(nullptr);
    }
    m_fabButton = button;
    if (m_fabButton) {
        m_fabButton->setParent(this);
        m_fabButton->raise();
    }
    invalidateLayoutCache();
    updateGeometry();
    update();
}

QAbstractButton* QtMaterialBottomAppBar::fabButton() const noexcept
{
    return m_fabButton;
}

QSize QtMaterialBottomAppBar::sizeHint() const
{
    ensureSpecResolved();
    return QSize();
    // return QSize(360, std::max(kDefaultHeight, m_spec.containerHeight + (m_fabAttached ? 16 : 0)));
}

QSize QtMaterialBottomAppBar::minimumSizeHint() const
{
    ensureSpecResolved();
    return QSize();
//    return QSize(240, std::max(kDefaultHeight, m_spec.containerHeight + (m_fabAttached ? 16 : 0)));
}

void QtMaterialBottomAppBar::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    ensureSpecResolved();
    ensureLayoutResolved();

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    // QtMaterialSurfaceRenderHelper::paintSurface(
    //     &p,
    //     m_visualRect,
    //     m_containerPath,
    //     m_spec.containerColor,
    //     m_elevated ? m_spec.shadowColor : QColor(),
    //     m_elevated ? m_spec.shadowBlur : 0,
    //     m_elevated ? m_spec.shadowYOffset : 0
    // );

    // p.setPen(m_spec.titleColor);
    // p.setFont(m_spec.titleFont);
    // p.drawText(m_titleRect, Qt::AlignVCenter | Qt::AlignLeft, QFontMetrics(m_spec.titleFont).elidedText(m_title, Qt::ElideRight, m_titleRect.width()));

    // if (hasFocus()) {
    //     QtMaterialFocusIndicator::paintRectFocusRing(
    //         &p,
    //         m_visualRect.adjusted(1, 1, -1, -1),
    //         m_spec.focusRingColor,
    //         m_spec.cornerRadius,
    //         m_spec.focusRingWidth
    //     );
    // }
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
        break;
    default:
        break;
    }
}

void QtMaterialBottomAppBar::keyPressEvent(QKeyEvent* event)
{
    if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) && m_navigationButton && m_navigationButton->isVisible()) {
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
    invalidateSpecCache();
}

void QtMaterialBottomAppBar::stateChangedEvent()
{
    QtMaterialSurface::stateChangedEvent();
    update();
}

void QtMaterialBottomAppBar::ensureSpecResolved() const
{
    if (!m_specDirty) {
        return;
    }

    QtMaterial::SpecFactory factory;
    // m_spec = factory.bottomAppBarSpec(theme(), density());
    // if (m_elevated) {
    //     m_spec.shadowBlur = std::max(m_spec.shadowBlur, 24);
    //     m_spec.shadowYOffset = std::max(m_spec.shadowYOffset, 8);
    // }
    m_specDirty = false;
    m_layoutDirty = true;
}

void QtMaterialBottomAppBar::ensureLayoutResolved() const
{
    if (!m_layoutDirty) {
        return;
    }
    ensureSpecResolved();

    m_visualRect = rect().adjusted(0, m_fabAttached ? 8 : 0, 0, 0);
    // m_visualRect.setHeight(m_spec.containerHeight);
    if (m_visualRect.bottom() > rect().bottom()) {
        m_visualRect.moveBottom(rect().bottom());
    }
    // m_containerPath = QtMaterialSurfaceRenderHelper::roundedRectPath(m_visualRect, m_spec.cornerRadius);

    const QRect content = availableContentRect();

    int left = content.left();
    int right = content.right();

    m_navRect = QRect();
    if (m_navigationButton && m_navigationButton->isVisible()) {
        m_navRect = QRect(left, content.center().y() - kActionSlot / 2, kActionSlot, kActionSlot);
        left = m_navRect.right() + 1 + kBetweenPadding;
        m_navigationButton->setGeometry(m_navRect);
    }

    m_actionRects.clear();
    int currentRight = right;
    for (int i = m_actionButtons.size() - 1; i >= 0; --i) {
        QRect slot(currentRight - kActionSlot + 1, content.center().y() - kActionSlot / 2, kActionSlot, kActionSlot);
        m_actionRects.prepend(slot);
        currentRight = slot.left() - kBetweenPadding;
        if (m_actionButtons[i].button) {
            m_actionButtons[i].button->setGeometry(slot);
        }
    }

    m_fabRect = QRect();
    if (m_fabAttached && m_fabButton) {
        m_fabRect = QRect(content.center().x() - kFabSlot / 2, m_visualRect.top() - 20, kFabSlot, kFabSlot);
        m_fabButton->setGeometry(m_fabRect);
        const int halfGap = (kFabSlot / 2) + kBetweenPadding;
        if (left < m_fabRect.center().x()) {
            currentRight = std::min(currentRight, m_fabRect.left() - kBetweenPadding);
            if (left + 120 > currentRight) {
                left = content.left();
            }
        }
    }

    const int titleRight = std::max(left, currentRight);
    m_titleRect = QRect(left, content.top(), std::max(0, titleRight - left), content.height());
    m_layoutDirty = false;
}

void QtMaterialBottomAppBar::invalidateSpecCache()
{
    m_specDirty = true;
    invalidateLayoutCache();
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
        m_navigationButton->setAutoRaise(true);
        m_navigationButton->setFocusPolicy(Qt::StrongFocus);
        connect(m_navigationButton, &QToolButton::clicked, this, &QtMaterialBottomAppBar::navigationTriggered);
    }

    m_navigationButton->setVisible(!m_navigationIcon.isNull());
    m_navigationButton->setIcon(m_navigationIcon);

    for (auto &entry : m_actionButtons) {
        if (entry.button) {
            entry.button->setVisible(true);
            entry.button->raise();
        }
    }

    if (m_fabButton) {
        m_fabButton->raise();
    }
}

void QtMaterialBottomAppBar::syncAccessibility()
{
    if (accessibleName().isEmpty()) {
        setAccessibleName(m_title);
    }
    setAccessibleDescription(QStringLiteral("Bottom app bar"));
}

QRect QtMaterialBottomAppBar::availableContentRect() const
{
    return m_visualRect.adjusted(kHorizontalPadding, 8, -kHorizontalPadding, -8);
}
