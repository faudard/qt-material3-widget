#include "qtmaterial/widgets/surfaces/qtmaterialnavigationdrawer.h"

#include <QEvent>
#include <QHideEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QShowEvent>
#include <QWidget>

namespace QtMaterial {

QtMaterialNavigationDrawer::QtMaterialNavigationDrawer(QWidget* parent)
    : QtMaterialOverlaySurface(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setAutoFillBackground(true);
    hide();
}

QtMaterialNavigationDrawer::~QtMaterialNavigationDrawer() = default;

QSize QtMaterialNavigationDrawer::sizeHint() const
{
    const QWidget* host = hostWidget() ? hostWidget() : parentWidget();
    return QSize(360, host ? host->height() : 640);
}

QSize QtMaterialNavigationDrawer::minimumSizeHint() const
{
    return QSize(240, 160);
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
    m_open = true;
    show();
    raise();
    focusInitialChild();
}

void QtMaterialNavigationDrawer::closeDrawer()
{
    m_open = false;
    hide();
}

bool QtMaterialNavigationDrawer::isOpen() const noexcept
{
    return m_open;
}

void QtMaterialNavigationDrawer::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(rect(), palette().window());
}

void QtMaterialNavigationDrawer::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    invalidateLayoutCache();
}

void QtMaterialNavigationDrawer::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    m_open = true;
}

void QtMaterialNavigationDrawer::hideEvent(QHideEvent* event)
{
    QWidget::hideEvent(event);
    m_open = false;
}

void QtMaterialNavigationDrawer::changeEvent(QEvent* event)
{
    QWidget::changeEvent(event);
    update();
}

void QtMaterialNavigationDrawer::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        closeDrawer();
        event->accept();
        return;
    }

    QWidget::keyPressEvent(event);
}

void QtMaterialNavigationDrawer::themeChangedEvent(const QtMaterial::Theme&)
{
    m_specDirty = true;
    update();
}

void QtMaterialNavigationDrawer::invalidateResolvedSpec()
{
    m_specDirty = true;
    update();
}

void QtMaterialNavigationDrawer::ensureSpecResolved() const
{
    m_specDirty = false;
}

void QtMaterialNavigationDrawer::ensureLayoutResolved() const
{
    m_layoutDirty = false;
}

void QtMaterialNavigationDrawer::invalidateLayoutCache()
{
    m_layoutDirty = true;
}

void QtMaterialNavigationDrawer::updateScrimVisuals()
{
}

void QtMaterialNavigationDrawer::focusInitialChild()
{
    if (QWidget* child = firstFocusableChild()) {
        child->setFocus(Qt::OtherFocusReason);
    }
}

QWidget* QtMaterialNavigationDrawer::firstFocusableChild() const
{
    const auto children = findChildren<QWidget*>();
    for (QWidget* child : children) {
        if (child && child->isVisible() && child->isEnabled() && child->focusPolicy() != Qt::NoFocus) {
            return child;
        }
    }
    return nullptr;
}

} // namespace QtMaterial