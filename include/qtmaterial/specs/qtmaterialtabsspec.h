#pragma once

#include <QColor>
#include <QMargins>
#include <QSize>
#include <QMetaType>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

enum class TabsVariant { Primary, Secondary };
enum class TabsDensity { Default, Compact };
enum class TabsAlignment { Start, Center, Stretch };
enum class TabsOverflowMode { ScrollButtons, Menu, ScrollButtonsAndMenu };

struct QTMATERIAL3_SPECS_EXPORT TabsSpec {
    TabsVariant variant = TabsVariant::Primary;
    TabsDensity density = TabsDensity::Default;
    TabsAlignment alignment = TabsAlignment::Start;
    TabsOverflowMode overflowMode = TabsOverflowMode::ScrollButtonsAndMenu;

    QColor containerColor;
    QColor activeLabelColor;
    QColor inactiveLabelColor;
    QColor activeIndicatorColor;
    QColor stateLayerColor;
    QColor hoverStateLayerColor;
    QColor focusedStateLayerColor;
    QColor pressedStateLayerColor;
    QColor focusRingColor;
    QColor disabledLabelColor;
    QColor badgeColor;
    QColor badgeLabelColor;
    QColor overflowButtonColor;

    qreal hoverOpacity = -1.0;
    qreal pressedOpacity = -1.0;
    qreal focusOpacity = -1.0;
    qreal disabledOpacity = 0.38;

    QMargins tabPadding = QMargins(16, 0, 16, 0);
    QSize iconSize = QSize(18, 18);
    int containerHeight = 48;
    int indicatorHeight = 3;
    int minimumTabWidth = 64;
    int maximumTabWidth = 360;
    int badgeDiameter = 18;
    int badgeDotDiameter = 6;
    int animationDuration = 180;
    bool scrollable = true;
    bool useGlobalTheme = true;
    bool wrapNavigation = true;
};

} // namespace QtMaterial

Q_DECLARE_METATYPE(QtMaterial::TabsVariant)
Q_DECLARE_METATYPE(QtMaterial::TabsDensity)
Q_DECLARE_METATYPE(QtMaterial::TabsAlignment)
Q_DECLARE_METATYPE(QtMaterial::TabsOverflowMode)
