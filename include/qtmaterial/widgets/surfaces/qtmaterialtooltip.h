#pragma once

#include <memory>

#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/qtmaterialglobal.h"

class QEvent;
class QPaintEvent;
class QWidget;

namespace QtMaterial {

class QtMaterialTooltipPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialTooltip : public QtMaterialWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(Placement placement READ placement WRITE setPlacement NOTIFY placementChanged)
    Q_PROPERTY(int showDelay READ showDelay WRITE setShowDelay NOTIFY showDelayChanged)
    Q_PROPERTY(bool tooltipVisible READ isTooltipVisible NOTIFY tooltipVisibleChanged)

public:
    enum class Placement {
        Auto,
        Above,
        Below,
        Left,
        Right
    };
    Q_ENUM(Placement)

    explicit QtMaterialTooltip(QWidget* parent = nullptr);
    ~QtMaterialTooltip() override;

    QString text() const;
    void setText(const QString& text);

    Placement placement() const noexcept;
    void setPlacement(Placement placement);

    int showDelay() const noexcept;
    void setShowDelay(int milliseconds);

    QWidget* targetWidget() const noexcept;
    void setTargetWidget(QWidget* target);

    bool isTooltipVisible() const noexcept;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void showTooltip();
    void hideTooltip();

Q_SIGNALS:
    void textChanged(const QString& text);
    void placementChanged(QtMaterial::QtMaterialTooltip::Placement placement);
    void showDelayChanged(int milliseconds);
    void tooltipVisibleChanged(bool visible);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;

private:
    void scheduleShow();
    void reposition();

    std::unique_ptr<QtMaterialTooltipPrivate> d_ptr;
};

} // namespace QtMaterial
