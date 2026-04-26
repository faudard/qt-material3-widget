#pragma once

#include <QColor>
#include <QEasingCurve>
#include <QtGlobal>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialmotiontokens.h"

namespace QtMaterial {

/** Tokenized keyboard-focus indicator. */
struct QTMATERIAL3_THEME_EXPORT FocusRingTokens {
    int width = 2;
    int offset = 2;
    int radiusAdjustment = 0;
    QColor color = QColor(QStringLiteral("#6750A4"));
    qreal opacity = 1.0;
};

/** Accessibility policy tokens resolved with the theme. */
struct QTMATERIAL3_THEME_EXPORT AccessibilityTokens {
    bool highContrast = false;
    bool reducedMotion = false;
    qreal minimumTextContrastRatio = 4.5;
    qreal minimumUiContrastRatio = 3.0;
    FocusRingTokens focusRing;
};

/** Interaction-state policy tokens shared by widgets/specs. */
struct QTMATERIAL3_THEME_EXPORT InteractionStateTokens {
    bool keyboardFocusVisible = true;
    bool strongFocusIndicators = true;
    bool hoverFeedbackEnabled = true;
    bool pressFeedbackEnabled = true;
    bool dragFeedbackEnabled = true;
};

qreal QTMATERIAL3_THEME_EXPORT relativeLuminance(const QColor& color);
qreal QTMATERIAL3_THEME_EXPORT contrastRatio(const QColor& foreground, const QColor& background);
bool QTMATERIAL3_THEME_EXPORT meetsContrastRatio(const QColor& foreground, const QColor& background, qreal minimumRatio);
void QTMATERIAL3_THEME_EXPORT applyReducedMotion(MotionTokens* motion, bool reducedMotion);

} // namespace QtMaterial
