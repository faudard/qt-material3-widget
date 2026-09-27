#include "qtmaterial/widgets/inputs/qtmaterialslider.h"

#include <QToolTip>

namespace QtMaterial {

class QtMaterialSliderPrivate final
{
public:
    bool valueLabelVisible = true;
};

QtMaterialSlider::QtMaterialSlider(Qt::Orientation orientation, QWidget* parent)
    : QSlider(orientation, parent)
    , d_ptr(std::make_unique<QtMaterialSliderPrivate>())
{
    setObjectName(QStringLiteral("qtmaterial_slider"));
    setFocusPolicy(Qt::StrongFocus);
    setTracking(true);
    setAccessibleName(tr("Slider"));

    connect(this, &QSlider::valueChanged, this, [this](int value) {
        if (d_ptr->valueLabelVisible && isSliderDown()) {
            QToolTip::showText(mapToGlobal(rect().center()), QString::number(value), this);
        }
        setAccessibleDescription(tr("Value %1").arg(value));
    });
}

QtMaterialSlider::~QtMaterialSlider() = default;

bool QtMaterialSlider::isValueLabelVisible() const noexcept { return d_ptr->valueLabelVisible; }

void QtMaterialSlider::setValueLabelVisible(bool visible)
{
    if (d_ptr->valueLabelVisible == visible) {
        return;
    }
    d_ptr->valueLabelVisible = visible;
    emit valueLabelVisibleChanged(visible);
}

} // namespace QtMaterial
