#include "qtmaterial/widgets/progress/qtmateriallinearprogressindicator.h"

#include <QEvent>
#include <QPainter>
#include <QPalette>
#include <QVariantAnimation>
#include <QtGlobal>
#include <cmath>

namespace QtMaterial {
namespace {
qreal clampProgress(qreal value)
{
    if (std::isnan(value)) {
        return 0.0;
    }
    return qBound<qreal>(0.0, value, 1.0);
}

QColor fallbackActive(const QWidget* widget)
{
    return widget->palette().color(QPalette::Highlight);
}

QColor fallbackTrack(const QWidget* widget)
{
    QColor color = widget->palette().color(QPalette::Mid);
    color.setAlpha(qMax(32, color.alpha() / 2));
    return color;
}
} // namespace

QtMaterialLinearProgressIndicator::QtMaterialLinearProgressIndicator(QWidget* parent)
    : QWidget(parent)
{
    initAnimation();
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

QtMaterialLinearProgressIndicator::QtMaterialLinearProgressIndicator(const ProgressIndicatorSpec& spec, QWidget* parent)
    : QWidget(parent)
    , m_spec(spec)
{
    initAnimation();
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

QtMaterialLinearProgressIndicator::~QtMaterialLinearProgressIndicator() = default;

qreal QtMaterialLinearProgressIndicator::value() const noexcept { return m_value; }

void QtMaterialLinearProgressIndicator::setValue(qreal value)
{
    const qreal normalized = clampProgress(value);
    if (qFuzzyCompare(m_value + 1.0, normalized + 1.0)) {
        return;
    }
    m_value = normalized;
    emit valueChanged(m_value);
    update();
}

void QtMaterialLinearProgressIndicator::resetValue() { setValue(0.0); }

QtMaterialLinearProgressIndicator::Mode QtMaterialLinearProgressIndicator::mode() const noexcept { return m_mode; }

void QtMaterialLinearProgressIndicator::setMode(Mode mode)
{
    if (m_mode == mode) {
        return;
    }
    m_mode = mode;
    updateAnimationState();
    emit modeChanged(m_mode);
    update();
}

bool QtMaterialLinearProgressIndicator::invertedAppearance() const noexcept { return m_invertedAppearance; }

void QtMaterialLinearProgressIndicator::setInvertedAppearance(bool inverted)
{
    if (m_invertedAppearance == inverted) {
        return;
    }
    m_invertedAppearance = inverted;
    emit invertedAppearanceChanged(m_invertedAppearance);
    update();
}

QColor QtMaterialLinearProgressIndicator::activeColor() const { return m_spec.activeColor; }
void QtMaterialLinearProgressIndicator::setActiveColor(const QColor& color)
{
    if (m_spec.activeColor == color) return;
    m_spec.activeColor = color;
    emit specChanged();
    update();
}
void QtMaterialLinearProgressIndicator::resetActiveColor() { setActiveColor(QColor()); }

QColor QtMaterialLinearProgressIndicator::trackColor() const { return m_spec.trackColor; }
void QtMaterialLinearProgressIndicator::setTrackColor(const QColor& color)
{
    if (m_spec.trackColor == color) return;
    m_spec.trackColor = color;
    emit specChanged();
    update();
}
void QtMaterialLinearProgressIndicator::resetTrackColor() { setTrackColor(QColor()); }

int QtMaterialLinearProgressIndicator::trackGap() const noexcept { return m_spec.trackGap; }
void QtMaterialLinearProgressIndicator::setTrackGap(int gap)
{
    gap = qMax(0, gap);
    if (m_spec.trackGap == gap) return;
    m_spec.trackGap = gap;
    emit specChanged();
    update();
}

int QtMaterialLinearProgressIndicator::stopIndicatorSize() const noexcept { return m_spec.stopIndicatorSize; }
void QtMaterialLinearProgressIndicator::setStopIndicatorSize(int size)
{
    size = qMax(0, size);
    if (m_spec.stopIndicatorSize == size) return;
    m_spec.stopIndicatorSize = size;
    emit specChanged();
    updateGeometry();
    update();
}

ProgressIndicatorSpec QtMaterialLinearProgressIndicator::spec() const { return m_spec; }
void QtMaterialLinearProgressIndicator::setSpec(const ProgressIndicatorSpec& spec)
{
    m_spec = spec;
    initAnimation();
    emit specChanged();
    updateGeometry();
    update();
}

QSize QtMaterialLinearProgressIndicator::sizeHint() const
{
    const int h = qMax(1, m_spec.linearHeight) + m_spec.margins.top() + m_spec.margins.bottom();
    return QSize(240, h);
}

QSize QtMaterialLinearProgressIndicator::minimumSizeHint() const
{
    const int h = qMax(1, m_spec.linearHeight) + m_spec.margins.top() + m_spec.margins.bottom();
    return QSize(64, h);
}

void QtMaterialLinearProgressIndicator::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QRectF r = rect().adjusted(m_spec.margins.left(), m_spec.margins.top(), -m_spec.margins.right(), -m_spec.margins.bottom());
    const qreal h = qMax<qreal>(1.0, m_spec.linearHeight);
    QRectF track(r.left(), r.center().y() - h / 2.0, r.width(), h);
    const qreal radius = h / 2.0;

    painter.setPen(Qt::NoPen);
    painter.setBrush(resolvedTrackColor());
    painter.drawRoundedRect(track, radius, radius);

    painter.setBrush(resolvedActiveColor());
    if (m_mode == Mode::Determinate) {
        const qreal gap = m_value > 0.0 && m_value < 1.0 ? qMin<qreal>(m_spec.trackGap, track.width()) : 0.0;
        const qreal w = qMax<qreal>(0.0, track.width() * m_value - gap);
        QRectF active = track;
        active.setWidth(w);
        if (m_invertedAppearance || layoutDirection() == Qt::RightToLeft) {
            active.moveRight(track.right());
        }
        if (active.width() > 0.0) {
            painter.drawRoundedRect(active, radius, radius);
        }
        if (m_spec.stopIndicatorSize > 0 && m_value > 0.0 && m_value < 1.0) {
            painter.setBrush(resolvedStopColor());
            const qreal s = qMin<qreal>(m_spec.stopIndicatorSize, h);
            const qreal x = (m_invertedAppearance || layoutDirection() == Qt::RightToLeft) ? active.left() - gap / 2.0 : active.right() + gap / 2.0;
            painter.drawEllipse(QPointF(x, track.center().y()), s / 2.0, s / 2.0);
        }
        return;
    }

    const qreal segmentWidth = track.width() * 0.32;
    const qreal travel = track.width() + segmentWidth;
    qreal x = track.left() - segmentWidth + travel * m_phase;
    if (m_invertedAppearance || layoutDirection() == Qt::RightToLeft) {
        x = track.right() - travel * m_phase;
    }
    QRectF active(x, track.top(), segmentWidth, track.height());
    active = active.intersected(track);
    if (!active.isEmpty()) {
        painter.drawRoundedRect(active, radius, radius);
    }
}

void QtMaterialLinearProgressIndicator::showEvent(QShowEvent* event) { QWidget::showEvent(event); updateAnimationState(); }
void QtMaterialLinearProgressIndicator::hideEvent(QHideEvent* event) { QWidget::hideEvent(event); updateAnimationState(); }
void QtMaterialLinearProgressIndicator::changeEvent(QEvent* event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::PaletteChange || event->type() == QEvent::StyleChange) {
        update();
    }
}

void QtMaterialLinearProgressIndicator::initAnimation()
{
    if (!m_animation) {
        m_animation = new QVariantAnimation(this);
        connect(m_animation, &QVariantAnimation::valueChanged, this, [this](const QVariant& value) {
            m_phase = value.toReal();
            update();
        });
    }
    m_animation->setStartValue(0.0);
    m_animation->setEndValue(1.0);
    m_animation->setDuration(qMax(250, m_spec.animationDurationMs));
    m_animation->setLoopCount(-1);
}

void QtMaterialLinearProgressIndicator::updateAnimationState()
{
    if (!m_animation) return;
    if (m_mode == Mode::Indeterminate && isVisible()) {
        if (m_animation->state() != QVariantAnimation::Running) m_animation->start();
    } else {
        if (m_animation->state() != QVariantAnimation::Stopped) m_animation->stop();
        m_phase = 0.0;
    }
}

QColor QtMaterialLinearProgressIndicator::resolvedActiveColor() const { return m_spec.activeColor.isValid() ? m_spec.activeColor : fallbackActive(this); }
QColor QtMaterialLinearProgressIndicator::resolvedTrackColor() const { return m_spec.trackColor.isValid() ? m_spec.trackColor : fallbackTrack(this); }
QColor QtMaterialLinearProgressIndicator::resolvedStopColor() const { return m_spec.stopIndicatorColor.isValid() ? m_spec.stopIndicatorColor : resolvedActiveColor(); }

} // namespace QtMaterial
