#include "qtmaterial/theme/qtmaterialaccessibilitytokens.h"

#include <array>
#include <cmath>

namespace QtMaterial {
namespace {

qreal linearizedChannel(qreal channel) {
    channel = qBound<qreal>(0.0, channel, 1.0);
    if (channel <= 0.03928) {
        return channel / 12.92;
    }
    return std::pow((channel + 0.055) / 1.055, 2.4);
}

constexpr std::array<MotionToken, 12> kAllMotionTokens = {
    MotionToken::Short1,
    MotionToken::Short2,
    MotionToken::Short3,
    MotionToken::Short4,
    MotionToken::Medium1,
    MotionToken::Medium2,
    MotionToken::Medium3,
    MotionToken::Medium4,
    MotionToken::Long1,
    MotionToken::Long2,
    MotionToken::Long3,
    MotionToken::Long4
};

} // namespace

qreal relativeLuminance(const QColor& color) {
    const qreal r = linearizedChannel(color.redF());
    const qreal g = linearizedChannel(color.greenF());
    const qreal b = linearizedChannel(color.blueF());
    return 0.2126 * r + 0.7152 * g + 0.0722 * b;
}

qreal contrastRatio(const QColor& foreground, const QColor& background) {
    const qreal l1 = relativeLuminance(foreground);
    const qreal l2 = relativeLuminance(background);
    const qreal lighter = qMax(l1, l2);
    const qreal darker = qMin(l1, l2);
    return (lighter + 0.05) / (darker + 0.05);
}

bool meetsContrastRatio(const QColor& foreground, const QColor& background, qreal minimumRatio) {
    return contrastRatio(foreground, background) + 0.0001 >= minimumRatio;
}

void applyReducedMotion(MotionTokens* motion, bool reducedMotion) {
    if (!motion || !reducedMotion) {
        return;
    }

    for (MotionToken token : kAllMotionTokens) {
        if (!motion->contains(token)) {
            continue;
        }
        MotionStyle style = motion->style(token);
        style.durationMs = 0;
        style.easing = QEasingCurve(QEasingCurve::Linear);
        motion->setStyle(token, style);
    }
}

} // namespace QtMaterial
