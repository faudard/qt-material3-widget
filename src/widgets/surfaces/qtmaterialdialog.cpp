//#include "qtmaterial/widgets/surfaces/private/qtmaterialdialog_p.h"
#include "qtmaterial/widgets/surfaces/qtmaterialdialog.h"
#include <algorithm>

#include <QEvent>
#include <QHideEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QShowEvent>
#include <QWidget>

#include "qtmaterial/effects/qtmaterialscrimwidget.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {
namespace {

static qreal boundedProgress(qreal value)
{
    return qBound<qreal>(0.0, value, 1.0);
}

} // namespace

QtMaterialDialog::QtMaterialDialog(QWidget* parent)
    : QtMaterialOverlaySurface(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_StyledBackground, false);
    setAttribute(Qt::WA_OpaquePaintEvent, false);

    m_transition = new QtMaterialTransitionController(this);
    m_transition->setDuration(180);

    connect(m_transition, &QtMaterialTransitionController::progressChanged,
            this, [this](qreal value) {
                updateScrimForProgress(value);
                update();
            });

    connect(m_transition, &QtMaterialTransitionController::finished,
            this, [this]() {
                if (m_closing) {
                    QWidget::hide();
                    if (m_scrim) {
                        m_scrim->hide();
                    }
                    m_closing = false;
                }
            });

    syncAccessibilityState();
}

QtMaterialDialog::~QtMaterialDialog() = default;

void QtMaterialDialog::setBodyWidget(QWidget* widget)
{
    if (m_bodyWidget == widget) {
        return;
    }

    if (m_bodyWidget) {
        m_bodyWidget->hide();
        m_bodyWidget->setParent(nullptr);
    }

    m_bodyWidget = widget;

    if (m_bodyWidget) {
        m_bodyWidget->setParent(this);
        m_bodyWidget->show();
    }

    invalidateLayoutCache();
    syncChildGeometry();
    syncAccessibilityState();
    updateGeometry();
    update();
}

QWidget* QtMaterialDialog::bodyWidget() const
{
    return m_bodyWidget.data();
}

void QtMaterialDialog::open()
{
    resolveSpecIfNeeded();
    ensureLayoutResolved();

    m_closing = false;
    QWidget::show();
    raise();

    updateScrimForProgress(0.0);

    if (m_scrim) {
        m_scrim->show();
        m_scrim->raise();
    }

    raise();
    m_transition->startForward();
    focusInitialChild();
}

void QtMaterialDialog::close()
{
    if (!isVisible()) {
        return;
    }

    m_closing = true;
    m_transition->startBackward();
}

QSize QtMaterialDialog::sizeHint() const
{
    resolveSpecIfNeeded();

    const QSize bodySize = m_bodyWidget ? m_bodyWidget->sizeHint() : QSize(320, 160);
    const int width = std::min<int>(
        m_spec.maxWidth,
        bodySize.width() + (2 * (m_spec.padding + m_cachedShadowMargin)));
    const int height =
        bodySize.height() + (2 * (m_spec.padding + m_cachedShadowMargin));

    return QSize(width, height);
}

QSize QtMaterialDialog::minimumSizeHint() const
{
    resolveSpecIfNeeded();

    const QSize bodyMin = m_bodyWidget ? m_bodyWidget->minimumSizeHint() : QSize(240, 120);
    return QSize(bodyMin.width() + (2 * m_spec.padding),
                 bodyMin.height() + (2 * m_spec.padding));
}

void QtMaterialDialog::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    resolveSpecIfNeeded();
    ensureLayoutResolved();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Simple shadow-like underlay. Kept intentionally cheap and self-contained.
    QRect shadowRect = m_cachedVisualRect.adjusted(0, 4, 0, 4);
    QColor shadowColor(0, 0, 0, 38);
    painter.setPen(Qt::NoPen);
    painter.setBrush(shadowColor);
    painter.drawRoundedRect(shadowRect, m_cachedCornerRadius, m_cachedCornerRadius);

    painter.setPen(Qt::NoPen);
    painter.setBrush(m_spec.containerColor);
    painter.drawPath(m_cachedContainerPath);

    if (hasFocus()) {
        QPen pen(theme().colorScheme().color(ColorRole::Primary), 2.0);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(m_cachedContainerPath);
    }
}

void QtMaterialDialog::keyPressEvent(QKeyEvent* event)
{
    if (!event) {
        return;
    }

    if (event->key() == Qt::Key_Escape) {
        close();
        event->accept();
        return;
    }

    QtMaterialOverlaySurface::keyPressEvent(event);
}

void QtMaterialDialog::resizeEvent(QResizeEvent* event)
{
    QtMaterialOverlaySurface::resizeEvent(event);
    invalidateLayoutCache();
    syncChildGeometry();
}

void QtMaterialDialog::showEvent(QShowEvent* event)
{
    QtMaterialOverlaySurface::showEvent(event);
    updateScrimForProgress(m_transition ? m_transition->progress() : 1.0);
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

    if (!event) {
        return;
    }

    switch (event->type()) {
    case QEvent::EnabledChange:
    case QEvent::FontChange:
    case QEvent::StyleChange:
    case QEvent::PaletteChange:
        invalidateLayoutCache();
        update();
        break;
    default:
        break;
    }
}

void QtMaterialDialog::themeChangedEvent(const QtMaterial::Theme& theme)
{
    QtMaterialOverlaySurface::themeChangedEvent(theme);
    Q_UNUSED(theme);

    invalidateResolvedSpec();
    updateScrimForProgress(m_transition ? m_transition->progress() : 1.0);
    update();
}

void QtMaterialDialog::invalidateResolvedSpec()
{
    m_specDirty = true;
    invalidateLayoutCache();
}

void QtMaterialDialog::resolveSpecIfNeeded() const
{
    if (!m_specDirty) {
        return;
    }

    SpecFactory factory;
    m_spec = factory.dialogSpec(theme());
    m_specDirty = false;
    m_layoutDirty = true;
}

void QtMaterialDialog::ensureLayoutResolved() const
{
    if (!m_layoutDirty) {
        return;
    }

    const QRect bounds = rect();
    const int availableWidth = std::max<int>(0, bounds.width() - (2 * m_cachedShadowMargin));
    const int targetWidth = std::min<int>(m_spec.maxWidth, availableWidth);

    const QSize desiredBody = m_bodyWidget ? m_bodyWidget->sizeHint() : QSize(320, 160);
    const int minHeight = std::max<int>(120, desiredBody.height() + (2 * m_spec.padding));
    const int availableHeight = std::max<int>(0, bounds.height() - (2 * m_cachedShadowMargin));
    const int targetHeight = std::min<int>(availableHeight, minHeight);

    const int x = bounds.x() + ((bounds.width() - targetWidth) / 2);
    const int y = bounds.y() + ((bounds.height() - targetHeight) / 2);

    m_cachedVisualRect = QRect(x, y, targetWidth, targetHeight);
    m_cachedContentRect = m_cachedVisualRect.adjusted(
        m_spec.padding,
        m_spec.padding,
        -m_spec.padding,
        -m_spec.padding);

    m_cachedCornerRadius = cornerRadiusForShape(m_spec.shapeRole);

    m_cachedContainerPath = QPainterPath();
    m_cachedContainerPath.addRoundedRect(
        QRectF(m_cachedVisualRect),
        m_cachedCornerRadius,
        m_cachedCornerRadius);

    m_layoutDirty = false;
    syncChildGeometry();
}

void QtMaterialDialog::invalidateLayoutCache()
{
    m_layoutDirty = true;
}

void QtMaterialDialog::syncChildGeometry() const
{
    if (m_layoutDirty) {
        return;
    }

    if (m_bodyWidget) {
        m_bodyWidget->setGeometry(m_cachedContentRect);
    }
}

void QtMaterialDialog::syncAccessibilityState()
{
    if (accessibleName().isEmpty()) {
        if (!windowTitle().isEmpty()) {
            setAccessibleName(windowTitle());
        } else {
            setAccessibleName(QStringLiteral("Dialog"));
        }
    }
}

void QtMaterialDialog::updateScrimForProgress(qreal progress)
{
    QWidget* host = parentWidget();
    if (!host) {
        host = hostWidget();
    }

    if (!host) {
        return;
    }

    if (!m_scrim || m_scrim->parentWidget() != host) {
        if (m_scrim) {
            m_scrim->deleteLater();
        }

        m_scrim = new QtMaterialScrimWidget(host);
        m_scrim->hide();
    }

    m_scrim->setGeometry(host->rect());

    QColor scrimColor = m_spec.scrimColor;
    scrimColor.setAlphaF(scrimColor.alphaF() * boundedProgress(progress));
    m_scrim->setScrimColor(scrimColor);

    if (isVisible() || progress > 0.001) {
        m_scrim->show();
        m_scrim->raise();
    } else {
        m_scrim->hide();
    }
}

void QtMaterialDialog::focusInitialChild()
{
    QWidget* child = firstFocusableChild();
    if (child) {
        child->setFocus(Qt::OtherFocusReason);
    } else {
        setFocus(Qt::OtherFocusReason);
    }
}

QWidget* QtMaterialDialog::firstFocusableChild() const
{
    const auto children = findChildren<QWidget*>(QString(), Qt::FindChildrenRecursively);
    for (QWidget* child : children) {
        if (!child || child == this) {
            continue;
        }
        if (!child->isVisibleTo(const_cast<QtMaterialDialog*>(this))) {
            continue;
        }
        if (!child->isEnabled()) {
            continue;
        }
        if (child->focusPolicy() == Qt::NoFocus) {
            continue;
        }
        return child;
    }
    return nullptr;
}

qreal QtMaterialDialog::cornerRadiusForShape(ShapeRole role)
{
    switch (role) {
    case ShapeRole::ExtraSmall:
        return 4.0;
    case ShapeRole::Small:
        return 8.0;
    case ShapeRole::Medium:
        return 12.0;
    case ShapeRole::Large:
        return 16.0;
    case ShapeRole::ExtraLarge:
        return 28.0;
    case ShapeRole::Full:
        return 999.0;
    case ShapeRole::None:
    default:
        return 0.0;
    }
}

} // namespace QtMaterial