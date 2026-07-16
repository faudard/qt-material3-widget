#include "qtmaterial/widgets/selection/qtmaterialradiobutton.h"
#include <memory>
#include <QPainterPath>
#include <QRectF>

// #include <QChangeEvent>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QResizeEvent>

#include "private/qtmaterialselectionrenderhelper_p.h"
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include "qtmaterial/specs/qtmaterialselectionspecresolver.h"
#include "qtmaterial/specs/qtmaterialradiobuttonspec.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/core/qtmaterialeventcompat.h"

namespace QtMaterial {

namespace {
QRectF centeredRect(const QRectF& outer, qreal size)
{
    return QRectF(outer.center().x() - size / 2.0,
                  outer.center().y() - size / 2.0,
                  size,
                  size);
}

qreal lerp(qreal a, qreal b, qreal t)
{
    return a + (b - a) * t;
}

QColor blendColor(const QColor& a, const QColor& b, qreal t)
{
    QColor out;
    out.setRedF(lerp(a.redF(), b.redF(), t));
    out.setGreenF(lerp(a.greenF(), b.greenF(), t));
    out.setBlueF(lerp(a.blueF(), b.blueF(), t));
    out.setAlphaF(lerp(a.alphaF(), b.alphaF(), t));
    return out;
}
}

struct QtMaterialRadioButtonPrivate
{
    explicit QtMaterialRadioButtonPrivate(QtMaterialRadioButton* owner)
        : ripple(new QtMaterialRippleController(owner))
        , transition(new QtMaterialTransitionController(owner))
    {
    }

    mutable bool specDirty = true;
    mutable bool layoutDirty = true;
    mutable RadioButtonSpec spec;
    QtMaterialRippleController* ripple = nullptr;
    QtMaterialTransitionController* transition = nullptr;
    mutable QRectF cachedIndicatorRect;
    mutable QRectF cachedDotRect;
    mutable QRectF cachedStateLayerRect;
    mutable QRectF cachedFocusRingRect;
    mutable QRect cachedLabelRect;
    mutable QPainterPath cachedIndicatorPath;
    mutable QPainterPath cachedRippleClipPath;
    mutable QString cachedElidedText;
};

QtMaterialRadioButton::QtMaterialRadioButton(QWidget* parent)
    : QtMaterialSelectionControl(parent)
    , d(std::make_unique<QtMaterialRadioButtonPrivate>(this))
{
    setCheckable(true);
    setAutoExclusive(false);
    setFocusPolicy(Qt::StrongFocus);

    d->transition->setProgress(isChecked() ? 1.0 : 0.0);

    QObject::connect(d->transition, &QtMaterialTransitionController::progressChanged, this, [this](qreal) {
        update();
    });

    QObject::connect(this, &QAbstractButton::toggled, this, [this](bool checked) {
        resolveSpecIfNeeded();
        if (!d->transition) {
            return;
        }
        checked ? d->transition->startForward() : d->transition->startBackward();
    });
}

QtMaterialRadioButton::QtMaterialRadioButton(const QString& text, QWidget* parent)
    : QtMaterialRadioButton(parent)
{
    setText(text);
}

QtMaterialRadioButton::~QtMaterialRadioButton() = default;

void QtMaterialRadioButton::themeChangedEvent(const Theme& theme)
{
    QtMaterialSelectionControl::themeChangedEvent(theme);
    d->specDirty = true;
    d->layoutDirty = true;
    updateGeometry();
    update();
}

void QtMaterialRadioButton::invalidateResolvedSpec()
{
    d->specDirty = true;
    d->layoutDirty = true;
}

void QtMaterialRadioButton::stateChangedEvent()
{
    QtMaterialSelectionControl::stateChangedEvent();
    update();
}

void QtMaterialRadioButton::resolveSpecIfNeeded() const
{
    if (!d->specDirty) {
        return;
    }

    SelectionSpecResolver factory;
    d->spec = factory.radioButtonSpec(theme(), density());
    const_cast<QtMaterialRadioButton*>(this)->setSpacing(d->spec.spacing);
    if (d->transition && theme().motion().contains(d->spec.motionToken)) {
        const MotionStyle motion = theme().motion().style(d->spec.motionToken);
        if (motion.durationMs > 0) {
            d->transition->setDuration(motion.durationMs);
        }
        d->transition->setEasingCurve(motion.easing);
    }
    d->specDirty = false;
    d->layoutDirty = true;
}

void QtMaterialRadioButton::invalidateLayoutCache()
{
    d->layoutDirty = true;
}

void QtMaterialRadioButton::resolveLayoutIfNeeded() const
{
    const_cast<QtMaterialRadioButton*>(this)->resolveSpecIfNeeded();

    if (!d->layoutDirty) {
        return;
    }

    const QRect bounds = rect();

    const int controlWidth = qMin(
        bounds.width(),
        qMax(d->spec.touchTarget.width(), d->spec.indicatorSize)
        );

    const int controlHeight = qMin(
        bounds.height(),
        qMax(d->spec.touchTarget.height(), d->spec.indicatorSize)
        );

    const bool rtl = layoutDirection() == Qt::RightToLeft;

    const int controlX = rtl ? qMax(0, bounds.width() - controlWidth) : 0;
    const int controlY = qMax(0, (bounds.height() - controlHeight) / 2);

    const QRect controlRect(controlX, controlY, controlWidth, controlHeight);

    const int indicatorX =
        controlRect.center().x() - d->spec.indicatorSize / 2;

    const int indicatorY =
        controlRect.center().y() - d->spec.indicatorSize / 2;

    const QRect indicatorBounds(
        indicatorX,
        indicatorY,
        d->spec.indicatorSize,
        d->spec.indicatorSize
        );

    d->cachedIndicatorRect = QRectF(indicatorBounds);
    d->cachedDotRect = centeredRect(d->cachedIndicatorRect, d->spec.dotSize);

    d->cachedStateLayerRect =
        SelectionRenderHelper::centeredStateLayerRect(
            indicatorBounds,
            d->spec.stateLayerSize
            );

    d->cachedFocusRingRect = d->cachedStateLayerRect.adjusted(
        -3.0,
        -3.0,
        3.0,
        3.0
        );

    const int gap = text().isEmpty() ? 0 : spacing();

    if (rtl) {
        d->cachedLabelRect = QRect(
            0,
            0,
            qMax(0, controlRect.left() - gap),
            height()
            );
    } else {
        const int labelX = controlRect.right() + 1 + gap;

        d->cachedLabelRect = QRect(
            labelX,
            0,
            qMax(0, width() - labelX),
            height()
            );
    }

    d->cachedIndicatorPath = QPainterPath();
    d->cachedIndicatorPath.addEllipse(d->cachedIndicatorRect);

    d->cachedRippleClipPath = QPainterPath();
    d->cachedRippleClipPath.addEllipse(d->cachedStateLayerRect);

    const QFont labelFont = SelectionRenderHelper::labelFont(
        theme(),
        d->spec.labelTypeRole,
        font()
        );

    const QFontMetrics metrics(labelFont);

    d->cachedElidedText = metrics.elidedText(
        text(),
        Qt::ElideRight,
        d->cachedLabelRect.width()
        );

    d->layoutDirty = false;
}

QSize QtMaterialRadioButton::sizeHint() const
{
    const_cast<QtMaterialRadioButton*>(this)->resolveSpecIfNeeded();

    const QFont labelFont = SelectionRenderHelper::labelFont(theme(), d->spec.labelTypeRole, font());
    const QFontMetrics metrics(labelFont);
    const int labelWidth = text().isEmpty() ? 0 : (metrics.horizontalAdvance(text()) + spacing());
    const int width = qMax(d->spec.touchTarget.width(), d->spec.indicatorSize) + labelWidth;
    const int height = qMax(d->spec.touchTarget.height(), qMax(d->spec.indicatorSize, metrics.height()));
    return QSize(width, height);
}

QSize QtMaterialRadioButton::minimumSizeHint() const
{
    const_cast<QtMaterialRadioButton*>(this)->resolveSpecIfNeeded();
    return QSize(d->spec.touchTarget.width(), qMax(d->spec.touchTarget.height(), d->spec.indicatorSize));
}

void QtMaterialRadioButton::resizeEvent(QResizeEvent* event)
{
    invalidateLayoutCache();
    QtMaterialSelectionControl::resizeEvent(event);
}

void QtMaterialRadioButton::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::FontChange || event->type() == QEvent::StyleChange || event->type() == QEvent::EnabledChange) {
        invalidateLayoutCache();
    }
    QtMaterialSelectionControl::changeEvent(event);
}

void QtMaterialRadioButton::mousePressEvent(QMouseEvent* event)
{
    if (d->ripple) {
        d->ripple->addRipple(QtMaterial::mousePosition(event));
    }
    QtMaterialSelectionControl::mousePressEvent(event);
}

void QtMaterialRadioButton::paintEvent(QPaintEvent*)
{
    resolveLayoutIfNeeded();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const bool enabled = isEnabled();
    const qreal progress = d->transition ? d->transition->progress() : (isChecked() ? 1.0 : 0.0);
    const qreal stateOpacity = SelectionRenderHelper::stateLayerOpacity(theme(), interactionState());

    SelectionRenderHelper::paintCircularStateLayer(
        &painter,
        d->cachedStateLayerRect,
        d->spec.stateLayerColor,
        stateOpacity);

    painter.save();

    const QColor ringColor = enabled
                                 ? blendColor(d->spec.unselectedOutlineColor, d->spec.selectedColor, progress)
                                 : d->spec.disabledColor;

    QPen pen(ringColor, d->spec.outlineWidth);
    pen.setCosmetic(true);

    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(d->cachedIndicatorPath);

    if (progress > 0.0) {
        QRectF dotRect = centeredRect(d->cachedIndicatorRect, d->spec.dotSize * progress);

        QColor dotColor = enabled ? d->spec.selectedColor : d->spec.disabledColor;
        dotColor.setAlphaF(dotColor.alphaF() * progress);

        painter.setPen(Qt::NoPen);
        painter.setBrush(dotColor);
        painter.drawEllipse(dotRect);
    }

    painter.restore();

    if (d->ripple) {
        d->ripple->setClipPath(d->cachedRippleClipPath);
        d->ripple->paint(&painter, d->spec.stateLayerColor);
    }

    if (!d->cachedElidedText.isEmpty()) {
        const QFont labelFont = SelectionRenderHelper::labelFont(theme(), d->spec.labelTypeRole, font());
        const QColor labelColor = enabled ? d->spec.labelColor : d->spec.disabledColor;

        SelectionRenderHelper::paintLabel(
            &painter,
            d->cachedLabelRect,
            labelAlignment(),
            d->cachedElidedText,
            labelColor,
            labelFont);
    }

    if (interactionState().isFocused()) {
        QtMaterialFocusIndicator::paintRectFocusRing(
            &painter,
            d->cachedFocusRingRect,
            d->spec.focusRingColor,
            d->cachedFocusRingRect.width() / 2.0,
            2.0);
    }
}

} // namespace QtMaterial
