#pragma once

#include <QColor>
#include <QSize>
#include <QWidget>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialDivider : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(int leadingInset READ leadingInset WRITE setLeadingInset NOTIFY leadingInsetChanged)
    Q_PROPERTY(int trailingInset READ trailingInset WRITE setTrailingInset NOTIFY trailingInsetChanged)
    Q_PROPERTY(int thickness READ thickness WRITE setThickness NOTIFY thicknessChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor RESET resetColor NOTIFY colorChanged)

public:
    explicit QtMaterialDivider(QWidget *parent = nullptr);
    explicit QtMaterialDivider(Qt::Orientation orientation, QWidget *parent = nullptr);
    ~QtMaterialDivider() override;

    static QtMaterialDivider *horizontal(QWidget *parent = nullptr);
    static QtMaterialDivider *vertical(QWidget *parent = nullptr);

    Qt::Orientation orientation() const noexcept;
    void setOrientation(Qt::Orientation orientation);

    int leadingInset() const noexcept;
    void setLeadingInset(int value);

    int trailingInset() const noexcept;
    void setTrailingInset(int value);

    int thickness() const noexcept;
    void setThickness(int value);

    QColor color() const noexcept;
    void setColor(const QColor &color);
    void resetColor();

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

Q_SIGNALS:
    void orientationChanged(Qt::Orientation orientation);
    void leadingInsetChanged(int value);
    void trailingInsetChanged(int value);
    void thicknessChanged(int value);
    void colorChanged(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    QColor resolvedColor() const;

    Qt::Orientation m_orientation = Qt::Horizontal;
    int m_leadingInset = 0;
    int m_trailingInset = 0;
    int m_thickness = 1;
    QColor m_color;
};

} // namespace QtMaterial
