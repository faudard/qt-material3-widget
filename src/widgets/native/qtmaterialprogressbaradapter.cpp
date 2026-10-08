#include "qtmaterial/widgets/native/qtmaterialprogressbaradapter.h"

#include <QApplication>
#include <QPainter>
#include <QPointer>
#include <QProgressBar>
#include <QProxyStyle>
#include <QStyleOption>
#include <QTimer>
#include <QVariant>
#include <QWidget>

#include "../resolution/qtmaterialprogressspecresolution_p.h"
#include "qtmaterial/core/private/qtmaterialthemecontextbinding_p.h"

namespace QtMaterial {
namespace {

constexpr char kAppliedProperty[] = "qtm3MaterialProgressBar";
constexpr char kOptOutProperty[] = "qtm3MaterialOptOut";
constexpr char kStateObjectName[] = "_qtm3_native_progressbar_adapter_state";

bool isBusy(const QProgressBar* progressBar)
{
    return progressBar
        && progressBar->minimum() == 0
        && progressBar->maximum() == 0;
}

qreal normalizedProgress(const QProgressBar* progressBar)
{
    if (!progressBar
        || progressBar->maximum() <= progressBar->minimum()) {
        return 0.0;
    }

    const qreal numerator =
        qreal(progressBar->value() - progressBar->minimum());
    const qreal denominator =
        qreal(progressBar->maximum() - progressBar->minimum());
    return qBound<qreal>(
        0.0,
        numerator / denominator,
        1.0);
}

bool reverseDirection(const QProgressBar* progressBar)
{
    if (!progressBar) {
        return false;
    }

    if (progressBar->orientation() == Qt::Horizontal) {
        const bool rtl =
            progressBar->layoutDirection() == Qt::RightToLeft;
        return progressBar->invertedAppearance() ^ rtl;
    }

    // Native vertical progress grows bottom-to-top by default.
    return !progressBar->invertedAppearance();
}

QRectF trackRect(
    const QRect& bounds,
    Qt::Orientation orientation,
    int thickness)
{
    const qreal extent = qMax(1, thickness);
    if (orientation == Qt::Horizontal) {
        return QRectF(
            bounds.left(),
            bounds.center().y() - extent / 2.0,
            bounds.width(),
            extent);
    }

    return QRectF(
        bounds.center().x() - extent / 2.0,
        bounds.top(),
        extent,
        bounds.height());
}

class NativeProgressBarProxyStyle final : public QProxyStyle
{
public:
    explicit NativeProgressBarProxyStyle(
        const ProgressIndicatorSpec& spec)
        : QProxyStyle()
        , m_spec(spec)
    {
    }

    void setResolvedSpec(const ProgressIndicatorSpec& spec)
    {
        m_spec = spec;
    }

    void setPhase(qreal phase)
    {
        m_phase = qBound<qreal>(0.0, phase, 1.0);
    }

    QSize sizeFromContents(
        ContentsType type,
        const QStyleOption* option,
        const QSize& contentsSize,
        const QWidget* widget = nullptr) const override
    {
        QSize size = QProxyStyle::sizeFromContents(
            type,
            option,
            contentsSize,
            widget);

        const auto* progressBar =
            qobject_cast<const QProgressBar*>(widget);
        if (type != CT_ProgressBar
            || !progressBar
            || !QtMaterialProgressBarAdapter::isApplied(progressBar)) {
            return size;
        }

        const int minimumCrossAxis =
            qMax(
                m_spec.linearHeight,
                progressBar->isTextVisible() ? 20 : m_spec.linearHeight);

        if (progressBar->orientation() == Qt::Horizontal) {
            size.setHeight(qMax(size.height(), minimumCrossAxis));
        } else {
            size.setWidth(qMax(size.width(), minimumCrossAxis));
        }
        return size;
    }

    void drawControl(
        ControlElement element,
        const QStyleOption* option,
        QPainter* painter,
        const QWidget* widget = nullptr) const override
    {
        const auto* progressBar =
            qobject_cast<const QProgressBar*>(widget);
        const auto* progressOption =
            qstyleoption_cast<const QStyleOptionProgressBar*>(option);

        if (element != CE_ProgressBar
            || !progressBar
            || !progressOption
            || !QtMaterialProgressBarAdapter::isApplied(progressBar)) {
            QProxyStyle::drawControl(
                element,
                option,
                painter,
                widget);
            return;
        }

        const QRectF track =
            trackRect(
                progressOption->rect,
                progressBar->orientation(),
                m_spec.linearHeight);
        if (!track.isValid()) {
            return;
        }

        const qreal crossExtent =
            progressBar->orientation() == Qt::Horizontal
                ? track.height()
                : track.width();
        const qreal radius =
            m_spec.cornerRadius < 0.0
                ? crossExtent / 2.0
                : qMin<qreal>(
                      m_spec.cornerRadius,
                      crossExtent / 2.0);

        painter->save();
        painter->setRenderHint(
            QPainter::Antialiasing,
            true);
        painter->setPen(Qt::NoPen);
        painter->setBrush(m_spec.trackColor);
        painter->drawRoundedRect(track, radius, radius);

        painter->setBrush(m_spec.activeColor);

        if (isBusy(progressBar)) {
            paintBusySegment(
                painter,
                track,
                radius,
                progressBar);
        } else {
            paintDeterminate(
                painter,
                track,
                radius,
                progressBar);
        }

        painter->restore();

        if (progressBar->isTextVisible()) {
            QStyleOptionProgressBar labelOption(*progressOption);
            QProxyStyle::drawControl(
                CE_ProgressBarLabel,
                &labelOption,
                painter,
                widget);
        }
    }

private:
    void paintDeterminate(
        QPainter* painter,
        const QRectF& track,
        qreal radius,
        const QProgressBar* progressBar) const
    {
        const qreal value = normalizedProgress(progressBar);
        if (value <= 0.0) {
            return;
        }

        const bool reverse = reverseDirection(progressBar);
        QRectF active = track;

        if (progressBar->orientation() == Qt::Horizontal) {
            qreal extent = track.width() * value;
            const qreal gap =
                value < 1.0
                    ? qMin<qreal>(m_spec.trackGap, extent)
                    : 0.0;
            extent = qMax<qreal>(0.0, extent - gap);
            active.setWidth(extent);
            if (reverse) {
                active.moveRight(track.right());
            }
        } else {
            qreal extent = track.height() * value;
            const qreal gap =
                value < 1.0
                    ? qMin<qreal>(m_spec.trackGap, extent)
                    : 0.0;
            extent = qMax<qreal>(0.0, extent - gap);
            active.setHeight(extent);
            if (reverse) {
                active.moveBottom(track.bottom());
            }
        }

        if (!active.isEmpty()) {
            painter->drawRoundedRect(active, radius, radius);
        }

        if (m_spec.stopIndicatorSize <= 0
            || value <= 0.0
            || value >= 1.0) {
            return;
        }

        painter->setBrush(m_spec.stopIndicatorColor);
        const qreal stopSize =
            qMin<qreal>(
                m_spec.stopIndicatorSize,
                qMax<qreal>(1.0, crossExtent(track, progressBar)));

        QPointF stopCenter;
        if (progressBar->orientation() == Qt::Horizontal) {
            const qreal x =
                reverse ? active.left() : active.right();
            stopCenter = QPointF(x, track.center().y());
        } else {
            const qreal y =
                reverse ? active.top() : active.bottom();
            stopCenter = QPointF(track.center().x(), y);
        }

        painter->drawEllipse(
            stopCenter,
            stopSize / 2.0,
            stopSize / 2.0);
    }

    void paintBusySegment(
        QPainter* painter,
        const QRectF& track,
        qreal radius,
        const QProgressBar* progressBar) const
    {
        const bool reverse = reverseDirection(progressBar);

        if (progressBar->orientation() == Qt::Horizontal) {
            const qreal segment =
                qMax<qreal>(track.width() * 0.32, m_spec.linearHeight);
            const qreal travel = track.width() + segment;
            qreal left =
                track.left() - segment + travel * m_phase;
            if (reverse) {
                left =
                    track.right() - travel * m_phase;
            }
            QRectF active(
                left,
                track.top(),
                segment,
                track.height());
            active = active.intersected(track);
            if (!active.isEmpty()) {
                painter->drawRoundedRect(active, radius, radius);
            }
            return;
        }

        const qreal segment =
            qMax<qreal>(track.height() * 0.32, m_spec.linearHeight);
        const qreal travel = track.height() + segment;
        qreal top =
            track.top() - segment + travel * m_phase;
        if (reverse) {
            top =
                track.bottom() - travel * m_phase;
        }
        QRectF active(
            track.left(),
            top,
            track.width(),
            segment);
        active = active.intersected(track);
        if (!active.isEmpty()) {
            painter->drawRoundedRect(active, radius, radius);
        }
    }

    static qreal crossExtent(
        const QRectF& track,
        const QProgressBar* progressBar)
    {
        return progressBar->orientation() == Qt::Horizontal
            ? track.height()
            : track.width();
    }

    ProgressIndicatorSpec m_spec;
    qreal m_phase = 0.0;
};

class ProgressBarAdapterState final : public QObject
{
public:
    explicit ProgressBarAdapterState(QProgressBar* progressBar)
        : QObject(progressBar)
        , m_progressBar(progressBar)
        , m_previousStyle(
              progressBar ? progressBar->style() : nullptr)
        , m_themeBinding(
              new QtMaterialThemeContextBinding(
                  progressBar,
                  this))
        , m_style(
              new NativeProgressBarProxyStyle(
                  resolvedSpec()))
        , m_timer(new QTimer(this))
    {
        setObjectName(
            QString::fromLatin1(kStateObjectName));
        m_style->setParent(this);

        if (m_progressBar) {
            m_progressBar->setStyle(m_style);
        }

        m_timer->setInterval(45);
        QObject::connect(
            m_timer,
            &QTimer::timeout,
            this,
            [this]() {
                if (!m_progressBar
                    || !m_progressBar->isVisible()
                    || !isBusy(m_progressBar)
                    || m_reducedMotion) {
                    return;
                }

                m_phase += 0.035;
                if (m_phase > 1.0) {
                    m_phase -= 1.0;
                }
                m_style->setPhase(m_phase);
                m_progressBar->update();
            });
        m_timer->start();

        QObject::connect(
            m_themeBinding,
            &QtMaterialThemeContextBinding::themeChanged,
            this,
            [this]() { refreshResolvedSpec(); });
        QObject::connect(
            m_themeBinding,
            &QtMaterialThemeContextBinding::
                effectiveThemeContextChanged,
            this,
            [this]() { refreshResolvedSpec(); });

        refreshResolvedSpec();
    }

    void restore()
    {
        if (!m_progressBar) {
            return;
        }

        if (m_progressBar->style() == m_style) {
            QStyle* restoreStyle =
                m_previousStyle.data();
            if (!restoreStyle) {
                restoreStyle =
                    QApplication::style();
            }
            m_progressBar->setStyle(restoreStyle);
        }
    }

private:
    ProgressIndicatorSpec resolvedSpec() const
    {
        Q_ASSERT(m_themeBinding);
        return ProgressSpecResolution::linearProgressSpec(
            m_themeBinding);
    }

    void refreshResolvedSpec()
    {
        if (!m_progressBar
            || !m_style
            || !m_themeBinding) {
            return;
        }

        m_style->setResolvedSpec(resolvedSpec());
        m_reducedMotion =
            ProgressSpecResolution::reducedMotion(
                m_themeBinding);
        if (m_reducedMotion) {
            m_phase = 0.5;
            m_style->setPhase(m_phase);
        }
        m_progressBar->updateGeometry();
        m_progressBar->update();
    }

    QPointer<QProgressBar> m_progressBar;
    QPointer<QStyle> m_previousStyle;
    QtMaterialThemeContextBinding*
        m_themeBinding = nullptr;
    NativeProgressBarProxyStyle* m_style = nullptr;
    QTimer* m_timer = nullptr;
    qreal m_phase = 0.0;
    bool m_reducedMotion = false;
};

ProgressBarAdapterState* adapterState(QProgressBar* progressBar)
{
    if (!progressBar) {
        return nullptr;
    }

    QObject* object =
        progressBar->findChild<QObject*>(
            QString::fromLatin1(kStateObjectName),
            Qt::FindDirectChildrenOnly);
    return static_cast<ProgressBarAdapterState*>(object);
}

} // namespace

void QtMaterialProgressBarAdapter::apply(
    QProgressBar* progressBar)
{
    if (!progressBar) {
        return;
    }

    progressBar->setProperty(
        kAppliedProperty,
        true);

    if (!adapterState(progressBar)) {
        new ProgressBarAdapterState(progressBar);
    }

    progressBar->updateGeometry();
    progressBar->update();
}

void QtMaterialProgressBarAdapter::remove(
    QProgressBar* progressBar)
{
    if (!progressBar) {
        return;
    }

    if (ProgressBarAdapterState* state =
            adapterState(progressBar)) {
        state->restore();
        delete state;
    }

    progressBar->setProperty(
        kAppliedProperty,
        QVariant());
    progressBar->updateGeometry();
    progressBar->update();
}

bool QtMaterialProgressBarAdapter::isApplied(
    const QProgressBar* progressBar)
{
    return progressBar
        && progressBar->property(
                         kAppliedProperty)
               .toBool();
}

void QtMaterialProgressBarAdapter::setOptOut(
    QProgressBar* progressBar,
    bool excluded)
{
    if (!progressBar) {
        return;
    }

    if (excluded) {
        remove(progressBar);
    }
    progressBar->setProperty(
        kOptOutProperty,
        excluded);
}

bool QtMaterialProgressBarAdapter::isOptedOut(
    const QProgressBar* progressBar)
{
    return progressBar
        && progressBar->property(
                         kOptOutProperty)
               .toBool();
}

int QtMaterialProgressBarAdapter::applyToDescendants(
    QWidget* root)
{
    if (!root) {
        return 0;
    }

    int count = 0;
    if (auto* rootProgress =
            qobject_cast<QProgressBar*>(root)) {
        if (!isOptedOut(rootProgress)) {
            apply(rootProgress);
            ++count;
        }
    }

    const auto progressBars =
        root->findChildren<QProgressBar*>();
    for (QProgressBar* progressBar : progressBars) {
        if (!progressBar
            || isOptedOut(progressBar)) {
            continue;
        }
        apply(progressBar);
        ++count;
    }

    return count;
}

const char*
QtMaterialProgressBarAdapter::
    appliedPropertyName() noexcept
{
    return kAppliedProperty;
}

const char*
QtMaterialProgressBarAdapter::
    optOutPropertyName() noexcept
{
    return kOptOutProperty;
}

} // namespace QtMaterial
