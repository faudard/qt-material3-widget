#pragma once

#include <QElapsedTimer>
#include <QObject>
#include <QPainterPath>
#include <QPointer>
#include <QPointF>
#include <QTimer>
#include <QVector>

#include "qtmaterial/qtmaterialglobal.h"

class QWidget;
class QPainter;

namespace QtMaterial {

class QTMATERIAL3_EFFECTS_EXPORT QtMaterialRippleController : public QObject
{
    Q_OBJECT

public:
    enum class BoundsMode {
        Bounded,
        Unbounded
    };
    Q_ENUM(BoundsMode)

    enum class OriginMode {
        Pointer,
        Center
    };
    Q_ENUM(OriginMode)

    explicit QtMaterialRippleController(QWidget* target);
    ~QtMaterialRippleController() override;

    void setClipPath(const QPainterPath& path);
    void setClipRect(const QRectF& rect, qreal radius = 0.0);

    void setDuration(int durationMs);
    int duration() const noexcept;

    void setBaseOpacity(qreal opacity) noexcept;
    qreal baseOpacity() const noexcept;

    void setBoundsMode(BoundsMode mode) noexcept;
    BoundsMode boundsMode() const noexcept;

    void setOriginMode(OriginMode mode) noexcept;
    OriginMode originMode() const noexcept;

    void setEnabled(bool enabled);
    bool isEnabled() const noexcept;

    void setReducedMotion(bool reducedMotion);
    bool reducedMotion() const noexcept;

    void addRipple(const QPointF& pointerPosition);
    void clear();

    bool isActive() const noexcept;
    void paint(QPainter* painter, const QColor& color);

private slots:
    void advance();

private:
    friend class QtMaterialRippleControllerCaptureAccess;

    qreal targetRadiusFor(const QPointF& center) const;
    QPointF resolvedCenterFor(const QPointF& pointerPosition) const;

    struct Ripple
    {
        QPointF center;
        qreal endRadius = 0.0;
        qint64 startedMs = 0;
        qreal fixedProgress = -1.0;
    };

    QPointer<QWidget> m_target;
    QVector<Ripple> m_ripples;
    QPainterPath m_clipPath;
    QElapsedTimer m_clock;
    QTimer* m_timer = nullptr;
    int m_durationMs = 280;
    qreal m_baseOpacity = 0.18;
    BoundsMode m_boundsMode = BoundsMode::Bounded;
    OriginMode m_originMode = OriginMode::Pointer;
    bool m_enabled = true;
    bool m_reducedMotion = false;
};

} // namespace QtMaterial
