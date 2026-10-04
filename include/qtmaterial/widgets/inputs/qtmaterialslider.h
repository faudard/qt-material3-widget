#pragma once

#include <memory>

#include <QSlider>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QtMaterialSliderPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialSlider : public QSlider
{
    Q_OBJECT
    Q_PROPERTY(bool valueLabelVisible READ isValueLabelVisible WRITE setValueLabelVisible NOTIFY valueLabelVisibleChanged)

public:
    // Supports Qt Designer/uic's standard parent-only widget construction.
    explicit QtMaterialSlider(QWidget* parent);
    explicit QtMaterialSlider(Qt::Orientation orientation = Qt::Horizontal, QWidget* parent = nullptr);
    ~QtMaterialSlider() override;

    bool isValueLabelVisible() const noexcept;
    void setValueLabelVisible(bool visible);

signals:
    void valueLabelVisibleChanged(bool visible);

private:
    std::unique_ptr<QtMaterialSliderPrivate> d_ptr;
};

} // namespace QtMaterial
