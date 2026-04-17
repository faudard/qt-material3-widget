#include "qtmaterial/theme/qtmaterialstatelayer.h"
namespace QtMaterial {
StateLayer::StateLayer() : color(Qt::black), hoverOpacity(0.08), focusOpacity(0.10), pressOpacity(0.10), dragOpacity(0.16) {}
StateLayer::~StateLayer() = default;
} // namespace QtMaterial
