#include "qtmaterial/theme/qtmaterialmotiontokens.h"

namespace QtMaterial {

MotionTokens::MotionTokens()
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

} // namespace QtMaterial