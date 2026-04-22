#include "qtmaterial/widgets/surfaces/qtmaterialbottomsheet.h"
#include <QFocusEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QShowEvent>
#include <QWidget>

#include "qtmaterial/effects/qtmaterialscrimwidget.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/specs/qtmaterialbottomsheetspec.h"
#include "private/qtmaterialsurfacerenderhelper_p.h"


namespace QtMaterial
{
QtMaterialBottomSheet::QtMaterialBottomSheet(QWidget *parent)
    : QtMaterialOverlaySurface(parent)
{
    setFocusPolicy(Qt::StrongFocus);

    m_transition = new QtMaterialTransitionController(this);
    m_scrim = new QtMaterialScrimWidget(parent ? parent : this);
    if (m_scrim) {
        m_scrim->hide();
    }

    connect(m_transition, &QtMaterialTransitionController::progressChanged,
            this, [this](qreal) {
                invalidateCachedGeometry();
                syncScrim();
                update();
            });
    connect(m_transition, &QtMaterialTransitionController::finished,
            this, [this]() {
                if (m_state == SheetState::Closing) {
                    m_state = SheetState::Closed;
                    hide();
                    if (m_scrim) {
                        m_scrim->hide();
                    }
                } else if (m_state == SheetState::Opening) {
                    m_state = SheetState::Open;
                    focusFirstChild();
                }
            });
}

QtMaterialBottomSheet::~QtMaterialBottomSheet() = default;

void QtMaterialBottomSheet::open()
{
    if (isVisible() && (m_state == SheetState::Opening || m_state == SheetState::Open)) {
        return;
    }

    syncToHost();
    show();
    raise();
    if (m_scrim && m_modal) {
        m_scrim->show();
        m_scrim->raise();
    }
    raise();

    m_state = SheetState::Opening;
    m_transition->startForward();
    syncScrim();
}

void QtMaterialBottomSheet::close()
{
    if (m_state == SheetState::Closing || m_state == SheetState::Closed) {
        return;
    }

    m_state = SheetState::Closing;
    m_transition->startBackward();
    syncScrim();
}

bool QtMaterialBottomSheet::isOpen() const noexcept
{
    return m_state == SheetState::Open || m_state == SheetState::Opening;
}

void QtMaterialBottomSheet::setModal(bool modal)
{
    if (m_modal == modal) {
        return;
    }
    m_modal = modal;
    syncScrim();
}

bool QtMaterialBottomSheet::isModal() const noexcept
{
    return m_modal;
}

void QtMaterialBottomSheet::setExpandedHeight(int px)
{
    const int clamped = qMax(120, px);
    if (m_expandedHeight == clamped) {
        return;
    }
    m_expandedHeight = clamped;
    invalidateCachedGeometry();
    updateGeometry();
    update();
}

int QtMaterialBottomSheet::expandedHeight() const noexcept
{
    return m_expandedHeight;
}

QSize QtMaterialBottomSheet::sizeHint() const
{
    ensureSpecResolved();
    return QSize(640, qMax(m_expandedHeight, 240));
}

QSize QtMaterialBottomSheet::minimumSizeHint() const
{
    ensureSpecResolved();
    return QSize(280, 120);
}

void QtMaterialBottomSheet::paintEvent(QPaintEvent *)
{
    ensureSpecResolved();
    ensureGeometryResolved();

    if (!m_specPtr || m_cachedVisualRect.isEmpty()) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(m_specPtr->containerColor);
    painter.drawPath(m_cachedContainerPath);
}

void QtMaterialBottomSheet::resizeEvent(QResizeEvent *event)
{
    QtMaterialOverlaySurface::resizeEvent(event);
    invalidateCachedGeometry();
    syncToHost();
}

void QtMaterialBottomSheet::showEvent(QShowEvent *event)
{
    QtMaterialOverlaySurface::showEvent(event);
    syncToHost();
    syncScrim();
}

void QtMaterialBottomSheet::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Escape:
    case Qt::Key_Back:
        close();
        event->accept();
        return;
    case Qt::Key_Down:
        if (event->modifiers() & Qt::ControlModifier) {
            close();
            event->accept();
            return;
        }
        break;
    default:
        break;
    }
    QtMaterialOverlaySurface::keyPressEvent(event);
}

void QtMaterialBottomSheet::themeChangedEvent(const QtMaterial::Theme &theme)
{
    QtMaterialOverlaySurface::themeChangedEvent(theme);
    Q_UNUSED(theme)
    m_specDirty = true;
    invalidateCachedGeometry();
    syncScrim();
    update();
}

void QtMaterialBottomSheet::ensureSpecResolved() const
{
    if (!m_specDirty && m_specPtr) {
        return;
    }

    static QtMaterial::SpecFactory factory;
    static QtMaterial::BottomSheetSpec spec;
    spec = factory.bottomSheetSpec(theme());

    m_specPtr = &spec;
    m_specDirty = false;
}

void QtMaterialBottomSheet::ensureGeometryResolved() const
{
    if (!m_geometryDirty) {
        return;
    }

    ensureSpecResolved();
    if (!m_specPtr) {
        return;
    }

    const QRect hostRect = rect();
    if (!hostRect.isValid()) {
        return;
    }

    const int minHeight = qMax(120, minimumSizeHint().height());
    const int sheetHeight = qMin(qMax(m_expandedHeight, minHeight), hostRect.height());

    const qreal progress = m_transition ? m_transition->progress() : 1.0;
    const int visibleY = hostRect.height() - sheetHeight;
    const int hiddenY = hostRect.height();
    const int y = hiddenY - qRound(sheetHeight * qBound<qreal>(0.0, progress, 1.0));

    m_cachedVisualRect = QRect(0, y, hostRect.width(), sheetHeight);
    m_cachedContentRect = m_cachedVisualRect.adjusted(0, m_specPtr->topPadding, 0, 0);

    m_cachedCornerRadius = 28.0; // TODO: dériver depuis shapeRole
    m_cachedContainerPath = QPainterPath();
    m_cachedContainerPath.addRoundedRect(m_cachedVisualRect,
                                         m_cachedCornerRadius,
                                         m_cachedCornerRadius);

    m_geometryDirty = false;
}

void QtMaterialBottomSheet::invalidateCachedGeometry()
{
    m_geometryDirty = true;
    m_cachedVisualRect = QRect();
    m_cachedContentRect = QRect();
    m_cachedContainerPath = QPainterPath();
    m_cachedCornerRadius = 0.0;
}

void QtMaterialBottomSheet::syncToHost()
{
    if (QWidget *host = parentWidget()) {
        setGeometry(host->rect());
        if (m_scrim) {
            m_scrim->setGeometry(host->rect());
        }
    }
    invalidateCachedGeometry();
}

void QtMaterialBottomSheet::syncScrim()
{
    if (!m_scrim) {
        return;
    }

    if (!m_modal || m_state == SheetState::Closed) {
        m_scrim->hide();
        return;
    }

    ensureSpecResolved();
    if (!m_specPtr) {
        return;
    }

    QColor scrim = m_specPtr->scrimColor;
    const qreal progress = m_transition ? m_transition->progress() : 1.0;
    scrim.setAlphaF(qBound<qreal>(0.0, scrim.alphaF() * progress, 1.0));

    m_scrim->setGeometry(rect());
    m_scrim->setScrimColor(scrim);

    if (!m_scrim->isVisible()) {
        m_scrim->show();
    }

    m_scrim->raise();
}

void QtMaterialBottomSheet::focusFirstChild()
{
    const auto children = findChildren<QWidget*>();
    for (QWidget *child : children) {
        if (!child || !child->isVisible() || !child->isEnabled()) {
            continue;
        }
        if (child->focusPolicy() != Qt::NoFocus) {
            child->setFocus(Qt::OtherFocusReason);
            return;
        }
    }
    setFocus(Qt::OtherFocusReason);
}

QRect QtMaterialBottomSheet::sheetVisualRect() const
{
    ensureGeometryResolved();
    return m_cachedVisualRect;
}

QRect QtMaterialBottomSheet::contentRect() const
{
    ensureGeometryResolved();
    return m_cachedContentRect;
}

qreal QtMaterialBottomSheet::cornerRadius() const
{
    ensureGeometryResolved();
    return m_cachedCornerRadius;
}

}