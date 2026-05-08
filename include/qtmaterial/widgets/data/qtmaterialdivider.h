#pragma once

#include <memory>

#include <QColor>
#include <QRect>
#include <QSize>
#include <QString>
#include <QWidget>

#include "qtmaterial/qtmaterialglobal.h"

class QEvent;

namespace QtMaterial {

class QtMaterialDividerPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialDivider : public QWidget {
    Q_OBJECT

    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(int leadingInset READ leadingInset WRITE setLeadingInset NOTIFY leadingInsetChanged)
    Q_PROPERTY(int trailingInset READ trailingInset WRITE setTrailingInset NOTIFY trailingInsetChanged)
    Q_PROPERTY(int thickness READ thickness WRITE setThickness NOTIFY thicknessChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor RESET resetColor NOTIFY colorChanged)
    Q_PROPERTY(bool decorative READ isDecorative WRITE setDecorative NOTIFY decorativeChanged)
    Q_PROPERTY(QString accessibilityLabel READ accessibilityLabel WRITE setAccessibilityLabel NOTIFY accessibilityLabelChanged)

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

    bool isDecorative() const noexcept;
    void setDecorative(bool decorative);

    QString accessibilityLabel() const;
    void setAccessibilityLabel(const QString &label);

    QString accessibilitySummary() const;
    QRect lineRect() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

Q_SIGNALS:
    void orientationChanged(Qt::Orientation orientation);
    void leadingInsetChanged(int value);
    void trailingInsetChanged(int value);
    void thicknessChanged(int value);
    void colorChanged(const QColor &color);
    void decorativeChanged(bool decorative);
    void accessibilityLabelChanged(const QString &label);
    void accessibilitySummaryChanged(const QString &summary);

protected:
    void paintEvent(QPaintEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    std::unique_ptr<QtMaterialDividerPrivate> d_ptr;
    QColor resolvedColor() const;
    void syncAccessibility();

};

} // namespace QtMaterial
