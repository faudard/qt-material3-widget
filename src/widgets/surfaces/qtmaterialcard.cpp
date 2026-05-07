#include "qtmaterial/widgets/surfaces/qtmaterialcard.h"

#include "qtmaterial/specs/qtmaterialspecfactory.h"

#include <QEvent>
#include <QFont>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QResizeEvent>

namespace {

constexpr int kDefaultMinimumWidth = 120;
constexpr int kDefaultMinimumHeight = 72;
constexpr int kDefaultCornerRadius = 12;

QColor withAlpha(QColor color, int alpha)
{
    color.setAlpha(alpha);
    return color;
}

} // namespace

namespace QtMaterial {

QtMaterialCard::QtMaterialCard(QWidget* parent)
    : QtMaterialSurface(parent)
{
    setFocusPolicy(Qt::NoFocus);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    setAttribute(Qt::WA_Hover, true);
    syncAccessibility();
}

QtMaterialCard::~QtMaterialCard() = default;

QSize QtMaterialCard::sizeHint() const
{
    ensureSpecResolved();

    const QFontMetrics fm(font());
    const int textWidth = qMax(fm.horizontalAdvance(m_titleText), fm.horizontalAdvance(m_bodyText));
    const int titleHeight = m_titleText.isEmpty() ? 0 : fm.height();
    const QRect bodyBounds = m_bodyText.isEmpty()
        ? QRect()
        : fm.boundingRect(QRect(0, 0, 280, 1000),
                          Qt::TextWordWrap,
                          m_bodyText);

    const int spacing = (!m_titleText.isEmpty() && !m_bodyText.isEmpty()) ? 8 : 0;
    const int width = qMax(kDefaultMinimumWidth,
                           m_spec.contentPadding.left()
                               + qMax(textWidth, bodyBounds.width())
                               + m_spec.contentPadding.right());
    const int height = qMax(kDefaultMinimumHeight,
                            m_spec.contentPadding.top()
                                + titleHeight
                                + spacing
                                + bodyBounds.height()
                                + m_spec.contentPadding.bottom());

    return QSize(width, height);
}

QSize QtMaterialCard::minimumSizeHint() const
{
    return QSize(kDefaultMinimumWidth, kDefaultMinimumHeight);
}

void QtMaterialCard::setTitleText(const QString& text)
{
    if (m_titleText == text) {
        return;
    }

    m_titleText = text;
    invalidateLayoutCache();
    syncAccessibility();
    updateGeometry();
    update();
    Q_EMIT titleTextChanged(m_titleText);
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
    syncAccessibility();
    updateGeometry();
    update();
    Q_EMIT bodyTextChanged(m_bodyText);
}

QString QtMaterialCard::bodyText() const
{
    return m_bodyText;
}

QtMaterialCard::Variant QtMaterialCard::variant() const noexcept
{
    return m_variant;
}

void QtMaterialCard::setVariant(Variant variant)
{
    if (m_variant == variant) {
        return;
    }

    m_variant = variant;
    invalidateLayoutCache();
    update();
    Q_EMIT variantChanged(m_variant);
}

bool QtMaterialCard::isInteractive() const noexcept
{
    return m_interactive;
}

void QtMaterialCard::setInteractive(bool interactive)
{
    if (m_interactive == interactive) {
        return;
    }

    m_interactive = interactive;
    setFocusPolicy(m_interactive ? Qt::StrongFocus : Qt::NoFocus);
    if (!m_interactive && m_pressed) {
        m_pressed = false;
    }
    syncAccessibility();
    update();
    Q_EMIT interactiveChanged(m_interactive);
}

bool QtMaterialCard::isPressed() const noexcept
{
    return m_pressed;
}

QString QtMaterialCard::accessibilitySummary() const
{
    if (!m_titleText.isEmpty() && !m_bodyText.isEmpty()) {
        return QStringLiteral("%1. %2").arg(m_titleText, m_bodyText);
    }
    if (!m_titleText.isEmpty()) {
        return m_titleText;
    }
    if (!m_bodyText.isEmpty()) {
        return m_bodyText;
    }
    return QStringLiteral("Card");
}

void QtMaterialCard::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    ensureSpecResolved();
    ensureLayoutResolved();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QColor container = resolvedContainerColor();
    painter.fillPath(m_cachedContainerPath, container);

    if (m_variant == Variant::Outlined) {
        QPen outlinePen(m_spec.outlineColor);
        outlinePen.setWidth(1);
        painter.setPen(outlinePen);
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(m_cachedContainerPath);
    }

    if (m_interactive && (m_hovered || m_pressed)) {
        const int alpha = m_pressed ? 30 : 18;
        painter.fillPath(m_cachedContainerPath, withAlpha(resolvedContentColor(), alpha));
    }

    QRect textRect = m_cachedContentRect;
    const QFont baseFont = font();
    QFont titleFont = baseFont;
    titleFont.setBold(true);

    if (!m_titleText.isEmpty()) {
        painter.setFont(titleFont);
        painter.setPen(resolvedContentColor());
        const QFontMetrics titleMetrics(titleFont);
        const QRect titleRect(textRect.left(), textRect.top(), textRect.width(), titleMetrics.height());
        painter.drawText(titleRect,
                         Qt::AlignLeft | Qt::AlignVCenter,
                         titleMetrics.elidedText(m_titleText, Qt::ElideRight, titleRect.width()));
        textRect.setTop(titleRect.bottom() + 1 + 8);
    }

    if (!m_bodyText.isEmpty()) {
        painter.setFont(baseFont);
        painter.setPen(resolvedContentColor());
        painter.drawText(textRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, m_bodyText);
    }

    if (m_interactive && hasFocus()) {
        QPen focusPen(m_spec.outlineColor);
        focusPen.setWidth(2);
        focusPen.setJoinStyle(Qt::RoundJoin);
        painter.setPen(focusPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(m_cachedContainerPath);
    }
}

void QtMaterialCard::resizeEvent(QResizeEvent* event)
{
    QtMaterialSurface::resizeEvent(event);
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
        syncAccessibility();
        updateGeometry();
        update();
        break;
    default:
        break;
    }
}

void QtMaterialCard::keyPressEvent(QKeyEvent* event)
{
    if (!m_interactive || !isEnabled()) {
        QtMaterialSurface::keyPressEvent(event);
        return;
    }

    if ((event->key() == Qt::Key_Space || event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
        && !event->isAutoRepeat()) {
        if (!m_pressed) {
            m_pressed = true;
            update();
            Q_EMIT pressed();
        }
        event->accept();
        return;
    }

    QtMaterialSurface::keyPressEvent(event);
}

void QtMaterialCard::keyReleaseEvent(QKeyEvent* event)
{
    if (!m_interactive || !isEnabled()) {
        QtMaterialSurface::keyReleaseEvent(event);
        return;
    }

    if ((event->key() == Qt::Key_Space || event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
        && !event->isAutoRepeat()) {
        const bool wasPressed = m_pressed;
        m_pressed = false;
        update();
        if (wasPressed) {
            Q_EMIT released();
            Q_EMIT clicked();
        }
        event->accept();
        return;
    }

    QtMaterialSurface::keyReleaseEvent(event);
}

void QtMaterialCard::mousePressEvent(QMouseEvent* event)
{
    if (m_interactive && isEnabled() && event->button() == Qt::LeftButton && rect().contains(event->pos())) {
        m_pressed = true;
        update();
        Q_EMIT pressed();
        event->accept();
        return;
    }

    QtMaterialSurface::mousePressEvent(event);
}

void QtMaterialCard::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_interactive && isEnabled() && event->button() == Qt::LeftButton) {
        const bool wasPressed = m_pressed;
        m_pressed = false;
        update();
        Q_EMIT released();
        if (wasPressed && rect().contains(event->pos())) {
            Q_EMIT clicked();
        }
        event->accept();
        return;
    }

    QtMaterialSurface::mouseReleaseEvent(event);
}

bool QtMaterialCard::event(QEvent* event)
{
    switch (event->type()) {
    case QEvent::Enter:
    case QEvent::HoverEnter:
        if (!m_hovered) {
            m_hovered = true;
            update();
        }
        break;
    case QEvent::Leave:
    case QEvent::HoverLeave:
        if (m_hovered) {
            m_hovered = false;
            update();
        }
        if (m_pressed) {
            m_pressed = false;
            update();
            Q_EMIT released();
        }
        break;
    default:
        break;
    }

    return QtMaterialSurface::event(event);
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
    m_spec = factory.cardSpec(theme());
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
    m_cachedCornerRadius = kDefaultCornerRadius;
    m_cachedContainerPath = containerPath();
    m_layoutDirty = false;
}

void QtMaterialCard::invalidateLayoutCache()
{
    m_layoutDirty = true;
}

void QtMaterialCard::syncAccessibility()
{
    const QString summary = accessibilitySummary();

    if (accessibleName().isEmpty() || accessibleName() == m_lastAccessibilitySummary) {
        setAccessibleName(m_titleText.isEmpty() ? QStringLiteral("Card") : m_titleText);
    }

    setAccessibleDescription(summary);

    if (m_lastAccessibilitySummary != summary) {
        m_lastAccessibilitySummary = summary;
        Q_EMIT accessibilitySummaryChanged(summary);
    }
}

QRect QtMaterialCard::visualRect() const
{
    return rect().adjusted(1, 1, -1, -1);
}

QRect QtMaterialCard::contentRectForPaint() const
{
    ensureSpecResolved();
    return visualRect().marginsRemoved(m_spec.contentPadding);
}

QPainterPath QtMaterialCard::containerPath() const
{
    QPainterPath path;
    path.addRoundedRect(QRectF(m_cachedVisualRect), m_cachedCornerRadius, m_cachedCornerRadius);
    return path;
}

QColor QtMaterialCard::resolvedContainerColor() const
{
    QColor color = m_spec.containerColor;

    if (m_variant == Variant::Outlined) {
        color.setAlpha(0);
        return color;
    }

    if (!isEnabled()) {
        color.setAlpha(96);
    }

    return color;
}

QColor QtMaterialCard::resolvedContentColor() const
{
    QColor color = m_spec.contentColor;
    if (!isEnabled()) {
        color.setAlpha(128);
    }
    return color;
}

} // namespace QtMaterial
