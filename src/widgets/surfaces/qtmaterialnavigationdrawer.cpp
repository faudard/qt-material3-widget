#include "qtmaterial/widgets/surfaces/qtmaterialnavigationdrawer.h"

#include <QEvent>
#include <QHideEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QShowEvent>
#include <QWidget>
#include <memory>
#include <QPainterPath>
#include "qtmaterial/specs/qtmaterialnavigationdrawerspec.h"

namespace QtMaterial {


class QtMaterialNavigationDrawerPrivate {
public:
 QtMaterialNavigationDrawer::Edge edge = QtMaterialNavigationDrawer::Edge::Left;
 bool open = false;
 mutable bool specDirty = true;
 mutable bool layoutDirty = true;
 mutable QtMaterial::NavigationDrawerSpec spec;
 mutable QRect cachedVisualRect;
 mutable QRect cachedContentRect;
 mutable QPainterPath cachedContainerPath;
 mutable qreal cachedCornerRadius = 0.0;
 mutable int cachedShadowMargin = 0;
 QPointer<QtMaterialScrimWidget> scrim;
 QtMaterialTransitionController* transition = nullptr;
};

namespace {

void ensureNavigationDrawerSpecResolved(const QtMaterialNavigationDrawerPrivate* d)
{
    if (!d) {
        return;
    }
    d->specDirty = false;
}

void ensureNavigationDrawerLayoutResolved(const QtMaterialNavigationDrawerPrivate* d)
{
    if (!d) {
        return;
    }
    d->layoutDirty = false;
}

void invalidateNavigationDrawerLayoutCache(QtMaterialNavigationDrawerPrivate* d)
{
    if (!d) {
        return;
    }
    d->layoutDirty = true;
}

void updateNavigationDrawerScrimVisuals(QtMaterialNavigationDrawerPrivate*)
{
}

QWidget* firstNavigationDrawerFocusableChild(const QtMaterialNavigationDrawer* drawer)
{
    if (!drawer) {
        return nullptr;
    }

    const auto children = drawer->findChildren<QWidget*>();
    for (QWidget* child : children) {
        if (child && child->isVisible() && child->isEnabled() && child->focusPolicy() != Qt::NoFocus) {
            return child;
        }
    }
    return nullptr;
}

void focusNavigationDrawerInitialChild(QtMaterialNavigationDrawer* drawer)
{
    if (QWidget* child = firstNavigationDrawerFocusableChild(drawer)) {
        child->setFocus(Qt::OtherFocusReason);
    }
}

} // namespace

QtMaterialNavigationDrawer::QtMaterialNavigationDrawer(QWidget* parent)
    : QtMaterialOverlaySurface(parent)
    , d_ptr(std::make_unique<QtMaterialNavigationDrawerPrivate>())
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
    if (d_ptr->edge == edge) {
        return;
    }

    d_ptr->edge = edge;
    invalidateNavigationDrawerLayoutCache(d_ptr.get());
    update();
}

QtMaterialNavigationDrawer::Edge QtMaterialNavigationDrawer::edge() const noexcept
{
    return d_ptr->edge;
}

void QtMaterialNavigationDrawer::open()
{
    d_ptr->open = true;
    show();
    raise();
    focusNavigationDrawerInitialChild(this);
}

void QtMaterialNavigationDrawer::closeDrawer()
{
    d_ptr->open = false;
    hide();
}

bool QtMaterialNavigationDrawer::isOpen() const noexcept
{
    return d_ptr->open;
}

void QtMaterialNavigationDrawer::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(rect(), palette().window());
}

void QtMaterialNavigationDrawer::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    invalidateNavigationDrawerLayoutCache(d_ptr.get());
}

void QtMaterialNavigationDrawer::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    d_ptr->open = true;
}

void QtMaterialNavigationDrawer::hideEvent(QHideEvent* event)
{
    QWidget::hideEvent(event);
    d_ptr->open = false;
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
    d_ptr->specDirty = true;
    update();
}

void QtMaterialNavigationDrawer::invalidateResolvedSpec()
{
    d_ptr->specDirty = true;
    update();
}

} // namespace QtMaterial