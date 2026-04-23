#pragma once

#include <QColor>
#include <QPointer>
#include <QWidget>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialprogressindicatorspec.h"

class QVariantAnimation;

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialCircularProgressIndicator : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal value READ value WRITE setValue RESET resetValue NOTIFY valueChanged)
    Q_PROPERTY(Mode mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(QColor activeColor READ activeColor WRITE setActiveColor RESET resetActiveColor NOTIFY specChanged)
    Q_PROPERTY(QColor trackColor READ trackColor WRITE setTrackColor RESET resetTrackColor NOTIFY specChanged)
    Q_PROPERTY(int trackGap READ trackGap WRITE setTrackGap NOTIFY specChanged)
    Q_PROPERTY(int strokeWidth READ strokeWidth WRITE setStrokeWidth NOTIFY specChanged)

public:
    enum class Mode { Determinate, Indeterminate };
    Q_ENUM(Mode)

    explicit QtMaterialCircularProgressIndicator(QWidget* parent = nullptr);
    explicit QtMaterialCircularProgressIndicator(const ProgressIndicatorSpec& spec, QWidget* parent = nullptr);
    ~QtMaterialCircularProgressIndicator() override;

    qreal value() const noexcept;
    void setValue(qreal value);
    void resetValue();

    Mode mode() const noexcept;
    void setMode(Mode mode);

    QColor activeColor() const;
    void setActiveColor(const QColor& color);
    void resetActiveColor();

    QColor trackColor() const;
    void setTrackColor(const QColor& color);
    void resetTrackColor();

    int trackGap() const noexcept;
    void setTrackGap(int gap);

    int strokeWidth() const noexcept;
    void setStrokeWidth(int width);

    ProgressIndicatorSpec spec() const;
    void setSpec(const ProgressIndicatorSpec& spec);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

Q_SIGNALS:
    void valueChanged(qreal value);
    void modeChanged(QtMaterial::QtMaterialCircularProgressIndicator::Mode mode);
    void specChanged();

protected:
    void paintEvent(QPaintEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    void changeEvent(QEvent* event) override;

private Q_SLOTS:
    void updateAccessibility();

private:
    void initAnimation();
    void updateAnimationState();
    QColor resolvedActiveColor() const;
    QColor resolvedTrackColor() const;
    QColor disabledColor(const QColor& source) const;

    ProgressIndicatorSpec m_spec;
    qreal m_value = 0.0;
    qreal m_phase = 0.0;
    Mode m_mode = Mode::Determinate;
    QPointer<QVariantAnimation> m_animation;
};

} // namespace QtMaterial
