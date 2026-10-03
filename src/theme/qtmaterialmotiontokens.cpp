#include "qtmaterial/theme/qtmaterialmotiontokens.h"

namespace QtMaterial {

MotionTokens::MotionTokens()
    : MotionTokens(MotionScheme::Standard)
{
}

MotionTokens::MotionTokens(MotionScheme scheme)
{
    const auto standard   = QEasingCurve(QEasingCurve::OutCubic);
    const auto emphasized = QEasingCurve(QEasingCurve::OutCubic);
    const auto exitCurve  = QEasingCurve(QEasingCurve::InCubic);

    setStyle(MotionToken::Short1,  MotionStyle{  50, standard   });
    setStyle(MotionToken::Short2,  MotionStyle{ 100, standard   });
    setStyle(MotionToken::Short3,  MotionStyle{ 150, standard   });
    setStyle(MotionToken::Short4,  MotionStyle{ 200, exitCurve  });

    setStyle(MotionToken::Medium1, MotionStyle{ 250, standard   });
    setStyle(MotionToken::Medium2, MotionStyle{ 300, emphasized });
    setStyle(MotionToken::Medium3, MotionStyle{ 350, emphasized });
    setStyle(MotionToken::Medium4, MotionStyle{ 400, emphasized });

    setStyle(MotionToken::Long1,   MotionStyle{ 450, standard   });
    setStyle(MotionToken::Long2,   MotionStyle{ 500, standard   });
    setStyle(MotionToken::Long3,   MotionStyle{ 550, standard   });
    setStyle(MotionToken::Long4,   MotionStyle{ 600, standard   });
    applyScheme(scheme);
}

MotionTokens::~MotionTokens() = default;

bool MotionTokens::contains(MotionToken token) const
{
    return m_styles.contains(token);
}

MotionStyle MotionTokens::style(MotionToken token) const
{
    return m_styles.value(token, MotionStyle{});
}

void MotionTokens::setStyle(MotionToken token, const MotionStyle& style)
{
    m_styles.insert(token, style);
}

void MotionTokens::applyScheme(MotionScheme scheme)
{
    // Duration/easing adaptation for Qt Widgets; effects never overshoot colors
    // or opacity. Legacy duration tokens retain their existing defaults.
    const bool expressive = scheme == MotionScheme::Expressive;
    const QEasingCurve spatial(expressive ? QEasingCurve::OutBack : QEasingCurve::OutCubic);
    const QEasingCurve effects(QEasingCurve::OutCubic);
    setStyle(MotionToken::SpatialFast, {expressive ? 240 : 160, spatial});
    setStyle(MotionToken::SpatialDefault, {expressive ? 380 : 300, spatial});
    setStyle(MotionToken::SpatialSlow, {expressive ? 500 : 450, spatial});
    setStyle(MotionToken::EffectsFast, {150, effects});
    setStyle(MotionToken::EffectsDefault, {250, effects});
    setStyle(MotionToken::EffectsSlow, {350, effects});
}

} // namespace QtMaterial
