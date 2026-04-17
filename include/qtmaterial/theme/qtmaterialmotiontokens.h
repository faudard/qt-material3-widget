#pragma once
#include <QEasingCurve>
#include <QHash>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
namespace QtMaterial {
struct MotionStyle { int durationMs = 0; QEasingCurve easing = QEasingCurve::Linear; };
class QTMATERIAL3_THEME_EXPORT MotionTokens {
public:
    MotionTokens();
    ~MotionTokens();
    bool contains(MotionToken token) const;
    MotionStyle style(MotionToken token) const;
    void setStyle(MotionToken token, const MotionStyle& style);
private:
    QHash<MotionToken, MotionStyle> m_styles;
};
} // namespace QtMaterial
