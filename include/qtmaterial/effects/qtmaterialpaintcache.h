#pragma once

#include <QPixmap>
#include <QString>
#include <QSize>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QTMATERIAL3_EFFECTS_EXPORT QtMaterialPaintCache {
public:
    static QString makeKey(const QString& category, const QSize& size, qreal devicePixelRatio, quint64 revision = 0);
    static bool find(const QString& key, QPixmap* pixmap);
    static void insert(const QString& key, const QPixmap& pixmap);
    static void clear();
};

} // namespace QtMaterial
