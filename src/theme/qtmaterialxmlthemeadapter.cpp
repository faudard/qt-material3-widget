#include "qtmaterial/theme/qtmaterialxmlthemeadapter.h"

#include <QFile>
#include <QHash>
#include <QSaveFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "qtmaterial/theme/qtmaterialcolorscheme.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

namespace QtMaterial {

namespace {

static bool isLikelyDarkTheme(const QColor& c)
{
    return c.isValid() && c.lightnessF() < 0.5;
}

static QString toHex(const QColor& c)
{
    return c.name(QColor::HexRgb).toUpper();
}

static bool fail(bool* ok, QString* errorString, const QString& error)
{
    if (ok) {
        *ok = false;
    }
    if (errorString) {
        *errorString = error;
    }
    return false;
}

} // namespace

Theme XmlThemeAdapter::fromQtMaterialXml(
    const QByteArray& xml,
    bool* ok,
    QString* errorString)
{
    QHash<QString, QColor> colors;

    QXmlStreamReader reader(xml);
    while (!reader.atEnd()) {
        reader.readNext();

        if (reader.isStartElement() && reader.name() == u"color") {
            const auto attrs = reader.attributes();
            const QString name = attrs.value(u"name").toString().trimmed();
            const QString value = reader.readElementText().trimmed();
            const QColor color(value);
            if (!name.isEmpty() && color.isValid()) {
                colors.insert(name, color);
            }
        }
    }

    if (reader.hasError()) {
        fail(ok, errorString, reader.errorString());
        return Theme();
    }

    if (!colors.contains(QStringLiteral("primaryColor"))
        || !colors.contains(QStringLiteral("secondaryColor"))
        || !colors.contains(QStringLiteral("primaryTextColor"))
        || !colors.contains(QStringLiteral("secondaryTextColor"))) {
        fail(ok,
             errorString,
             QStringLiteral("Missing required qt-material XML colors."));
        return Theme();
    }

    ThemeOptions options;
    options.sourceColor = colors.value(QStringLiteral("primaryColor"));
    options.mode = isLikelyDarkTheme(colors.value(QStringLiteral("secondaryColor")))
        ? ThemeMode::Dark
        : ThemeMode::Light;

    ThemeBuilder builder;
    Theme theme = builder.build(options);

    auto& scheme = theme.colorScheme();
    scheme.setColor(ColorRole::Primary, colors.value(QStringLiteral("primaryColor")));
    scheme.setColor(ColorRole::OnPrimary, colors.value(QStringLiteral("primaryTextColor")));
    scheme.setColor(ColorRole::Surface, colors.value(QStringLiteral("secondaryColor")));
    scheme.setColor(ColorRole::OnSurface, colors.value(QStringLiteral("secondaryTextColor")));
    scheme.setColor(ColorRole::OnSurfaceVariant, colors.value(QStringLiteral("secondaryTextColor")));

    if (colors.contains(QStringLiteral("primaryLightColor"))) {
        scheme.setColor(ColorRole::PrimaryContainer,
                        colors.value(QStringLiteral("primaryLightColor")));
    }
    if (colors.contains(QStringLiteral("secondaryLightColor"))) {
        scheme.setColor(ColorRole::SurfaceContainerHigh,
                        colors.value(QStringLiteral("secondaryLightColor")));
    }
    if (colors.contains(QStringLiteral("secondaryDarkColor"))) {
        scheme.setColor(ColorRole::SurfaceVariant,
                        colors.value(QStringLiteral("secondaryDarkColor")));
    }

    theme.setOptions(options);

    if (ok) {
        *ok = true;
    }
    return theme;
}

bool XmlThemeAdapter::readQtMaterialXmlFile(
    const QString& filePath,
    Theme* outTheme,
    QString* errorString)
{
    if (!outTheme) {
        return fail(nullptr,
                    errorString,
                    QStringLiteral("Output theme pointer is null."));
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return fail(nullptr, errorString, file.errorString());
    }

    bool ok = false;
    Theme imported = fromQtMaterialXml(file.readAll(), &ok, errorString);
    if (!ok) {
        return false;
    }

    *outTheme = imported;
    return true;
}

QByteArray XmlThemeAdapter::toQtMaterialXml(const Theme& theme)
{
    QByteArray out;
    QXmlStreamWriter writer(&out);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement(QStringLiteral("resources"));

    const auto& scheme = theme.colorScheme();

    const auto writeColor = [&writer](const QString& name, const QColor& color) {
        writer.writeStartElement(QStringLiteral("color"));
        writer.writeAttribute(QStringLiteral("name"), name);
        writer.writeCharacters(toHex(color));
        writer.writeEndElement();
    };

    writeColor(QStringLiteral("primaryColor"), scheme.color(ColorRole::Primary));
    writeColor(QStringLiteral("primaryLightColor"), scheme.color(ColorRole::PrimaryContainer));
    writeColor(QStringLiteral("secondaryColor"), scheme.color(ColorRole::Surface));
    writeColor(QStringLiteral("secondaryLightColor"), scheme.color(ColorRole::SurfaceContainerHigh));
    writeColor(QStringLiteral("secondaryDarkColor"), scheme.color(ColorRole::SurfaceVariant));
    writeColor(QStringLiteral("primaryTextColor"), scheme.color(ColorRole::OnPrimary));
    writeColor(QStringLiteral("secondaryTextColor"), scheme.color(ColorRole::OnSurface));

    writer.writeEndElement();
    writer.writeEndDocument();
    return out;
}

bool XmlThemeAdapter::writeQtMaterialXmlFile(
    const Theme& theme,
    const QString& filePath,
    QString* errorString)
{
    QSaveFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return fail(nullptr, errorString, file.errorString());
    }

    const QByteArray xml = toQtMaterialXml(theme);
    if (file.write(xml) != xml.size()) {
        return fail(nullptr, errorString, file.errorString());
    }

    return file.commit();
}

} // namespace QtMaterial
