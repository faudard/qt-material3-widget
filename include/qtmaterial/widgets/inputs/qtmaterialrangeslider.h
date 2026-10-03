#pragma once

#include <memory>

#include <QWidget>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QtMaterialRangeSliderPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialRangeSlider : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(int maximum READ maximum WRITE setMaximum)
    Q_PROPERTY(int lowerValue READ lowerValue WRITE setLowerValue NOTIFY lowerValueChanged)
    Q_PROPERTY(int upperValue READ upperValue WRITE setUpperValue NOTIFY upperValueChanged)
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)

public:
    explicit QtMaterialRangeSlider(QWidget* parent = nullptr);
    ~QtMaterialRangeSlider() override;

    int minimum() const noexcept;
    int maximum() const noexcept;
    void setMinimum(int value);
    void setMaximum(int value);
    void setRange(int minimum, int maximum);

    int lowerValue() const noexcept;
    int upperValue() const noexcept;
    void setLowerValue(int value);
    void setUpperValue(int value);
    void setValues(int lower, int upper);

    Qt::Orientation orientation() const noexcept;
    void setOrientation(Qt::Orientation orientation);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void lowerValueChanged(int value);
    void upperValueChanged(int value);
    void valuesChanged(int lower, int upper);
    void rangeChanged(int minimum, int maximum);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    bool focusNextPrevChild(bool next) override;

private:
    enum class Handle { Lower, Upper };
    qreal normalizedForValue(int value) const noexcept;
    int valueForPosition(const QPoint& position) const noexcept;
    QPointF handleCenter(Handle handle) const noexcept;
    void moveActiveHandleTo(int value);

    friend class QtMaterialRangeSliderPrivate;
    std::unique_ptr<QtMaterialRangeSliderPrivate> d_ptr;
};

} // namespace QtMaterial
