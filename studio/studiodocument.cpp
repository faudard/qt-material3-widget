#include "studiodocument.h"
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QRegularExpression>
#include <QSaveFile>
#include <QSet>
#include <QTextStream>
namespace Studio {
namespace {
const QList<Component>& whitelist()
{
    static const QList<Component> entries = {
        {"button.filled", "QtMaterialFilledButton", "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h", "Filled Button", "text"},
        {"button.outlined", "QtMaterialOutlinedButton", "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h", "Outlined Button", "text"},
        {"button.filled-tonal", "QtMaterialFilledTonalButton", "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h", "Tonal Button", "text"},
        {"selection.checkbox", "QtMaterialCheckbox", "qtmaterial/widgets/selection/qtmaterialcheckbox.h", "Checkbox", "text"},
        {"selection.radio", "QtMaterialRadioButton", "qtmaterial/widgets/selection/qtmaterialradiobutton.h", "Radio Button", "text"},
        {"input.text.outlined", "QtMaterialOutlinedTextField", "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h", "Outlined Text Field", "placeholderText"},
        {"surface.card", "QtMaterialCard", "qtmaterial/widgets/surfaces/qtmaterialcard.h", "Card", "titleText"}
    };
    return entries;
}
bool fail(QString* error, const QString& why) { if (error) *error = why; return false; }
bool only(const QJsonObject& object, const QSet<QString>& keys)
{
    for (auto i = object.constBegin(); i != object.constEnd(); ++i)
        if (!keys.contains(i.key())) return false;
    return true;
}
bool asInt(const QJsonValue& value, int min, int max, int* out)
{
    if (!value.isDouble()) return false;
    const double v = value.toDouble();
    if (v < min || v > max || v != static_cast<int>(v)) return false;
    *out = int(v);
    return true;
}
bool safeString(const QString& text)
{
    if (text.size() > 4096) return false;
    for (QChar c : text) if (c.unicode() < 32 && c != QLatin1Char('\t')
        && c != QLatin1Char('\r') && c != QLatin1Char('\n')) return false;
    return true;
}
QString xml(QString value)
{
    value.replace("&", "&amp;"); value.replace("<", "&lt;");
    value.replace(">", "&gt;"); value.replace("\"", "&quot;");
    value.replace("'", "&apos;");
    return value;
}
bool writeFile(const QString& path, const QByteArray& bytes, QString* error)
{
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)) return fail(error, file.errorString());
    if (file.write(bytes) != bytes.size()) return fail(error, file.errorString());
    if (!file.commit()) return fail(error, file.errorString());
    return true;
}
}
QList<Component> components() { return whitelist(); }
const Component* lookup(const QString& id)
{
    for (const Component& c : whitelist()) if (c.id == id) return &c;
    return nullptr;
}
QByteArray serialize(const Document& d)
{
    QJsonArray items;
    for (const Item& i : d.items) {
        items.append(QJsonObject{
            {"id", i.id}, {"component", i.component},
            {"rect", QJsonObject{{"x", i.rect.x()}, {"y", i.rect.y()},
                                 {"width", i.rect.width()}, {"height", i.rect.height()}}},
            {"properties", QJsonObject{{"text", i.text}}}
        });
    }
    return QJsonDocument(QJsonObject{
        {"schemaVersion", 1}, {"name", d.name},
        {"canvas", QJsonObject{{"width", d.width}, {"height", d.height}}},
        {"appearance", QJsonObject{{"theme", d.theme}, {"density", d.density},
                                    {"direction", d.direction}}},
        {"elements", items}
    }).toJson(QJsonDocument::Indented);
}
bool parse(const QByteArray& input, Document* result, QString* error)
{
    if (!result) return fail(error, "Null document");
    if (input.size() > 2 * 1024 * 1024) return fail(error, "Document exceeds 2 MiB");
    QJsonParseError jsonError;
    QJsonDocument json = QJsonDocument::fromJson(input, &jsonError);
    if (!json.isObject() || jsonError.error != QJsonParseError::NoError)
        return fail(error, "Invalid JSON: " + jsonError.errorString());
    QJsonObject root = json.object();
    if (!only(root, {"schemaVersion", "name", "canvas", "appearance", "elements"}))
        return fail(error, "Unknown root field");
    int version = 0;
    if (!asInt(root.value("schemaVersion"), 1, 1, &version))
        return fail(error, "Unsupported schema version");
    if (!root.value("name").isString() || !safeString(root.value("name").toString()))
        return fail(error, "Invalid document name");
    if (!root.value("canvas").isObject() || !root.value("appearance").isObject()
        || !root.value("elements").isArray()) return fail(error, "Missing canvas, appearance or elements");
    Document d;
    d.name = root.value("name").toString();
    QJsonObject size = root.value("canvas").toObject();
    if (!only(size, {"width", "height"}) ||
        !asInt(size.value("width"), 240, 4096, &d.width) ||
        !asInt(size.value("height"), 240, 4096, &d.height))
        return fail(error, "Invalid canvas");
    QJsonObject appearance = root.value("appearance").toObject();
    if (!only(appearance, {"theme", "density", "direction"}) ||
        !appearance.value("theme").isString() ||
        !appearance.value("density").isString() ||
        !appearance.value("direction").isString())
        return fail(error, "Invalid appearance fields");
    d.theme = appearance.value("theme").toString();
    d.density = appearance.value("density").toString();
    d.direction = appearance.value("direction").toString();
    if ((d.theme != "light" && d.theme != "dark") ||
        (d.density != "standard" && d.density != "compact") ||
        (d.direction != "ltr" && d.direction != "rtl"))
        return fail(error, "Unsupported appearance option");
    const QJsonArray elements = root.value("elements").toArray();
    if (elements.size() > 512) return fail(error, "More than 512 elements");
    const QRegularExpression idRule("^[A-Za-z_][A-Za-z_0-9]{0,63}$");
    QSet<QString> ids;
    for (int n = 0; n < elements.size(); ++n) {
        if (!elements[n].isObject()) return fail(error, QString("Element %1 is not an object").arg(n));
        QJsonObject e = elements[n].toObject();
        if (!only(e, {"id", "component", "rect", "properties"}) ||
            !e.value("id").isString() || !e.value("component").isString() ||
            !e.value("rect").isObject() || !e.value("properties").isObject())
            return fail(error, QString("Invalid element %1 fields").arg(n));
        Item item;
        item.id = e.value("id").toString();
        item.component = e.value("component").toString();
        if (!idRule.match(item.id).hasMatch() || ids.contains(item.id) ||
            !lookup(item.component))
            return fail(error, QString("Invalid id or unknown component at %1").arg(n));
        QJsonObject r = e.value("rect").toObject();
        QJsonObject p = e.value("properties").toObject();
        if (!only(r, {"x", "y", "width", "height"}) || !only(p, {"text"}) ||
            !p.value("text").isString()) return fail(error, QString("Unknown property at %1").arg(n));
        item.text = p.value("text").toString();
        int x, y, w, h;
        if (!safeString(item.text) ||
            !asInt(r.value("x"), 0, d.width, &x) ||
            !asInt(r.value("y"), 0, d.height, &y) ||
            !asInt(r.value("width"), 24, d.width, &w) ||
            !asInt(r.value("height"), 24, d.height, &h) ||
            x + w > d.width || y + h > d.height)
            return fail(error, QString("Invalid bounds/text at %1").arg(n));
        item.rect = QRect(x, y, w, h);
        ids.insert(item.id);
        d.items.append(item);
    }
    *result = d; // Fail-closed and transactional: do not modify on invalid input.
    if (error) error->clear();
    return true;
}
bool load(const QString& path, Document* document, QString* error)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) return fail(error, f.errorString());
    if (f.size() > 2 * 1024 * 1024) return fail(error, "Document exceeds 2 MiB");
    return parse(f.readAll(), document, error);
}
bool save(const QString& path, const Document& d, QString* error)
{
    Document check;
    if (!parse(serialize(d), &check, error)) return false;
    return writeFile(path, serialize(d), error);
}
bool exportUi(const Document& d, const QString& path, QString* error)
{
    Document check;
    if (!parse(serialize(d), &check, error)) return false;
    QByteArray out;
    QTextStream s(&out, QIODevice::WriteOnly);
    s << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      << "<ui version=\"4.0\"><class>StudioForm</class>\n"
      << " <widget class=\"QWidget\" name=\"StudioForm\">\n"
      << "  <property name=\"geometry\"><rect><x>0</x><y>0</y><width>"
      << d.width << "</width><height>" << d.height << "</height></rect></property>\n"
      << "  <property name=\"layoutDirection\"><enum>Qt::"
      << (d.direction == "rtl" ? "RightToLeft" : "LeftToRight") << "</enum></property>\n";
    for (const Item& item : d.items) {
        const Component* c = lookup(item.component);
        s << "  <widget class=\"QtMaterial::" << c->widgetType << "\" name=\"" << item.id << "\">\n"
          << "   <property name=\"geometry\"><rect><x>" << item.rect.x() << "</x><y>"
          << item.rect.y() << "</y><width>" << item.rect.width() << "</width><height>"
          << item.rect.height() << "</height></rect></property>\n"
          << "   <property name=\"" << c->property << "\"><string>" << xml(item.text)
          << "</string></property>\n  </widget>\n";
    }
    QSet<QString> used;
    for (const Item& i : d.items) used.insert(i.component);
    s << " </widget>\n <customwidgets>\n";
    for (const Component& c : whitelist()) {
        if (!used.contains(c.id)) continue;
        s << "  <customwidget><class>QtMaterial::" << c.widgetType << "</class><extends>"
          << "QWidget"
          << "</extends><header location=\"global\">" << c.header
          << "</header></customwidget>\n";
    }
    s << " </customwidgets>\n <resources/>\n <connections/>\n</ui>\n";
    s.flush();
    return writeFile(path, out, error);
}
bool exportCpp(const Document& d, const QString& dir, QString* error)
{
    Document check;
    if (!parse(serialize(d), &check, error)) return false;
    if (!QDir().mkpath(dir)) return fail(error, "Cannot create output directory");
    QByteArray out;
    QTextStream s(&out, QIODevice::WriteOnly);
    s << "#include <QApplication>\n#include <QByteArray>\n#include <QWidget>\n#include <QTimer>\n";
    QSet<QString> used;
    for (const Item& i : d.items) used.insert(i.component);
    for (const Component& c : whitelist())
        if (used.contains(c.id)) s << "#include <" << c.header << ">\n";
    s << "int main(int argc, char** argv) {\n QApplication app(argc, argv);\n"
      << " QWidget window;\n window.resize(" << d.width << ", " << d.height << ");\n"
      << " window.setLayoutDirection(Qt::"
      << (d.direction == "rtl" ? "RightToLeft" : "LeftToRight") << ");\n";
    for (const Item& i : d.items) {
        const Component* c = lookup(i.component);
        const QString setter = c->property == "titleText" ? "setTitleText"
            : c->property == "placeholderText" ? "setPlaceholderText" : "setText";
        s << " auto* " << i.id << " = new QtMaterial::" << c->widgetType << "(&window);\n"
          << " " << i.id << "->setObjectName(QStringLiteral(\"" << i.id << "\"));\n"
          << " " << i.id << "->setGeometry(" << i.rect.x() << "," << i.rect.y()
          << "," << i.rect.width() << "," << i.rect.height() << ");\n"
          << " " << i.id << "->" << setter
          << "(QString::fromUtf8(QByteArray::fromHex(\"" << i.text.toUtf8().toHex()
          << "\")));\n";
    }
    s << " window.show();\n"
      << " if (qEnvironmentVariableIsSet(\"QTMATERIAL3_STUDIO_SMOKE\"))\n"
      << "  QTimer::singleShot(0, &app, &QCoreApplication::quit);\n"
      << " return app.exec();\n}\n";
    s.flush();
    const QByteArray cmake = R"cmake(cmake_minimum_required(VERSION 3.21)
project(studio_generated LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(QtMaterial3Widgets REQUIRED CONFIG)
add_executable(studio_generated main.cpp)
target_link_libraries(studio_generated PRIVATE Qt${QT_VERSION_MAJOR}::Widgets QtMaterial3::Widgets)
)cmake";
    if (!writeFile(QDir(dir).filePath("main.cpp"), out, error)) return false;
    return writeFile(QDir(dir).filePath("CMakeLists.txt"), cmake, error);
}
Document templateDocument(const QString& name)
{
    Document d;
    d.name = name;
    if (name == "dashboard") {
        d.items = {{"overview", "surface.card", QRect(30, 30, 340, 180), "Overview"},
                   {"activity", "surface.card", QRect(400, 30, 340, 180), "Activity"},
                   {"refresh", "button.filled", QRect(30, 240, 150, 48), "Refresh"}};
    } else if (name == "settings") {
        d.items = {{"notifications", "selection.checkbox", QRect(40, 50, 300, 50), "Notifications"},
                   {"privacy", "selection.checkbox", QRect(40, 120, 300, 50), "Privacy"},
                   {"apply", "button.filled-tonal", QRect(40, 210, 150, 48), "Apply"}};
    } else {
        d.name = "form";
        d.items = {{"email", "input.text.outlined", QRect(40, 50, 360, 60), "Email"},
                   {"accept", "selection.checkbox", QRect(40, 138, 360, 50), "I agree"},
                   {"submit", "button.filled", QRect(40, 220, 180, 48), "Submit"}};
    }
    return d;
}
} // namespace Studio
