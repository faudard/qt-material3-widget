#pragma once

#include <QColor>
#include <QPointer>
#include <QWidget>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialprogressindicatorspec.h"

class QVariantAnimation;

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialLinearProgressIndicator : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal value READ value WRITE setValue RESET resetValue NOTIFY valueChanged)
    Q_PROPERTY(Mode mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(bool invertedAppearance READ invertedAppearance WRITE setInvertedAppearance NOTIFY invertedAppearanceChanged)
    Q_PROPERTY(QColor activeColor READ activeColor WRITE setActiveColor RESET resetActiveColor NOTIFY specChanged)
    Q_PROPERTY(QColor trackColor READ trackColor WRITE setTrackColor RESET resetTrackColor NOTIFY specChanged)
    Q_PROPERTY(int trackGap READ trackGap WRITE setTrackGap NOTIFY specChanged)
    Q_PROPERTY(int stopIndicatorSize READ stopIndicatorSize WRITE setStopIndicatorSize NOTIFY specChanged)

public:
    enum class Mode { Determinate, Indeterminate };
    Q_ENUM(Mode)

    explicit QtMaterialLinearProgressIndicator(QWidget* parent = nullptr);
    explicit QtMaterialLinearProgressIndicator(const ProgressIndicatorSpec& spec, QWidget* parent = nullptr);
    ~QtMaterialLinearProgressIndicator() override;

    qreal value() const noexcept;
    void setValue(qreal value);
    void resetValue();

    Mode mode() const noexcept;
    void setMode(Mode mode);

    bool invertedAppearance() const noexcept;
    void setInvertedAppearance(bool inverted);

    QColor activeColor() const;
    void setActiveColor(const QColor& color);
    void resetActiveColor();

    QColor trackColor() const;
    void setTrackColor(const QColor& color);
    void resetTrackColor();

    int trackGap() const noexcept;
    void setTrackGap(int gap);

    int stopIndicatorSize() const noexcept;
    void setStopIndicatorSize(int size);

    ProgressIndicatorSpec spec() const;
    void setSpec(const ProgressIndicatorSpec& spec);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

Q_SIGNALS:
    void valueChanged(qreal value);
    void modeChanged(QtMaterial::QtMaterialLinearProgressIndicator::Mode mode);
    void invertedAppearanceChanged(bool invertedAppearance);
    void specChanged();

protected:
    void paintEvent(QPaintEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    void changeEvent(QEvent* event) override;

private:
    void initAnimation();
    void updateAnimationState();
    QColor resolvedActiveColor() const;
    QColor resolvedTrackColor() const;
    QColor resolvedStopColor() const;

    ProgressIndicatorSpec m_spec;
    qreal m_value = 0.0;
    qreal m_phase = 0.0;
    Mode m_mode = Mode::Determinate;
    bool m_invertedAppearance = false;
    QPointer<QVariantAnimation> m_animation;
};

} // namespace QtMaterial
