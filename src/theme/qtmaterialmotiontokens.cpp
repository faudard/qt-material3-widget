#include "qtmaterial/theme/qtmaterialmotiontokens.h"

namespace QtMaterial {
namespace { const MotionStyle kDefaultMotionStyle{}; }

MotionTokens::MotionTokens() = default;
MotionTokens::~MotionTokens() = default;

bool MotionTokens::contains(MotionToken token) const { return m_styles.contains(token); }
const MotionStyle& MotionTokens::style(MotionToken token) const {
    auto it = m_styles.constFind(token);
    return (it != m_styles.constEnd()) ? it.value() : kDefaultMotionStyle;
}
void MotionTokens::setStyle(MotionToken token, const MotionStyle& style) { m_styles.insert(token, style); }

} // namespace QtMaterial
