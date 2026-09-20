#pragma once

#include <QObject>
#include <QEasingCurve>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialmotiontokens.h"

class QVariantAnimation;

namespace QtMaterial {
class Theme;

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

    void setReducedMotion(bool reducedMotion);
    bool reducedMotion() const noexcept;

    bool isRunning() const noexcept;
    void stop();
    void finish();

    void startForward();
    void startBackward();
    void startTo(qreal target);

    void applyMotionStyle(const MotionStyle& style);
    void applyMotionToken(const Theme& theme, MotionToken token);

signals:
    void progressChanged(qreal value);
    void finished();

private:
    friend class QtMaterialTransitionControllerCaptureAccess;

    qreal m_progress = 0.0;
    qreal m_targetProgress = 0.0;
    int m_durationMs = 180;
    QEasingCurve m_easing = QEasingCurve(QEasingCurve::OutCubic);
    QVariantAnimation* m_animation = nullptr;
    bool m_reducedMotion = false;
};

} // namespace QtMaterial
