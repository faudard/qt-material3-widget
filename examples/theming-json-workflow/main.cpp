#include <QCoreApplication>
#include <QColor>
#include <QFile>
#include <QJsonDocument>
#include <QTextStream>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemeserializer.h"

using namespace QtMaterial;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    QTextStream out(stdout);
    QTextStream err(stderr);

    ThemeOptions options;
    options.sourceColor = QColor(QStringLiteral("#6750A4"));
    options.mode = ThemeMode::Light;
    options.contrast = ContrastMode::Standard;

    ThemeBuilder builder;
    const Theme theme = builder.build(options);
    const QByteArray json = ThemeSerializer::toJson(theme, QJsonDocument::Indented);

    bool ok = false;
    QString errorString;
    const Theme restored = ThemeSerializer::fromJson(json, ThemeReadMode::Strict, &ok, &errorString);
    if (!ok) {
        err << "Strict theme read failed: " << errorString << Qt::endl;
        return 1;
    }

    const QString filePath = app.arguments().size() > 1
        ? app.arguments().at(1)
        : QStringLiteral("theme-v2.generated.json");

    if (!ThemeSerializer::writeToFile(restored, filePath, &errorString, QJsonDocument::Indented)) {
        err << "Could not write " << filePath << ": " << errorString << Qt::endl;
        return 2;
    }

    out << "Wrote strict-readable theme snapshot to " << filePath << Qt::endl;
    out << "Format version: " << ThemeSerializer::kCurrentFormatVersion << Qt::endl;
    out << "Seed color: " << restored.options().sourceColor.name(QColor::HexRgb).toUpper() << Qt::endl;
    return 0;
}
