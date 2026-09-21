#include "qtmaterial/widgets/inputs/qtmaterialslider.h"

#include <QToolTip>

namespace QtMaterial {

QtMaterialSlider::QtMaterialSlider(Qt::Orientation orientation, QWidget* parent)
    : QSlider(orientation, parent)
{
    setObjectName(QStringLiteral("qtmaterial_slider"));
    setFocusPolicy(Qt::StrongFocus);
    setTracking(true);
    setAccessibleName(tr("Slider"));

    connect(this, &QSlider::valueChanged, this, [this](int value) {
        if (m_valueLabelVisible && isSliderDown()) {
            QToolTip::showText(mapToGlobal(rect().center()), QString::number(value), this);
        }
        setAccessibleDescription(tr("Value %1").arg(value));
    });
}

QtMaterialSlider::~QtMaterialSlider() = default;

bool QtMaterialSlider::isValueLabelVisible() const noexcept { return m_valueLabelVisible; }

void QtMaterialSlider::setValueLabelVisible(bool visible)
{
    if (m_valueLabelVisible == visible) {
        return;
    }
    m_valueLabelVisible = visible;
    emit valueLabelVisibleChanged(visible);
}

} // namespace QtMaterial
