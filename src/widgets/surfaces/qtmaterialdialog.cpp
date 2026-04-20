#include "qtmaterial/widgets/surfaces/qtmaterialdialog.h"

#include <QApplication>
#include <QEvent>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QShowEvent>
#include <QHideEvent>
#include <QStyleOption>
#include <QVBoxLayout>
#include <QWidget>

#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialshadowrenderer.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"

namespace {

QWidget* firstFocusableDescendant(QWidget* root)
{
    if (!root) {
        return nullptr;
    }

    const auto children = root->findChildren<QWidget*>();
    for (QWidget* child : children) {
        if (!child->isVisible()) {
            continue;
        }
        if (!child->isEnabled()) {
            continue;
        }
        const Qt::FocusPolicy policy = child->focusPolicy();
        if (policy == Qt::NoFocus) {
            continue;
        }
        return child;
    }
    return nullptr;
}

} // namespace

QtMaterialDialog::QtMaterialDialog(QWidget* parent)
    : QtMaterialOverlaySurface(parent)
    , m_scrim(new QtMaterialScrimWidget(parent ? parent : this))
    , m_transition(new QtMaterialTransitionController(this))
{
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_DeleteOnClose, false);
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

QtMaterialDialog::~QtMaterialDialog() = default;

QSize QtMaterialDialog::sizeHint() const
{
    ensureSpecResolved();
    ensureLayoutResolved();
    const int width = std::min(m_spec.maxWidth, 560);
    const int height = std::max(160, m_cachedContentRect.height() + 2 * m_spec.padding + 2 * m_cachedShadowMargin);
    return QSize(width, height);
}

QSize QtMaterialDialog::minimumSizeHint() const
{
    ensureSpecResolved();
    return QSize(280, 160);
}

void QtMaterialDialog::open()
{
    ensureSpecResolved();
    ensureLayoutResolved();
    setVisible(true);
    raise();
    if (m_scrim) {
        m_scrim->setVisible(true);
        m_scrim->raise();
    }
    m_transition->setDuration(220);
    m_transition->startForward();
    updateScrimVisuals();
    focusInitialChild();
}

void QtMaterialDialog::close()
{
    m_transition->setDuration(180);
    m_transition->startBackward();
    QWidget::close();
    if (m_scrim) {
        m_scrim->hide();
    }
}

void QtMaterialDialog::paintEvent(QPaintEvent*)
{
    ensureSpecResolved();
    ensureLayoutResolved();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QtMaterialShadowRenderer::paintRoundedShadow(
        &painter,
        m_cachedVisualRect,
        m_cachedCornerRadius,
        m_spec.scrimColor,
        18,
        6
    );

    painter.setPen(Qt::NoPen);
    painter.setBrush(m_spec.containerColor);
    painter.drawPath(m_cachedContainerPath);

    if (hasFocus()) {
        QtMaterialFocusIndicator::paintPathFocusRing(
            &painter,
            m_cachedContainerPath,
            m_spec.scrimColor,
            2.0
        );
    }
}

void QtMaterialDialog::resizeEvent(QResizeEvent* event)
{
    QtMaterialOverlaySurface::resizeEvent(event);
    invalidateLayoutCache();
}

void QtMaterialDialog::showEvent(QShowEvent* event)
{
    QtMaterialOverlaySurface::showEvent(event);
    updateScrimVisuals();
    focusInitialChild();
}

void QtMaterialDialog::hideEvent(QHideEvent* event)
{
    QtMaterialOverlaySurface::hideEvent(event);
    if (m_scrim) {
        m_scrim->hide();
    }
}

void QtMaterialDialog::changeEvent(QEvent* event)
{
    QtMaterialOverlaySurface::changeEvent(event);
    switch (event->type()) {
    case QEvent::EnabledChange:
    case QEvent::StyleChange:
    case QEvent::FontChange:
        invalidateLayoutCache();
        update();
        break;
    default:
        break;
    }
}

void QtMaterialDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        event->accept();
        close();
        return;
    }
    QtMaterialOverlaySurface::keyPressEvent(event);
}

void QtMaterialDialog::themeChangedEvent(const QtMaterial::Theme& theme)
{
    QtMaterialOverlaySurface::themeChangedEvent(theme);
    Q_UNUSED(theme)
    m_specDirty = true;
    invalidateLayoutCache();
    updateScrimVisuals();
}

void QtMaterialDialog::invalidateResolvedSpec()
{
    m_specDirty = true;
    invalidateLayoutCache();
}

void QtMaterialDialog::ensureSpecResolved() const
{
    if (!m_specDirty) {
        return;
    }

    QtMaterial::SpecFactory factory;
    m_spec = factory.dialogSpec(QtMaterial::ThemeManager::instance().theme());
    m_specDirty = false;
    m_layoutDirty = true;
}

void QtMaterialDialog::ensureLayoutResolved() const
{
    ensureSpecResolved();
    if (!m_layoutDirty) {
        return;
    }

    const QRect outer = rect();
    m_cachedShadowMargin = 24;

    const int maxWidth = std::min(m_spec.maxWidth, std::max(280, outer.width() - 2 * m_cachedShadowMargin));
    const int desiredHeight = std::max(160, outer.height() / 3);
    const QSize visualSize(maxWidth, std::min(desiredHeight, outer.height() - 2 * m_cachedShadowMargin));

    QRect visual(QPoint(0, 0), visualSize);
    visual.moveCenter(outer.center());
    m_cachedVisualRect = visual;
    m_cachedContentRect = visual.adjusted(m_spec.padding, m_spec.padding, -m_spec.padding, -m_spec.padding);
    m_cachedCornerRadius = 28.0;

    QPainterPath path;
    path.addRoundedRect(QRectF(m_cachedVisualRect), m_cachedCornerRadius, m_cachedCornerRadius);
    m_cachedContainerPath = path;
    m_layoutDirty = false;
}

void QtMaterialDialog::invalidateLayoutCache()
{
    m_layoutDirty = true;
}

void QtMaterialDialog::updateScrimVisuals()
{
    if (!m_scrim) {
        return;
    }

    QColor scrim = m_spec.scrimColor;
    scrim.setAlphaF(0.32 * m_transition->progress());
    m_scrim->setGeometry(parentWidget() ? parentWidget()->rect() : rect());
    m_scrim->setColor(scrim);
    if (isVisible()) {
        m_scrim->show();
        m_scrim->lower();
    }
}

void QtMaterialDialog::focusInitialChild()
{
    if (QWidget* child = firstFocusableChild()) {
        child->setFocus(Qt::OtherFocusReason);
        return;
    }
    setFocus(Qt::OtherFocusReason);
}

QWidget* QtMaterialDialog::firstFocusableChild() const
{
    return firstFocusableDescendant(const_cast<QtMaterialDialog*>(this));
}
