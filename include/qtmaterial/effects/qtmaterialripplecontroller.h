#pragma once

#include <QElapsedTimer>
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
    explicit QtMaterialRippleController(QWidget* target);
    ~QtMaterialRippleController() override;

    void setClipPath(const QPainterPath& path);
    void setClipRect(const QRectF& rect, qreal radius = 0.0);

    void setDuration(int durationMs);
    int duration() const noexcept;

    void setBaseOpacity(qreal opacity) noexcept;
    qreal baseOpacity() const noexcept;

    void addRipple(const QPointF& center);
    void clear();

    bool isActive() const noexcept;
    void paint(QPainter* painter, const QColor& color);

private slots:
    void advance();

private:
    qreal targetRadiusFor(const QPointF& center) const;

    struct Ripple
    {
        QPointF center;
        qreal endRadius = 0.0;
        qint64 startedMs = 0;
    };

    QPointer<QWidget> m_target;
    QVector<Ripple> m_ripples;
    QPainterPath m_clipPath;
    QElapsedTimer m_clock;
    QTimer* m_timer = nullptr;
    int m_durationMs = 280;
    qreal m_baseOpacity = 0.18;
};

} // namespace QtMaterial