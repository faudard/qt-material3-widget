#pragma once

#include <QColor>
#include <QIcon>
#include <QPixmap>
#include <QSize>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

enum class MaterialIcon
{
    Add,
    AddCircle,
    ArrowBackIos,
    ArrowForward,
    ArrowForwardIos,
    Cancel,
    CheckCircle,
    ChevronLeft,
    ChevronRight,
    Close,
    Error,
    Home,
    MoreVert,
    Refresh,
    Search,
    Settings,
    Warning
};

class QTMATERIAL3_CORE_EXPORT QtMaterialIcon
{
public:
    static QString resourcePath(MaterialIcon icon);

    static QPixmap pixmap(MaterialIcon icon,
                          const QColor& color,
                          const QSize& size = QSize(24, 24),
                          qreal devicePixelRatio = 0.0);

    static QIcon icon(MaterialIcon icon,
                      const QColor& color,
                      const QSize& size = QSize(24, 24),
                      qreal devicePixelRatio = 0.0);

    static QIcon icon(MaterialIcon icon,
                      const QColor& normalColor,
                      const QColor& disabledColor,
                      const QSize& size = QSize(24, 24),
                      qreal devicePixelRatio = 0.0);

    static void clearCache();
};

} // namespace QtMaterial
