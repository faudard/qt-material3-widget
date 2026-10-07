#pragma once

#include <QString>
#include <QVector>

struct GalleryComponentEntry
{
    QString id;
    QString name;
    QString family;
    QString route;
    QString widgetType;
    QString publicHeader;
};

const QVector<GalleryComponentEntry>& galleryComponentCatalog();
int galleryTabForRoute(const QString& route);
