#pragma once
#include <QByteArray>
#include <QList>
#include <QRect>
#include <QString>
namespace Studio {
struct Item { QString id; QString component; QRect rect; QString text; };
struct Document {
    QString name = QStringLiteral("Untitled");
    int width = 840;
    int height = 600;
    QString theme = QStringLiteral("light");
    QString density = QStringLiteral("standard");
    QString direction = QStringLiteral("ltr");
    QList<Item> items;
};
struct Component {
    QString id;
    QString widgetType;
    QString header;
    QString label;
    QString property;
};
QList<Component> components();
const Component* lookup(const QString& id);
QByteArray serialize(const Document& document);
bool parse(const QByteArray& input, Document* document, QString* error = nullptr);
bool load(const QString& file, Document* document, QString* error = nullptr);
bool save(const QString& file, const Document& document, QString* error = nullptr);
bool exportUi(const Document& document, const QString& file, QString* error = nullptr);
bool exportCpp(const Document& document, const QString& directory, QString* error = nullptr);
Document templateDocument(const QString& name);
}
