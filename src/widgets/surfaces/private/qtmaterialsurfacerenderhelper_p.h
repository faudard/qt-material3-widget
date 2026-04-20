#pragma once

#include <QMargins>
#include <QPainterPath>
#include <QRect>

#include "qtmaterial/theme/qtmaterialtheme.h"
#include "qtmaterial/specs/qtmaterialdialogspec.h"
#include "qtmaterial/specs/qtmaterialnavigationdrawerspec.h"
#include "qtmaterial/specs/qtmaterialbottomsheetspec.h"
#include "qtmaterial/specs/qtmaterialcardspec.h"

class QPainter;

namespace QtMaterial {

class QtMaterialSurfaceRenderHelper
{
public:
    struct SurfaceFrame {
        QRectF panelRect;
        qreal radius = 0.0;
        QColor containerColor;
        QColor outlineColor;
        QColor shadowColor;
        int shadowBlur = 0;
        int shadowYOffset = 0;
        bool drawOutline = false;
        bool drawShadow = true;
    };

    static SurfaceFrame dialogFrame(const QtMaterial::Theme& theme, const DialogSpec& spec, const QRect& bounds);
    static SurfaceFrame drawerFrame(const QtMaterial::Theme& theme, const NavigationDrawerSpec& spec, const QRect& bounds);
    static SurfaceFrame bottomSheetFrame(const QtMaterial::Theme& theme, const BottomSheetSpec& spec, const QRect& bounds);
    static SurfaceFrame cardFrame(const QtMaterial::Theme& theme, const CardSpec& spec, const QRect& bounds);

    static QPainterPath roundedPath(const QRectF& rect, qreal radius);
    static void paintFrame(QPainter* painter, const SurfaceFrame& frame);

private:
    static SurfaceFrame baseFrame(const QtMaterial::Theme& theme, ShapeRole shapeRole, ElevationRole elevationRole,
        const QRectF& rect, const QColor& containerColor, const QColor& outlineColor, bool drawOutline);
};

} // namespace QtMaterial
