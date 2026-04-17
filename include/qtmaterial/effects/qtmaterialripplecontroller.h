#pragma once
#include <QPainterPath>
#include <QPointer>
#include <QVector>
#include <QObject>
#include <QPointF>
#include <QRectF>
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
    void addRipple(const QPointF& center);
    void clear();
    bool isActive() const noexcept;
    void paint(QPainter* painter, const QColor& color);

private:
    struct Ripple {
        QPointF center;
        qreal radius = 0.0;
        qreal opacity = 0.18;
    };

    QPointer<QWidget> m_target;
    QVector<Ripple> m_ripples;
    QPainterPath m_clipPath;
};
} // namespace QtMaterial
