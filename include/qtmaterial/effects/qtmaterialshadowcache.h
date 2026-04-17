#pragma once
#include <QPixmap>
#include <QString>
#include "qtmaterial/qtmaterialglobal.h"
namespace QtMaterial {
class QTMATERIAL3_EFFECTS_EXPORT QtMaterialShadowCache
{
public:
    QtMaterialShadowCache();
    ~QtMaterialShadowCache();
    QString keyFor(int width, int height, qreal radius, int blurRadius, int yOffset, const QColor& color) const;
    bool find(const QString& key, QPixmap* out) const;
    void insert(const QString& key, const QPixmap& pixmap) const;
};
} // namespace QtMaterial
