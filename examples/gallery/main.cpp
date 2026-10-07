#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>

#include "gallerywindow.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("qtmaterial3_gallery"));

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Qt Material 3 Gallery 2.0"));
    parser.addHelpOption();
    QCommandLineOption routeOption(
        {QStringLiteral("r"), QStringLiteral("route")},
        QStringLiteral("Open a Gallery deep link such as /buttons/filled."),
        QStringLiteral("route"));
    parser.addOption(routeOption);
    parser.addPositionalArgument(
        QStringLiteral("route"),
        QStringLiteral("Optional Gallery deep link."));
    parser.process(app);

    GalleryWindow window;
    const QStringList positional = parser.positionalArguments();
    const QString route = parser.isSet(routeOption)
        ? parser.value(routeOption)
        : (positional.isEmpty() ? QString() : positional.first());
    if (!route.isEmpty()) {
        window.navigateToRoute(route);
    }
    window.show();
    return app.exec();
}
