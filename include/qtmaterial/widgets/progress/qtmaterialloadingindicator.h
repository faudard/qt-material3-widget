#pragma once

#include <memory>

#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/qtmaterialglobal.h"

class QHideEvent;
class QPaintEvent;
class QShowEvent;

namespace QtMaterial {

class QtMaterialLoadingIndicatorPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialLoadingIndicator : public QtMaterialWidget
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(int indicatorSize READ indicatorSize WRITE setIndicatorSize NOTIFY indicatorSizeChanged)

public:
    explicit QtMaterialLoadingIndicator(QWidget* parent = nullptr);
    ~QtMaterialLoadingIndicator() override;

    bool isActive() const noexcept;
    void setActive(bool active);

    int indicatorSize() const noexcept;
    void setIndicatorSize(int size);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

Q_SIGNALS:
    void activeChanged(bool active);
    void indicatorSizeChanged(int size);

protected:
    void paintEvent(QPaintEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;

private:
    void updateAnimationState();

    std::unique_ptr<QtMaterialLoadingIndicatorPrivate> d;
};

} // namespace QtMaterial
