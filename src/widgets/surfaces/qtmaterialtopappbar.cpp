#include "qtmaterial/widgets/surfaces/qtmaterialtopappbar.h"

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
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/specs/qtmaterialtopappbarspec.h"
#include "qtmaterial/widgets/surfaces/private/qtmaterialsurfacerenderhelper_p.h"

namespace {
constexpr int kDefaultHeight = 64;
constexpr int kActionSlot = 48;
constexpr int kHorizontalPadding = 16;
constexpr int kBetweenPadding = 12;
}

QtMaterialTopAppBar::QtMaterialTopAppBar(QWidget* parent)
    : QtMaterialSurface(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
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
    m_title = title;
    invalidateLayoutCache();
    syncAccessibility();
    emit titleChanged(m_title);
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
    invalidateSpecCache();
    emit elevatedChanged(value);
    update();
}

QIcon QtMaterialTopAppBar::navigationIcon() const
{
    return m_navigationIcon;
}

void QtMaterialTopAppBar::setNavigationIcon(const QIcon& icon)
{
    m_navigationIcon = icon;
    syncButtons();
    invalidateLayoutCache();
    update();
}

void QtMaterialTopAppBar::clearNavigationIcon()
{
    setNavigationIcon(QIcon());
}

int QtMaterialTopAppBar::actionCount() const noexcept
{
    return m_actionButtons.size();
}

int QtMaterialTopAppBar::addActionButton(const QIcon& icon, const QString& toolTip)
{
    const int index = m_actionButtons.size();
    auto *button = new QToolButton(this);
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

void QtMaterialTopAppBar::clearActionButtons()
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

QSize QtMaterialTopAppBar::sizeHint() const
{
    ensureSpecResolved();
    return QSize(320, std::max(kDefaultHeight, m_spec.containerHeight));
}

QSize QtMaterialTopAppBar::minimumSizeHint() const
{
    ensureSpecResolved();
    return QSize(200, std::max(kDefaultHeight, m_spec.containerHeight));
}

void QtMaterialTopAppBar::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    ensureSpecResolved();
    ensureLayoutResolved();

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    QtMaterialSurfaceRenderHelper::paintSurface(
        &p,
        m_visualRect,
        m_containerPath,
        m_spec.containerColor,
        m_elevated ? m_spec.shadowColor : QColor(),
        m_elevated ? m_spec.shadowBlurRadius : 0,
        m_elevated ? m_spec.shadowYOffset : 0
    );

    p.setPen(m_spec.titleColor);
    p.setFont(m_spec.titleFont);
    const QString elided = QFontMetrics(m_spec.titleFont).elidedText(
        m_title,
        Qt::ElideRight,
        m_titleRect.width()
    );
    p.drawText(m_titleRect, Qt::AlignVCenter | (m_centeredTitle ? Qt::AlignHCenter : Qt::AlignLeft), elided);

    if (hasFocus()) {
        QtMaterialFocusIndicator::paintRectFocusRing(
            &p,
            QRectF(m_visualRect).adjusted(1, 1, -1, -1),
            m_spec.focusRingColor,
            m_spec.cornerRadius,
            m_spec.focusRingWidth
        );
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
        invalidateSpecCache();
        syncButtons();
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
    if ((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return || event->key() == Qt::Key_Select)
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
    invalidateSpecCache();
    syncButtons();
}

void QtMaterialTopAppBar::stateChangedEvent()
{
    QtMaterialSurface::stateChangedEvent();
    update();
}

void QtMaterialTopAppBar::ensureSpecResolved() const
{
    if (!m_specDirty) {
        return;
    }

    QtMaterial::SpecFactory factory;
    m_spec = factory.topAppBarSpec(theme(), density());
    if (m_elevated) {
        m_spec.elevationRole = QtMaterial::ElevationRole::Level2;
    }
    m_specDirty = false;
}

void QtMaterialTopAppBar::ensureLayoutResolved() const
{
    if (!m_layoutDirty) {
        return;
    }

    ensureSpecResolved();

    m_visualRect = rect();
    m_containerPath = QPainterPath();
    m_containerPath.addRoundedRect(QRectF(m_visualRect), m_spec.cornerRadius, m_spec.cornerRadius);

    QRect content = m_visualRect.adjusted(kHorizontalPadding, 0, -kHorizontalPadding, 0);
    const int top = (m_visualRect.height() - kActionSlot) / 2;

    if (m_navigationButton && !m_navigationIcon.isNull()) {
        m_navRect = QRect(content.left(), top, kActionSlot, kActionSlot);
        content.setLeft(m_navRect.right() + kBetweenPadding);
    } else {
        m_navRect = QRect();
    }

    m_actionRects.clear();
    int right = content.right();
    for (int i = m_actionButtons.size() - 1; i >= 0; --i) {
        QRect r(right - kActionSlot + 1, top, kActionSlot, kActionSlot);
        m_actionRects.prepend(r);
        right = r.left() - kBetweenPadding;
    }
    content.setRight(right);

    if (m_centeredTitle) {
        const int titleWidth = std::max(0, content.width());
        m_titleRect = QRect(content.left(), 0, titleWidth, m_visualRect.height());
    } else {
        m_titleRect = QRect(content.left(), 0, std::max(0, content.width()), m_visualRect.height());
    }

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

void QtMaterialTopAppBar::invalidateSpecCache()
{
    m_specDirty = true;
    invalidateLayoutCache();
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
            auto *button = new QToolButton(this);
            button->setAutoRaise(true);
            button->setFocusPolicy(Qt::StrongFocus);
            connect(button, &QToolButton::clicked, this, &QtMaterialTopAppBar::navigationTriggered);
            m_navigationButton = button;
        }
        m_navigationButton->setIcon(m_navigationIcon);
        m_navigationButton->show();
    } else if (m_navigationButton) {
        m_navigationButton->hide();
    }

    ensureSpecResolved();

    auto syncButton = [this](QToolButton* b) {
        if (!b) return;
        b->setEnabled(isEnabled());
        b->setIconSize(QSize(m_spec.actionIconSize, m_spec.actionIconSize));
        b->setStyleSheet(QStringLiteral("QToolButton { border: none; background: transparent; }"));
    };

    syncButton(m_navigationButton);
    for (auto &entry : m_actionButtons) {
        syncButton(entry.button);
    }
}

void QtMaterialTopAppBar::syncAccessibility()
{
    if (accessibleName().isEmpty()) {
        setAccessibleName(m_title);
    }
}

QRect QtMaterialTopAppBar::availableContentRect() const
{
    return m_titleRect;
}
