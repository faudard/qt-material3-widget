#include "qtmaterial/widgets/surfaces/qtmaterialcard.h"

#include <QEvent>
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>

#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/widgets/surfaces/private/qtmaterialsurfacerenderhelper_p.h"
#include "qtmaterial/core/private/qtmaterialaccessibilityhelper_p.h"

namespace QtMaterial {

QtMaterialCard::QtMaterialCard(QWidget* parent)
    : QtMaterialSurface(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
}

QtMaterialCard::~QtMaterialCard() = default;

QSize QtMaterialCard::sizeHint() const
{
    ensureSpecResolved();
    ensureLayoutResolved();

    const QFontMetrics fm(font());
    const int titleHeight = m_titleText.isEmpty() ? 0 : fm.height();
    const int bodyHeight = m_bodyText.isEmpty() ? 0 : fm.lineSpacing() * 2;

    const int width = qMax(m_spec.minWidth,
        m_spec.horizontalPadding * 2 + qMax(fm.horizontalAdvance(m_titleText), fm.horizontalAdvance(m_bodyText)));
    const int height = m_spec.verticalPadding * 2 + titleHeight + bodyHeight +
        ((m_titleText.isEmpty() || m_bodyText.isEmpty()) ? 0 : m_spec.titleBodySpacing);

    return QSize(width, qMax(height, m_spec.minHeight));
}

QSize QtMaterialCard::minimumSizeHint() const
{
    ensureSpecResolved();
    return QSize(m_spec.minWidth, m_spec.minHeight);
}

void QtMaterialCard::setTitleText(const QString& text)
{
    if (m_titleText == text) {
        return;
    }

    m_titleText = text;
    invalidateLayoutCache();
    updateGeometry();
    update();

    QtMaterialAccessibilityHelper::syncNameIfEmpty(this, text);
}

QString QtMaterialCard::titleText() const
{
    return m_titleText;
}

void QtMaterialCard::setBodyText(const QString& text)
{
    if (m_bodyText == text) {
        return;
    }

    m_bodyText = text;
    invalidateLayoutCache();
    updateGeometry();
    update();
}

QString QtMaterialCard::bodyText() const
{
    return m_bodyText;
}

void QtMaterialCard::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    ensureSpecResolved();
    ensureLayoutResolved();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QtMaterialSurfaceRenderHelper::paintSurface(
        &painter,
        m_cachedContainerPath,
        isEnabled() ? m_spec.containerColor : m_spec.disabledContainerColor,
        m_spec.shadowColor,
        m_spec.shadowBlurRadius,
        m_spec.shadowYOffset
    );

    painter.save();
    painter.setClipPath(m_cachedContainerPath);

    QRect textRect = m_cachedContentRect;
    const QFontMetrics fm(font());

    if (!m_titleText.isEmpty()) {
        painter.setPen(isEnabled() ? m_spec.titleColor : m_spec.disabledTitleColor);
        const QRect titleRect(textRect.left(), textRect.top(), textRect.width(), fm.height());
        painter.drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter,
                         fm.elidedText(m_titleText, Qt::ElideRight, titleRect.width()));
        textRect.setTop(titleRect.bottom() + 1 + m_spec.titleBodySpacing);
    }

    if (!m_bodyText.isEmpty()) {
        painter.setPen(isEnabled() ? m_spec.bodyColor : m_spec.disabledBodyColor);
        painter.drawText(textRect, Qt::TextWordWrap,
                         fm.elidedText(m_bodyText, Qt::ElideRight, textRect.width() * 3));
    }

    painter.restore();

    if (hasFocus()) {
        QtMaterialFocusIndicator::paintPathFocusRing(
            &painter,
            m_cachedContainerPath,
            m_spec.focusRingColor,
            m_spec.focusRingWidth
        );
    }
}

void QtMaterialCard::resizeEvent(QResizeEvent* event)
{
    QtMaterialSurface::resizeEvent(event);
    Q_UNUSED(event)
    invalidateLayoutCache();
}

void QtMaterialCard::changeEvent(QEvent* event)
{
    QtMaterialSurface::changeEvent(event);

    switch (event->type()) {
    case QEvent::FontChange:
    case QEvent::StyleChange:
    case QEvent::LayoutDirectionChange:
    case QEvent::EnabledChange:
        invalidateLayoutCache();
        updateGeometry();
        update();
        break;
    default:
        break;
    }
}

void QtMaterialCard::themeChangedEvent(const QtMaterial::Theme& theme)
{
    QtMaterialSurface::themeChangedEvent(theme);
    Q_UNUSED(theme)
    m_specDirty = true;
    invalidateLayoutCache();
    updateGeometry();
    update();
}

void QtMaterialCard::stateChangedEvent()
{
    QtMaterialSurface::stateChangedEvent();
    update();
}

void QtMaterialCard::ensureSpecResolved() const
{
    if (!m_specDirty) {
        return;
    }

    QtMaterial::SpecFactory factory;
    m_spec = factory.cardSpec(theme(), density());
    m_specDirty = false;
    m_layoutDirty = true;
}

void QtMaterialCard::ensureLayoutResolved() const
{
    if (!m_layoutDirty) {
        return;
    }

    m_cachedVisualRect = visualRect();
    m_cachedContentRect = contentRectForPaint();
    m_cachedCornerRadius = m_spec.cornerRadius;
    m_cachedContainerPath = containerPath();
    m_layoutDirty = false;
}

void QtMaterialCard::invalidateLayoutCache()
{
    m_layoutDirty = true;
}

QRect QtMaterialCard::visualRect() const
{
    ensureSpecResolved();
    const QRect r = rect();
    return r.adjusted(
        m_spec.shadowMarginLeft,
        m_spec.shadowMarginTop,
        -m_spec.shadowMarginRight,
        -m_spec.shadowMarginBottom
    );
}

QRect QtMaterialCard::contentRectForPaint() const
{
    const QRect vr = visualRect();
    return vr.adjusted(
        m_spec.horizontalPadding,
        m_spec.verticalPadding,
        -m_spec.horizontalPadding,
        -m_spec.verticalPadding
    );
}

QPainterPath QtMaterialCard::containerPath() const
{
    QPainterPath path;
    path.addRoundedRect(QRectF(m_cachedVisualRect), m_spec.cornerRadius, m_spec.cornerRadius);
    return path;
}

} // namespace QtMaterial
