#pragma once

#include <QSlider>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialSlider : public QSlider
{
    Q_OBJECT
    Q_PROPERTY(bool valueLabelVisible READ isValueLabelVisible WRITE setValueLabelVisible NOTIFY valueLabelVisibleChanged)

public:
    explicit QtMaterialSlider(Qt::Orientation orientation = Qt::Horizontal, QWidget* parent = nullptr);
    ~QtMaterialSlider() override;

    bool isValueLabelVisible() const noexcept;
    void setValueLabelVisible(bool visible);

signals:
    void valueLabelVisibleChanged(bool visible);

private:
    bool m_valueLabelVisible = true;
};

} // namespace QtMaterial
