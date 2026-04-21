#pragma once

#include <QObject>
#include <QEasingCurve>

#include "qtmaterial/qtmaterialglobal.h"

class QVariantAnimation;

namespace QtMaterial {

class QTMATERIAL3_EFFECTS_EXPORT QtMaterialTransitionController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal progress READ progress WRITE setProgress NOTIFY progressChanged)

public:
    explicit QtMaterialTransitionController(QObject* parent = nullptr);
    ~QtMaterialTransitionController() override;

    qreal progress() const noexcept;
    void setProgress(qreal value);

    void setDuration(int durationMs);
    int duration() const noexcept;

    void setEasingCurve(const QEasingCurve& curve);
    QEasingCurve easingCurve() const noexcept;

    void startForward();
    void startBackward();

    void startTo(qreal target);
signals:
    void progressChanged(qreal value);
    void finished();

private:

    qreal m_progress = 0.0;
    int m_durationMs = 180;
    QEasingCurve m_easing = QEasingCurve(QEasingCurve::OutCubic);
    QVariantAnimation* m_animation = nullptr;
};

} // namespace QtMaterial