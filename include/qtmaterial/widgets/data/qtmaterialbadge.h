#pragma once

#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/qtmaterialglobal.h"

class QPaintEvent;

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialBadge : public QtMaterialWidget
{
    Q_OBJECT
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(int maximum READ maximum WRITE setMaximum NOTIFY maximumChanged)
    Q_PROPERTY(bool dot READ isDot WRITE setDot NOTIFY dotChanged)

public:
    explicit QtMaterialBadge(QWidget* parent = nullptr);
    ~QtMaterialBadge() override;

    int count() const noexcept;
    void setCount(int count);

    int maximum() const noexcept;
    void setMaximum(int maximum);

    bool isDot() const noexcept;
    void setDot(bool dot);

    QString displayText() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

Q_SIGNALS:
    void countChanged(int count);
    void maximumChanged(int maximum);
    void dotChanged(bool dot);

protected:
    void paintEvent(QPaintEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;

private:
    void syncAccessibility();

    int m_count = 0;
    int m_maximum = 999;
    bool m_dot = false;
};

} // namespace QtMaterial
