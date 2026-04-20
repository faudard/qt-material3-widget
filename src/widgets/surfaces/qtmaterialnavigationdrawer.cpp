#include "qtmaterial/widgets/surfaces/qtmaterialnavigationdrawer.h"

#include <QEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QShowEvent>
#include <QHideEvent>
#include <QWidget>

#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialscrimwidget.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/widgets/surfaces/private/qtmaterialsurfacerenderhelper_p.h"

namespace {

QWidget* firstFocusableDescendant(QWidget* root)
{
    if (!root) {
        return nullptr;
    }

    const auto children = root->findChildren<QWidget*>();
    for (QWidget* child : children) {
        if (!child->isVisible() || !child->isEnabled()) {
            continue;
        }
        if (child->focusPolicy() == Qt::NoFocus) {
            continue;
        }
        return child;
    }
    return nullptr;
}

} // namespace

QtMaterialNavigationDrawer::QtMaterialNavigationDrawer(QWidget* parent)
    : QtMaterialOverlaySurface(parent)
    , m_scrim(new QtMaterialScrimWidget(parent ? parent : this))
    , m_transition(new QtMaterialTransitionController(this))
{
    setFocusPolicy(Qt::StrongFocus);
    setVisible(false);

    if (m_scrim) {
        m_scrim->hide();
        m_scrim->setVisible(false);
    }

    connect(m_transition, &QtMaterialTransitionController::progressChanged,
            this, [this](qreal) {
                updateScrimVisuals();
                update();
            });
}

QtMaterialNavigationDrawer::~QtMaterialNavigationDrawer() = default;

QSize QtMaterialNavigationDrawer::sizeHint() const
{
    ensureSpecResolved();
    const int width = qMax(m_spec.minWidth, qMin(m_spec.maxWidth, 360));
    const int height = qMax(240, parentWidget() ? parentWidget()->height() : 480);
    return QSize(width, height);
}

QSize QtMaterialNavigationDrawer::minimumSizeHint() const
{
    ensureSpecResolved();
    return QSize(m_spec.minWidth, 240);
}

void QtMaterialNavigationDrawer::setEdge(Edge edge)
{
    if (m_edge == edge) {
        return;
    }
    m_edge = edge;
    invalidateLayoutCache();
    update();
}

QtMaterialNavigationDrawer::Edge QtMaterialNavigationDrawer::edge() const noexcept
{
    return m_edge;
}

void QtMaterialNavigationDrawer::open()
{
    ensureSpecResolved();
    ensureLayoutResolved();
    m_open = true;
    setVisible(true);
    raise();
    if (m_scrim) {
        m_scrim->setVisible(true);
        m_scrim->raise();
    }
    m_transition->setDuration(240);
    m_transition->startForward();
    updateScrimVisuals();
    focusInitialChild();
}

void QtMaterialNavigationDrawer::closeDrawer()
{
    m_open = false;
    m_transition->setDuration(180);
    m_transition->startBackward();
    hide();
    if (m_scrim) {
        m_scrim->hide();
    }
}

bool QtMaterialNavigationDrawer::isOpen() const noexcept
{
    return m_open;
}

void QtMaterialNavigationDrawer::paintEvent(QPaintEvent*)
{
    ensureSpecResolved();
    ensureLayoutResolved();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QColor fill = isEnabled() ? m_spec.containerColor : m_spec.disabledContainerColor;
    QtMaterialSurfaceRenderHelper::paintSurface(
        &painter,
        m_cachedContainerPath,
        fill,
        m_spec.shadowColor,
        m_spec.shadowBlurRadius,
        m_spec.shadowYOffset
    );

    if (hasFocus()) {
        QtMaterialFocusIndicator::paintPathFocusRing(
            &painter,
            m_cachedContainerPath,
            m_spec.focusRingColor,
            m_spec.focusRingWidth
        );
    }
}

void QtMaterialNavigationDrawer::resizeEvent(QResizeEvent* event)
{
    QtMaterialOverlaySurface::resizeEvent(event);
    Q_UNUSED(event)
    invalidateLayoutCache();
}

void QtMaterialNavigationDrawer::showEvent(QShowEvent* event)
{
    QtMaterialOverlaySurface::showEvent(event);
    updateScrimVisuals();
}

void QtMaterialNavigationDrawer::hideEvent(QHideEvent* event)
{
    QtMaterialOverlaySurface::hideEvent(event);
    if (m_scrim) {
        m_scrim->hide();
    }
}

void QtMaterialNavigationDrawer::changeEvent(QEvent* event)
{
    QtMaterialOverlaySurface::changeEvent(event);
    switch (event->type()) {
    case QEvent::EnabledChange:
    case QEvent::StyleChange:
    case QEvent::FontChange:
    case QEvent::LayoutDirectionChange:
        invalidateLayoutCache();
        update();
        break;
    default:
        break;
    }
}

void QtMaterialNavigationDrawer::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        event->accept();
        closeDrawer();
        return;
    }

    if ((m_edge == Edge::Left && event->key() == Qt::Key_Left) ||
        (m_edge == Edge::Right && event->key() == Qt::Key_Right)) {
        event->accept();
        closeDrawer();
        return;
    }

    QtMaterialOverlaySurface::keyPressEvent(event);
}

void QtMaterialNavigationDrawer::themeChangedEvent(const QtMaterial::Theme& theme)
{
    QtMaterialOverlaySurface::themeChangedEvent(theme);
    Q_UNUSED(theme)
    m_specDirty = true;
    invalidateLayoutCache();
    updateScrimVisuals();
}

void QtMaterialNavigationDrawer::invalidateResolvedSpec()
{
    m_specDirty = true;
    invalidateLayoutCache();
}

void QtMaterialNavigationDrawer::ensureSpecResolved() const
{
    if (!m_specDirty) {
        return;
    }

    QtMaterial::SpecFactory factory;
    m_spec = factory.navigationDrawerSpec(QtMaterial::ThemeManager::instance().theme(), density());
    m_specDirty = false;
    m_layoutDirty = true;
}

void QtMaterialNavigationDrawer::ensureLayoutResolved() const
{
    ensureSpecResolved();
    if (!m_layoutDirty) {
        return;
    }

    const QRect outer = rect();
    m_cachedShadowMargin = m_spec.shadowMargin;

    const int width = qMin(m_spec.maxWidth, qMax(m_spec.minWidth, outer.width() - m_cachedShadowMargin));
    const int x = (m_edge == Edge::Left)
        ? outer.left() + m_cachedShadowMargin
        : outer.right() - width - m_cachedShadowMargin + 1;

    m_cachedVisualRect = QRect(x, outer.top() + m_cachedShadowMargin, width,
                               qMax(outer.height() - 2 * m_cachedShadowMargin, 0));
    m_cachedContentRect = m_cachedVisualRect.adjusted(
        m_spec.horizontalPadding,
        m_spec.verticalPadding,
        -m_spec.horizontalPadding,
        -m_spec.verticalPadding
    );
    m_cachedCornerRadius = m_spec.cornerRadius;

    QPainterPath path;
    path.addRoundedRect(QRectF(m_cachedVisualRect), m_cachedCornerRadius, m_cachedCornerRadius);
    m_cachedContainerPath = path;
    m_layoutDirty = false;
}

void QtMaterialNavigationDrawer::invalidateLayoutCache()
{
    m_layoutDirty = true;
}

void QtMaterialNavigationDrawer::updateScrimVisuals()
{
    if (!m_scrim) {
        return;
    }

    ensureSpecResolved();
    QColor scrim = m_spec.scrimColor;
    scrim.setAlphaF(scrim.alphaF() * m_transition->progress());
    m_scrim->setGeometry(parentWidget() ? parentWidget()->rect() : rect());
    m_scrim->setColor(scrim);
    if (isVisible()) {
        m_scrim->show();
        m_scrim->lower();
    }
}

void QtMaterialNavigationDrawer::focusInitialChild()
{
    if (QWidget* child = firstFocusableChild()) {
        child->setFocus(Qt::OtherFocusReason);
        return;
    }
    setFocus(Qt::OtherFocusReason);
}

QWidget* QtMaterialNavigationDrawer::firstFocusableChild() const
{
    return firstFocusableDescendant(const_cast<QtMaterialNavigationDrawer*>(this));
}
