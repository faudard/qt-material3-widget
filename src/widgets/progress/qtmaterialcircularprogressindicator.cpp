#include "qtmaterial/widgets/progress/qtmaterialcircularprogressindicator.h"

#include <QEvent>
#include <QHideEvent>
#include <QPainter>
#include <QPalette>
#include <QShowEvent>
#include <QSizePolicy>
#include <QVariantAnimation>
#include <algorithm>
#include <cmath>

namespace QtMaterial {
namespace {
qreal clampProgress(qreal value) {
    if (std::isnan(value)) {
        return 0.0;
    }
    return qBound(0.0, value, 1.0);
}

QColor fallbackActive(const QWidget* widget) {
    return widget->palette().color(QPalette::Highlight);
}

QColor fallbackTrack(const QWidget* widget) {
    QColor color = widget->palette().color(QPalette::Mid);
    color.setAlpha(qMax(32, color.alpha() / 2));
    return color;
}
} // namespace

QtMaterialCircularProgressIndicator::QtMaterialCircularProgressIndicator(QWidget* parent)
    : QtMaterialWidget(parent) {
    setMaterialComponent(QStringLiteral("CircularProgressIndicator"));
    syncAsyncStateFromProgress();
    initAnimation();
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

QtMaterialCircularProgressIndicator::QtMaterialCircularProgressIndicator(const ProgressIndicatorSpec& spec, QWidget* parent)
    : QtMaterialWidget(parent)
    , m_spec(spec) {
    setMaterialComponent(QStringLiteral("CircularProgressIndicator"));
    syncAsyncStateFromProgress();
    initAnimation();
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

QtMaterialCircularProgressIndicator::~QtMaterialCircularProgressIndicator() = default;

qreal QtMaterialCircularProgressIndicator::value() const noexcept { return m_value; }

void QtMaterialCircularProgressIndicator::setValue(qreal value) {
    const qreal normalized = clampProgress(value);
    if (qFuzzyCompare(m_value + 1.0, normalized + 1.0)) return;
    m_value = normalized;
    if (m_mode == Mode::Determinate) {
        m_asyncState.setProgress(m_value);
        syncMaterialStateFromAsyncState();
        emit asyncStateChanged();
    }
    emit valueChanged(m_value);
    update();
}

void QtMaterialCircularProgressIndicator::resetValue() { setValue(0.0); }

QtMaterialCircularProgressIndicator::Mode QtMaterialCircularProgressIndicator::mode() const noexcept { return m_mode; }

void QtMaterialCircularProgressIndicator::setMode(Mode mode) {
    if (m_mode == mode) return;
    m_mode = mode;
    syncAsyncStateFromProgress();
    updateAnimationState();
    emit modeChanged(m_mode);
    emit asyncStateChanged();
    update();
}

bool QtMaterialCircularProgressIndicator::isBusy() const noexcept { return m_asyncState.isBusy(); }

void QtMaterialCircularProgressIndicator::setBusy(bool busy) {
    if (m_asyncState.isBusy() == busy) return;
    m_asyncState.setBusy(busy);
    syncMaterialStateFromAsyncState();
    emit asyncStateChanged();
    update();
}

bool QtMaterialCircularProgressIndicator::isIndeterminate() const noexcept {
    return m_mode == Mode::Indeterminate;
}

void QtMaterialCircularProgressIndicator::setIndeterminate(bool indeterminate) {
    setMode(indeterminate ? Mode::Indeterminate : Mode::Determinate);
}

QString QtMaterialCircularProgressIndicator::statusText() const { return m_asyncState.statusText(); }

void QtMaterialCircularProgressIndicator::setStatusText(const QString& text) {
    if (m_asyncState.statusText() == text) return;
    m_asyncState.setStatusText(text);
    setAccessibleDescription(text);
    syncMaterialStateFromAsyncState();
    emit asyncStateChanged();
}

QtMaterialAsyncState QtMaterialCircularProgressIndicator::asyncState() const { return m_asyncState; }

void QtMaterialCircularProgressIndicator::setAsyncState(const QtMaterialAsyncState& state) {
    m_asyncState = state;
    m_mode = m_asyncState.isIndeterminate() ? Mode::Indeterminate : Mode::Determinate;
    if (m_asyncState.hasProgress()) {
        m_value = m_asyncState.progress();
    }
    setAccessibleDescription(m_asyncState.statusText());
    syncMaterialStateFromAsyncState();
    updateAnimationState();
    emit modeChanged(m_mode);
    emit valueChanged(m_value);
    emit asyncStateChanged();
    update();
}

QColor QtMaterialCircularProgressIndicator::activeColor() const { return m_spec.activeColor; }

void QtMaterialCircularProgressIndicator::setActiveColor(const QColor& color) {
    if (m_spec.activeColor == color) return;
    m_spec.activeColor = color;
    emit specChanged();
    update();
}

void QtMaterialCircularProgressIndicator::resetActiveColor() { setActiveColor(QColor()); }

QColor QtMaterialCircularProgressIndicator::trackColor() const { return m_spec.trackColor; }

void QtMaterialCircularProgressIndicator::setTrackColor(const QColor& color) {
    if (m_spec.trackColor == color) return;
    m_spec.trackColor = color;
    emit specChanged();
    update();
}

void QtMaterialCircularProgressIndicator::resetTrackColor() { setTrackColor(QColor()); }

int QtMaterialCircularProgressIndicator::trackGap() const noexcept { return m_spec.trackGap; }

void QtMaterialCircularProgressIndicator::setTrackGap(int gap) {
    gap = qMax(0, gap);
    if (m_spec.trackGap == gap) return;
    m_spec.trackGap = gap;
    emit specChanged();
    update();
}

int QtMaterialCircularProgressIndicator::strokeWidth() const noexcept { return m_spec.circularStrokeWidth; }

void QtMaterialCircularProgressIndicator::setStrokeWidth(int width) {
    width = qMax(1, width);
    if (m_spec.circularStrokeWidth == width) return;
    m_spec.circularStrokeWidth = width;
    emit specChanged();
    updateGeometry();
    update();
}

ProgressIndicatorSpec QtMaterialCircularProgressIndicator::spec() const { return m_spec; }

void QtMaterialCircularProgressIndicator::setSpec(const ProgressIndicatorSpec& spec) {
    m_spec = spec;
    initAnimation();
    emit specChanged();
    updateGeometry();
    update();
}

QSize QtMaterialCircularProgressIndicator::sizeHint() const {
    return m_spec.circularSize.expandedTo(QSize(16, 16));
}

QSize QtMaterialCircularProgressIndicator::minimumSizeHint() const { return QSize(24, 24); }

void QtMaterialCircularProgressIndicator::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    const int stroke = qMax(1, m_spec.circularStrokeWidth);
    QRectF arcRect = rect().adjusted(stroke, stroke, -stroke, -stroke);
    if (arcRect.isEmpty()) return;
    QPen trackPen(resolvedTrackColor(), stroke, Qt::SolidLine, Qt::RoundCap);
    QPen activePen(resolvedActiveColor(), stroke, Qt::SolidLine, Qt::RoundCap);
    painter.setPen(trackPen);
    const int gap16 = qMax(0, m_spec.trackGap) * 16;
    painter.drawArc(arcRect, gap16, 360 * 16 - (2 * gap16));
    painter.setPen(activePen);
    if (m_mode == Mode::Determinate) {
        const int span = qRound(360.0 * 16.0 * m_value) - (m_value > 0.0 && m_value < 1.0 ? gap16 : 0);
        if (span > 0) painter.drawArc(arcRect, 90 * 16, -span);
        return;
    }
    const int start = qRound((90.0 - 360.0 * m_phase) * 16.0);
    const int span = -qRound((72.0 + 108.0 * (0.5 + 0.5 * std::sin(m_phase * 6.283185307179586))) * 16.0);
    painter.drawArc(arcRect, start, span);
}

void QtMaterialCircularProgressIndicator::showEvent(QShowEvent* event) {
    QtMaterialWidget::showEvent(event);
    updateAnimationState();
}

void QtMaterialCircularProgressIndicator::hideEvent(QHideEvent* event) {
    QtMaterialWidget::hideEvent(event);
    updateAnimationState();
}

void QtMaterialCircularProgressIndicator::changeEvent(QEvent* event) {
    QtMaterialWidget::changeEvent(event);
    if (event->type() == QEvent::PaletteChange || event->type() == QEvent::StyleChange) update();
}

void QtMaterialCircularProgressIndicator::initAnimation() {
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

void QtMaterialCircularProgressIndicator::updateAnimationState() {
    if (!m_animation) return;
    if (m_mode == Mode::Indeterminate && isVisible()) {
        if (m_animation->state() != QVariantAnimation::Running) m_animation->start();
    } else {
        if (m_animation->state() != QVariantAnimation::Stopped) m_animation->stop();
        m_phase = 0.0;
    }
}

void QtMaterialCircularProgressIndicator::syncAsyncStateFromProgress() {
    if (m_mode == Mode::Indeterminate) {
        m_asyncState.clearProgress();
        m_asyncState.setIndeterminate(true);
        m_asyncState.setBusy(true);
    } else {
        m_asyncState.setIndeterminate(false);
        m_asyncState.setProgress(m_value);
    }
    syncMaterialStateFromAsyncState();
}

void QtMaterialCircularProgressIndicator::syncMaterialStateFromAsyncState() {
    setMaterialState(m_asyncState.toPropertyString());
}

QColor QtMaterialCircularProgressIndicator::resolvedActiveColor() const {
    return m_spec.activeColor.isValid() ? m_spec.activeColor : fallbackActive(this);
}

QColor QtMaterialCircularProgressIndicator::resolvedTrackColor() const {
    return m_spec.trackColor.isValid() ? m_spec.trackColor : fallbackTrack(this);
}

} // namespace QtMaterial
