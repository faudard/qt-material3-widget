#include "qtmaterial/theme/qtmaterialmotiontokens.h"
namespace QtMaterial {
MotionTokens::MotionTokens() = default;
MotionTokens::~MotionTokens() = default;
bool MotionTokens::contains(MotionToken token) const { return m_styles.contains(token); }
MotionStyle MotionTokens::style(MotionToken token) const { return m_styles.value(token, MotionStyle{}); }
void MotionTokens::setStyle(MotionToken token, const MotionStyle& style) { m_styles.insert(token, style); }
} // namespace QtMaterial
