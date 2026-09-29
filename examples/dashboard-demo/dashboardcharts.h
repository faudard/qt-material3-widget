#pragma once

#include <QColor>
#include <QVector>
#include <QWidget>

class LineChartWidget final : public QWidget
{
    Q_OBJECT
public:
    explicit LineChartWidget(QWidget* parent = nullptr);

    void setValues(const QVector<qreal>& values);
    void setAccentColor(const QColor& color);
    void clearAccentColor();

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QVector<qreal> m_values;
    QColor m_accentColor;
};

class DonutChartWidget final : public QWidget
{
    Q_OBJECT
public:
    explicit DonutChartWidget(QWidget* parent = nullptr);

    void setValue(int value);
    int value() const noexcept;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int m_value = 67;
};
