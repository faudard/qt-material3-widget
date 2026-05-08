#pragma once

#include <memory>

#include <QColor>
#include <QPointer>
#include <QSize>
#include <QString>

#include "qtmaterial/core/qtmaterialasyncstate.h"
#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialprogressindicatorspec.h"

class QVariantAnimation;

namespace QtMaterial {

class QtMaterialCircularProgressIndicatorPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialCircularProgressIndicator : public QtMaterialWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal value READ value WRITE setValue RESET resetValue NOTIFY valueChanged)
    Q_PROPERTY(Mode mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(bool busy READ isBusy WRITE setBusy NOTIFY asyncStateChanged)
    Q_PROPERTY(bool indeterminate READ isIndeterminate WRITE setIndeterminate NOTIFY asyncStateChanged)
    Q_PROPERTY(QString statusText READ statusText WRITE setStatusText NOTIFY asyncStateChanged)
    Q_PROPERTY(QString accessibleValueText READ accessibleValueText NOTIFY accessibilitySummaryChanged)
    Q_PROPERTY(QString accessibilitySummary READ accessibilitySummary NOTIFY accessibilitySummaryChanged)
    Q_PROPERTY(QColor activeColor READ activeColor WRITE setActiveColor RESET resetActiveColor NOTIFY specChanged)
    Q_PROPERTY(QColor trackColor READ trackColor WRITE setTrackColor RESET resetTrackColor NOTIFY specChanged)
    Q_PROPERTY(int trackGap READ trackGap WRITE setTrackGap NOTIFY specChanged)
    Q_PROPERTY(int strokeWidth READ strokeWidth WRITE setStrokeWidth NOTIFY specChanged)

public:
    enum class Mode {
        Determinate,
        Indeterminate
    };
    Q_ENUM(Mode)

    explicit QtMaterialCircularProgressIndicator(QWidget* parent = nullptr);
    explicit QtMaterialCircularProgressIndicator(const ProgressIndicatorSpec& spec, QWidget* parent = nullptr);
    ~QtMaterialCircularProgressIndicator() override;

    qreal value() const noexcept;
    void setValue(qreal value);
    void resetValue();

    Mode mode() const noexcept;
    void setMode(Mode mode);

    bool isBusy() const noexcept;
    void setBusy(bool busy);

    bool isIndeterminate() const noexcept;
    void setIndeterminate(bool indeterminate);

    QString statusText() const;
    void setStatusText(const QString& text);

    QString accessibleValueText() const;
    QString accessibilitySummary() const;

    QtMaterialAsyncState asyncState() const;
    void setAsyncState(const QtMaterialAsyncState& state);

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
    void asyncStateChanged();
    void accessibilitySummaryChanged(const QString& summary);
    void specChanged();

protected:
    void paintEvent(QPaintEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    void changeEvent(QEvent* event) override;

private:
    void init();
    void initAnimation();
    void updateAnimationState();
    void syncAsyncStateFromProgress();
    void syncMaterialStateFromAsyncState();
    void syncAccessibleState();

    std::unique_ptr<QtMaterialCircularProgressIndicatorPrivate> d;
};

} // namespace QtMaterial
