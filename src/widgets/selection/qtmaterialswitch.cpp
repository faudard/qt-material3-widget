#include "qtmaterial/widgets/selection/qtmaterialswitch.h"
#include <memory>

// #include <QChangeEvent>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QRectF>
#include <QResizeEvent>
#include <QStyle>

#include "private/qtmaterialselectionrenderhelper_p.h"
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialswitchspec.h"
#include "qtmaterial/specs/qtmaterialselectionspecresolver.h"
#include "qtmaterial/core/qtmaterialeventcompat.h"

namespace QtMaterial {


struct QtMaterialSwitchPrivate
{
 bool m_specDirty = true;
 bool m_layoutDirty = true;
 SwitchSpec m_spec;
 QtMaterialTransitionController* m_transition = nullptr;
 QtMaterialRippleController* m_ripple = nullptr;
 QRectF m_cachedTrackRect;
 QRectF m_cachedStateLayerRect;
 QRectF m_cachedHandleRect;
 QRectF m_cachedFocusRingRect;
 QRect m_cachedLabelRect;
 QPainterPath m_cachedTrackPath;
 QPainterPath m_cachedRippleClipPath;
 QPainterPath m_cachedFocusRingPath;
 QString m_cachedElidedText;
 QFont m_cachedLabelFont;
};

namespace {

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

} // namespace

QtMaterialSwitch::QtMaterialSwitch(QWidget* parent)
 : QtMaterialSelectionControl(parent)
 , d(std::make_unique<QtMaterialSwitchPrivate>())
{
 d->m_transition = new QtMaterialTransitionController(this);
 d->m_ripple = new QtMaterialRippleController(this);

    setCheckable(true);
    setFocusPolicy(Qt::StrongFocus);
    setAccessibleDescription(QStringLiteral("Material 3 switch"));

    QObject::connect(d->m_transition, &QtMaterialTransitionController::progressChanged, this, [this](qreal) {
        d->m_layoutDirty = true;
        update();
    });
    QObject::connect(this, &QAbstractButton::toggled, this, [this](bool) {
        syncTransitionState(true);
        syncAccessibleState();
    });

    syncTransitionState(false);
    syncAccessibleState();
}

QtMaterialSwitch::QtMaterialSwitch(const QString& text, QWidget* parent)
    : QtMaterialSwitch(parent)
{
    setText(text);
}

QtMaterialSwitch::~QtMaterialSwitch() = default;

void QtMaterialSwitch::themeChangedEvent(const Theme& theme)
{
    QtMaterialSelectionControl::themeChangedEvent(theme);
    d->m_specDirty = true;
    d->m_layoutDirty = true;
    updateGeometry();
    update();
}

void QtMaterialSwitch::invalidateResolvedSpec()
{
    d->m_specDirty = true;
    d->m_layoutDirty = true;
}

void QtMaterialSwitch::stateChangedEvent()
{
    QtMaterialSelectionControl::stateChangedEvent();
    syncAccessibleState();
    update();
}

void QtMaterialSwitch::contentChangedEvent()
{
    QtMaterialSelectionControl::contentChangedEvent();
    invalidateLayoutCache();
    syncAccessibleState();
    updateGeometry();
    update();
}

void QtMaterialSwitch::resolveSpecIfNeeded() const
{
    if (!d->m_specDirty) {
        return;
    }

    SelectionSpecResolver factory;
    d->m_spec = factory.switchSpec(theme(), density());
    const_cast<QtMaterialSwitch*>(this)->setSpacing(d->m_spec.spacing);
    SelectionRenderHelper::configureMotion(
        d->m_spec,
        d->m_transition,
        d->m_ripple);
    d->m_specDirty = false;
    d->m_layoutDirty = true;
}

void QtMaterialSwitch::invalidateLayoutCache()
{
    d->m_layoutDirty = true;
}

void QtMaterialSwitch::resolveLayoutIfNeeded() const
{
    const_cast<QtMaterialSwitch*>(this)->resolveSpecIfNeeded();

    if (!d->m_layoutDirty) {
        return;
    }

    const QRect bounds = rect();

    const int controlWidth = qMin(
        bounds.width(),
        qMax(d->m_spec.touchTarget.width(), d->m_spec.trackWidth)
        );

    const int controlHeight = qMin(
        bounds.height(),
        qMax(d->m_spec.touchTarget.height(), d->m_spec.trackHeight)
        );

    const bool rtl = layoutDirection() == Qt::RightToLeft;

    const int controlX = rtl ? qMax(0, bounds.width() - controlWidth) : 0;
    const int controlY = qMax(0, (bounds.height() - controlHeight) / 2);

    const QRect controlRect(controlX, controlY, controlWidth, controlHeight);

    QRectF localTrackRect = SelectionRenderHelper::switchTrackRect(
        QRect(0, 0, controlRect.width(), controlRect.height()),
        d->m_spec
        );

    localTrackRect.translate(controlRect.topLeft());

    const qreal progress = d->m_transition
                               ? d->m_transition->progress()
                               : (isChecked() ? 1.0 : 0.0);

    d->m_cachedTrackRect = localTrackRect;
    d->m_cachedTrackPath = SelectionRenderHelper::roundedTrackPath(d->m_cachedTrackRect);
    d->m_cachedHandleRect = SelectionRenderHelper::switchHandleRect(
        d->m_cachedTrackRect,
        d->m_spec,
        progress
        );

    const QRect handleBounds = d->m_cachedHandleRect.toAlignedRect();

    d->m_cachedStateLayerRect = SelectionRenderHelper::centeredStateLayerRect(
        handleBounds,
        d->m_spec.stateLayerSize
        );

    d->m_cachedFocusRingRect = d->m_cachedTrackRect.adjusted(
        -3.0,
        -3.0,
        3.0,
        3.0
        );

    const int gap = text().isEmpty() ? 0 : spacing();

    if (rtl) {
        d->m_cachedLabelRect = QRect(
            0,
            0,
            qMax(0, controlRect.left() - gap),
            height()
            );
    } else {
        const int labelX = controlRect.right() + 1 + gap;

        d->m_cachedLabelRect = QRect(
            labelX,
            0,
            qMax(0, width() - labelX),
            height()
            );
    }

    const qreal trackRadius = d->m_cachedTrackRect.height() / 2.0;

    d->m_cachedRippleClipPath = QPainterPath();
    d->m_cachedRippleClipPath.addEllipse(d->m_cachedStateLayerRect);

    d->m_cachedFocusRingPath = QPainterPath();
    d->m_cachedFocusRingPath.addRoundedRect(
        d->m_cachedFocusRingRect,
        d->m_cachedFocusRingRect.height() / 2.0,
        d->m_cachedFocusRingRect.height() / 2.0
        );

    d->m_cachedLabelFont = SelectionRenderHelper::resolvedLabelFont(font(), d->m_spec);

    const QFontMetrics metrics(d->m_cachedLabelFont);
    d->m_cachedElidedText = metrics.elidedText(
        text(),
        Qt::ElideRight,
        d->m_cachedLabelRect.width()
        );

    d->m_layoutDirty = false;
}

void QtMaterialSwitch::syncTransitionState(bool animated)
{
    if (!d->m_transition) {
        return;
    }

    if (animated) {
        if (isChecked()) {
            d->m_transition->startForward();
        } else {
            d->m_transition->startBackward();
        }
    } else {
        d->m_transition->setProgress(isChecked() ? 1.0 : 0.0);
    }
}

void QtMaterialSwitch::syncAccessibleState()
{
    if (accessibleName().isEmpty() && !text().isEmpty()) {
        setAccessibleName(text());
    }

    setAccessibleDescription(isChecked()
                                 ? QStringLiteral("On")
                                 : QStringLiteral("Off"));
}

QSize QtMaterialSwitch::sizeHint() const
{
    const_cast<QtMaterialSwitch*>(this)->resolveSpecIfNeeded();

    const QFont labelFont = SelectionRenderHelper::resolvedLabelFont(font(), d->m_spec);
    const QFontMetrics metrics(labelFont);
    const int labelWidth = text().isEmpty() ? 0 : (metrics.horizontalAdvance(text()) + spacing());
    const int width = qMax(d->m_spec.touchTarget.width(), d->m_spec.trackWidth) + labelWidth;
    const int height = qMax(d->m_spec.touchTarget.height(), qMax(d->m_spec.trackHeight, metrics.height()));
    return QSize(width, height);
}

QSize QtMaterialSwitch::minimumSizeHint() const
{
    const_cast<QtMaterialSwitch*>(this)->resolveSpecIfNeeded();
    return QSize(d->m_spec.touchTarget.width(), qMax(d->m_spec.touchTarget.height(), d->m_spec.trackHeight));
}

void QtMaterialSwitch::resizeEvent(QResizeEvent* event)
{
    invalidateLayoutCache();
    QtMaterialSelectionControl::resizeEvent(event);
}

void QtMaterialSwitch::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::FontChange || event->type() == QEvent::StyleChange || event->type() == QEvent::EnabledChange) {
        invalidateLayoutCache();
    }
    QtMaterialSelectionControl::changeEvent(event);
}

void QtMaterialSwitch::mousePressEvent(QMouseEvent* event)
{
    if (d->m_ripple) {
        d->m_ripple->addRipple(QtMaterial::mousePosition(event));
    }
    QtMaterialSelectionControl::mousePressEvent(event);
}

void QtMaterialSwitch::keyPressEvent(QKeyEvent* event)
{
    const bool rtl = layoutDirection() == Qt::RightToLeft;

    if (event->key() == Qt::Key_Left) {
        setChecked(rtl ? true : false);
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_Right) {
        setChecked(rtl ? false : true);
        event->accept();
        return;
    }

    QtMaterialSelectionControl::keyPressEvent(event);
}

void QtMaterialSwitch::paintEvent(QPaintEvent*)
{
    resolveLayoutIfNeeded();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const bool enabled = isEnabled();
    const qreal progress = d->m_transition ? d->m_transition->progress() : (isChecked() ? 1.0 : 0.0);
    const qreal stateOpacity = SelectionRenderHelper::stateLayerOpacity(d->m_spec, interactionState());

    const QColor offTrack = enabled
                                ? d->m_spec.unselectedTrackColor
                                : d->m_spec.disabledUnselectedTrackColor;
    const QColor onTrack = enabled
                               ? d->m_spec.selectedTrackColor
                               : d->m_spec.disabledSelectedTrackColor;

    const QColor offHandle = enabled
                                 ? d->m_spec.unselectedHandleColor
                                 : d->m_spec.disabledUnselectedHandleColor;
    const QColor onHandle = enabled
                                ? d->m_spec.selectedHandleColor
                                : d->m_spec.disabledSelectedHandleColor;

    const QColor trackColor = blendColor(offTrack, onTrack, progress);
    const QColor handleColor = blendColor(offHandle, onHandle, progress);

    SelectionRenderHelper::paintCircularStateLayer(&painter, d->m_cachedStateLayerRect, d->m_spec.stateLayerColor, stateOpacity);

    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(trackColor);
    painter.drawPath(d->m_cachedTrackPath);
    painter.setBrush(handleColor);
    painter.drawEllipse(d->m_cachedHandleRect);
    painter.restore();

    if (d->m_ripple) {
        d->m_ripple->setClipPath(d->m_cachedRippleClipPath);
        d->m_ripple->paint(&painter, d->m_spec.stateLayerColor);
    }

    if (!d->m_cachedElidedText.isEmpty()) {
        const QColor labelColor = enabled ? d->m_spec.labelColor : d->m_spec.disabledLabelColor;
        SelectionRenderHelper::paintLabel(&painter,
                                          d->m_cachedLabelRect,
                                          labelAlignment(),
                                          d->m_cachedElidedText,
                                          labelColor,
                                          d->m_cachedLabelFont);
    }

    if (interactionState().isFocused()) {
        painter.save();
        QtMaterialFocusIndicator::paintPathFocusRing(&painter,
                                                     d->m_cachedFocusRingPath,
                                                     d->m_spec.focusRingColor,
                                                     2.0);
        painter.restore();
    }
}

} // namespace QtMaterial
