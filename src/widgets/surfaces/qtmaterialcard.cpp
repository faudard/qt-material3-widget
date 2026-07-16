#include "qtmaterial/widgets/surfaces/qtmaterialcard.h"
#include <memory>

#include "qtmaterial/specs/qtmaterialsurfacespecresolver.h"
#include <QEvent>
#include <QFont>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QResizeEvent>

#include <QPainterPath>
#include "qtmaterial/theme/qtmaterialthemecontext.h"
namespace {

constexpr int kDefaultMinimumWidth = 120;
constexpr int kDefaultMinimumHeight = 72;
constexpr int kDefaultCornerRadius = 12;

QColor withAlpha(QColor color, int alpha)
{
    color.setAlpha(alpha);
    return color;
}

QPainterPath qtMaterialCardContainerPath(const QRect& visualRect, qreal cornerRadius)
{
    QPainterPath path;
    path.addRoundedRect(QRectF(visualRect), cornerRadius, cornerRadius);
    return path;
}
} // namespace

namespace QtMaterial {

class QtMaterialCardPrivate
{
public:
    QString titleText;
    QString bodyText;
    QtMaterialCard::Variant variant = QtMaterialCard::Variant::Elevated;
    bool interactive = false;
    bool pressed = false;
    bool hovered = false;
    QString lastAccessibilitySummary;
    bool specDirty = true;
    bool layoutDirty = true;
    QtMaterial::CardSpec spec;
    QRect cachedVisualRect;
    QRect cachedContentRect;
    qreal cachedCornerRadius = 12.0;
    QPainterPath cachedContainerPath;
};



// Tranche 35: QtMaterialCard private helpers moved out of the public header.
namespace {

void ensureSpecResolved(
    const QtMaterialCard* self,
    QtMaterialCardPrivate* d)
{
    if (!d->specDirty) {
        return;
    }

    QtMaterial::ThemeContext* context =
        self->effectiveThemeContext();
    Q_ASSERT(context);

    const QtMaterial::SurfaceSpecResolver resolver;
    d->spec = resolver.cardSpec(context->theme());
    d->specDirty = false;
    d->layoutDirty = true;
}

QRect visualRect(const QtMaterialCard* self)
{
    return self->rect().adjusted(1, 1, -1, -1);
}

QRect contentRectForPaint(const QtMaterialCard* self, QtMaterialCardPrivate* d)
{
    ensureSpecResolved(self, d);
    return visualRect(self).marginsRemoved(d->spec.contentPadding);
}

void ensureLayoutResolved(const QtMaterialCard* self, QtMaterialCardPrivate* d)
{
    if (!d->layoutDirty) {
        return;
    }

    d->cachedVisualRect = visualRect(self);
    d->cachedContentRect = contentRectForPaint(self, d);
    d->cachedCornerRadius = kDefaultCornerRadius;
    d->cachedContainerPath = qtMaterialCardContainerPath(d->cachedVisualRect, d->cachedCornerRadius);
    d->layoutDirty = false;
}

void invalidateLayoutCache(QtMaterialCardPrivate* d)
{
    d->layoutDirty = true;
}

QColor resolvedContainerColor(const QtMaterialCard* self, const QtMaterialCardPrivate* d)
{
    QColor color = d->spec.containerColor;
    if (d->variant == QtMaterialCard::Variant::Outlined) {
        color.setAlpha(0);
        return color;
    }

    if (!self->isEnabled()) {
        color.setAlpha(96);
    }
    return color;
}

QColor resolvedContentColor(const QtMaterialCard* self, const QtMaterialCardPrivate* d)
{
    QColor color = d->spec.contentColor;
    if (!self->isEnabled()) {
        color.setAlpha(128);
    }
    return color;
}

void syncAccessibility(QtMaterialCard* self, QtMaterialCardPrivate* d)
{
    const QString summary = self->accessibilitySummary();
    if (self->accessibleName().isEmpty() || self->accessibleName() == d->lastAccessibilitySummary) {
        self->setAccessibleName(d->titleText.isEmpty() ? QStringLiteral("Card") : d->titleText);
    }
    self->setAccessibleDescription(summary);
    if (d->lastAccessibilitySummary != summary) {
        d->lastAccessibilitySummary = summary;
        Q_EMIT self->accessibilitySummaryChanged(summary);
    }
}

} // namespace
QtMaterialCard::QtMaterialCard(QWidget* parent)
    : QtMaterialSurface(parent)
    , d(std::make_unique<QtMaterialCardPrivate>())
{
    setFocusPolicy(Qt::NoFocus);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    setAttribute(Qt::WA_Hover, true);
    syncAccessibility(this, d.get());
}

QtMaterialCard::~QtMaterialCard() = default;

QSize QtMaterialCard::sizeHint() const
{
    ensureSpecResolved(this, d.get());

    const QFontMetrics fm(font());
    const int textWidth = qMax(fm.horizontalAdvance(d->titleText), fm.horizontalAdvance(d->bodyText));
    const int titleHeight = d->titleText.isEmpty() ? 0 : fm.height();
    const QRect bodyBounds = d->bodyText.isEmpty()
        ? QRect()
        : fm.boundingRect(QRect(0, 0, 280, 1000),
                          Qt::TextWordWrap,
                          d->bodyText);

    const int spacing = (!d->titleText.isEmpty() && !d->bodyText.isEmpty()) ? 8 : 0;
    const int width = qMax(kDefaultMinimumWidth,
                           d->spec.contentPadding.left()
                               + qMax(textWidth, bodyBounds.width())
                               + d->spec.contentPadding.right());
    const int height = qMax(kDefaultMinimumHeight,
                            d->spec.contentPadding.top()
                                + titleHeight
                                + spacing
                                + bodyBounds.height()
                                + d->spec.contentPadding.bottom());

    return QSize(width, height);
}

QSize QtMaterialCard::minimumSizeHint() const
{
    return QSize(kDefaultMinimumWidth, kDefaultMinimumHeight);
}

void QtMaterialCard::setTitleText(const QString& text)
{
    if (d->titleText == text) {
        return;
    }

    d->titleText = text;
    invalidateLayoutCache(d.get());
    syncAccessibility(this, d.get());
    updateGeometry();
    update();
    Q_EMIT titleTextChanged(d->titleText);
}

QString QtMaterialCard::titleText() const
{
    return d->titleText;
}

void QtMaterialCard::setBodyText(const QString& text)
{
    if (d->bodyText == text) {
        return;
    }

    d->bodyText = text;
    invalidateLayoutCache(d.get());
    syncAccessibility(this, d.get());
    updateGeometry();
    update();
    Q_EMIT bodyTextChanged(d->bodyText);
}

QString QtMaterialCard::bodyText() const
{
    return d->bodyText;
}

QtMaterialCard::Variant QtMaterialCard::variant() const noexcept
{
    return d->variant;
}

void QtMaterialCard::setVariant(Variant variant)
{
    if (d->variant == variant) {
        return;
    }

    d->variant = variant;
    invalidateLayoutCache(d.get());
    update();
    Q_EMIT variantChanged(d->variant);
}

bool QtMaterialCard::isInteractive() const noexcept
{
    return d->interactive;
}

void QtMaterialCard::setInteractive(bool interactive)
{
    if (d->interactive == interactive) {
        return;
    }

    d->interactive = interactive;
    setFocusPolicy(d->interactive ? Qt::StrongFocus : Qt::NoFocus);
    if (!d->interactive && d->pressed) {
        d->pressed = false;
    }
    syncAccessibility(this, d.get());
    update();
    Q_EMIT interactiveChanged(d->interactive);
}

bool QtMaterialCard::isPressed() const noexcept
{
    return d->pressed;
}

QString QtMaterialCard::accessibilitySummary() const
{
    if (!d->titleText.isEmpty() && !d->bodyText.isEmpty()) {
        return QStringLiteral("%1. %2").arg(d->titleText, d->bodyText);
    }
    if (!d->titleText.isEmpty()) {
        return d->titleText;
    }
    if (!d->bodyText.isEmpty()) {
        return d->bodyText;
    }
    return QStringLiteral("Card");
}

void QtMaterialCard::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    ensureSpecResolved(this, d.get());
    ensureLayoutResolved(this, d.get());

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QColor container = resolvedContainerColor(this, d.get());
    painter.fillPath(d->cachedContainerPath, container);

    if (d->variant == Variant::Outlined) {
        QPen outlinePen(d->spec.outlineColor);
        outlinePen.setWidth(1);
        painter.setPen(outlinePen);
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(d->cachedContainerPath);
    }

    if (d->interactive && (d->hovered || d->pressed)) {
        const int alpha = d->pressed ? 30 : 18;
        painter.fillPath(d->cachedContainerPath, withAlpha(resolvedContentColor(this, d.get()), alpha));
    }

    QRect textRect = d->cachedContentRect;
    const QFont baseFont = font();
    QFont titleFont = baseFont;
    titleFont.setBold(true);

    if (!d->titleText.isEmpty()) {
        painter.setFont(titleFont);
        painter.setPen(resolvedContentColor(this, d.get()));
        const QFontMetrics titleMetrics(titleFont);
        const QRect titleRect(textRect.left(), textRect.top(), textRect.width(), titleMetrics.height());
        painter.drawText(titleRect,
                         Qt::AlignLeft | Qt::AlignVCenter,
                         titleMetrics.elidedText(d->titleText, Qt::ElideRight, titleRect.width()));
        textRect.setTop(titleRect.bottom() + 1 + 8);
    }

    if (!d->bodyText.isEmpty()) {
        painter.setFont(baseFont);
        painter.setPen(resolvedContentColor(this, d.get()));
        painter.drawText(textRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, d->bodyText);
    }

    if (d->interactive && hasFocus()) {
        QPen focusPen(d->spec.outlineColor);
        focusPen.setWidth(2);
        focusPen.setJoinStyle(Qt::RoundJoin);
        painter.setPen(focusPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(d->cachedContainerPath);
    }
}

void QtMaterialCard::resizeEvent(QResizeEvent* event)
{
    QtMaterialSurface::resizeEvent(event);
    invalidateLayoutCache(d.get());
}

void QtMaterialCard::changeEvent(QEvent* event)
{
    QtMaterialSurface::changeEvent(event);

    switch (event->type()) {
    case QEvent::FontChange:
    case QEvent::StyleChange:
    case QEvent::LayoutDirectionChange:
    case QEvent::EnabledChange:
        invalidateLayoutCache(d.get());
        syncAccessibility(this, d.get());
        updateGeometry();
        update();
        break;
    default:
        break;
    }
}

void QtMaterialCard::keyPressEvent(QKeyEvent* event)
{
    if (!d->interactive || !isEnabled()) {
        QtMaterialSurface::keyPressEvent(event);
        return;
    }

    if ((event->key() == Qt::Key_Space || event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
        && !event->isAutoRepeat()) {
        if (!d->pressed) {
            d->pressed = true;
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
    if (!d->interactive || !isEnabled()) {
        QtMaterialSurface::keyReleaseEvent(event);
        return;
    }

    if ((event->key() == Qt::Key_Space || event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
        && !event->isAutoRepeat()) {
        const bool wasPressed = d->pressed;
        d->pressed = false;
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
    if (d->interactive && isEnabled() && event->button() == Qt::LeftButton && rect().contains(event->pos())) {
        d->pressed = true;
        update();
        Q_EMIT pressed();
        event->accept();
        return;
    }

    QtMaterialSurface::mousePressEvent(event);
}

void QtMaterialCard::mouseReleaseEvent(QMouseEvent* event)
{
    if (d->interactive && isEnabled() && event->button() == Qt::LeftButton) {
        const bool wasPressed = d->pressed;
        d->pressed = false;
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
        if (!d->hovered) {
            d->hovered = true;
            update();
        }
        break;
    case QEvent::Leave:
    case QEvent::HoverLeave:
        if (d->hovered) {
            d->hovered = false;
            update();
        }
        if (d->pressed) {
            d->pressed = false;
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
    d->specDirty = true;
    invalidateLayoutCache(d.get());
    updateGeometry();
    update();
}

void QtMaterialCard::stateChangedEvent()
{
    QtMaterialSurface::stateChangedEvent();
    update();
}

} // namespace QtMaterial
